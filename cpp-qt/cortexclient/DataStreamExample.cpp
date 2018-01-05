/***************
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
***************/
#include "DataStreamExample.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtDebug>

/*
 * To get a client id and a client secret, you must connect to your Emotiv
 * account on emotiv.com and create a Cortex app.
 */
const QString clientId = "the client id of your Cortex app";
const QString clientSecret = "the client secret of your Cortex app";

/*
 * As a developer, you can use your personal EmotivID to run the examples.
 * But in a real application, you should ask your users to login
 * with their own EmotivID.
 */
const QString username = "a EmotivID";
const QString password = "a password";

DataStreamExample::DataStreamExample(QObject *parent)
    : QObject(parent)
{
    connect(&client, &CortexClient::connected, this, &DataStreamExample::onConnected);
    connect(&client, &CortexClient::disconnected, this, &DataStreamExample::onDisconnected);
    connect(&client, &CortexClient::errorReceived, this, &DataStreamExample::onErrorReceived);
    connect(&client, &CortexClient::subscribe, this, &DataStreamExample::onSubscribe);
    connect(&client, &CortexClient::unsubscribe, this, &DataStreamExample::onUnsubscribe);
    connect(&client, &CortexClient::streamDataReceived, this, &DataStreamExample::onStreamDataReceived);
    connect(&client, &CortexClient::closeSession, this, &DataStreamExample::onCloseSession);

    connect(&client, &CortexClient::getUserLoginOk, this, &DataStreamExample::onGetUserLogin);
    connect(&client, &CortexClient::logoutOk, this, &DataStreamExample::onLogout);
    connect(&client, &CortexClient::loginOk, this, &DataStreamExample::onLogin);
    connect(&client, &CortexClient::authorized, this, &DataStreamExample::onAuthorized);

    connect(&finder, &HeadsetFinder::headsetsFound, this, &DataStreamExample::onHeadsetsFound);
    connect(&creator, &SessionCreator::sessionCreated, this, &DataStreamExample::onSessionCreated);
}

void DataStreamExample::start(const QString &stream, const QString &license)
{
    this->stream = stream;
    this->license = license;
    nextDataTime = 0;
    timerId = 0;
    client.open();
}

void DataStreamExample::onConnected()
{
    qInfo() << "Connected to Cortex.";
    /// After connected to Cortex, client must obtain token for working with cortex, by using one of below actions:
    //  client.getUserLogin(): then logout, then login with username/password;
    //  client.login(): Login with username/password;
    //  client.authorize(): Get token from Cortex
    //  client.authorize(clientId, clientSecrect, liencense);
    //  client.setToken("You token"): Tell client to use old token (which be saved before)

    // first step: get the current user
    // Note: if you already have a token, you can reuse it
    // so you can skip the login procedure and call onAuthorizeOk("your token")

    //client.authorize();
    client.getUserLogin();
    finder.findHeadsets(&client);
}

void DataStreamExample::onDisconnected() {
    qInfo() << "Disconnected.";
    QCoreApplication::quit();
}

void DataStreamExample::onErrorReceived(const QString &method, int code, const QString &error) {
    qCritical() << "Cortex returned an error:";
    qCritical() << "\t" << method << code << error;
    QCoreApplication::quit();
}

void DataStreamExample::onGetUserLogin(const QStringList &usernames)
{
    if (usernames.isEmpty()) {
        // no one is logged in, so no need for a logout
        onLogout();
    } else if (usernames.contains(username)) {
        // we are already logged in
        // we can skip the logout/login steps
        onLogin();
    } else {
        // logout the current user before we can login, or use current user
        //client.logout(usernames.first());
        onLogin();
    }
}

void DataStreamExample::onLogout()
{
    // now, we can login
    client.login(username, password, clientId, clientSecret);
}

void DataStreamExample::onLogin()
{
    if (license.isEmpty()) {
        client.authorize();
    } else {
        client.authorize(clientId, clientSecret, license);
    }
}

void DataStreamExample::onAuthorized(const QString &token)
{
    qInfo() << "Authorize successful, token: " << token;
    // next step: open a session for the headset
}

void DataStreamExample::onHeadsetsFound(const QList<Headset> &headsets)
{
    finder.clear();

    // we take the first headset
    // TODO in a real application, you should ask the user to choose a headset from the list
    this->headsetId = headsets.first().id;

    // next step: create a session for this headset
    creator.createSession(&client, headsetId, license);
}

void DataStreamExample::onSessionCreated(const QString &sessionId)
{
    creator.clear();    
    this->sessionId = sessionId;

    // next step: subscribe to a data stream
    client.subscribe(sessionId, stream);
}

void DataStreamExample::onSubscribe(const QString &sid)
{
    qInfo() << "Subscription successful, sid" << sid;
    qInfo() << "Receiving data for 30 seconds.";
    timerId = startTimer(30*1000);
}

void DataStreamExample::onStreamDataReceived(
        const QString &sessionId, const QString &stream, double time, const QJsonArray &data)
{
    Q_UNUSED(sessionId);
    // a data stream can publish a lot of data
    // we display only a few data per second
    if (time >= nextDataTime) {
        qInfo() << stream << data;
        nextDataTime = time + 0.25;
    }
}

void DataStreamExample::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerId) {
        killTimer(timerId);
        client.unsubscribe(sessionId, stream);
    }
}

void DataStreamExample::onUnsubscribe(const QString &msg)
{
    qInfo() << "Subscription cancelled:" << msg;
    client.closeSession(sessionId);
}

void DataStreamExample::onCloseSession()
{
    qInfo() << "Session closed.";
    client.close();
}
