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
#include "Training.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QtDebug>


const QString clientId = "the client id of your Cortex app";
const QString clientSecret = "the client secret of your Cortex app";

/*
 * As a developer, you can use your personal EmotivID to run the examples.
 * But in a real application, you should ask your users to login
 * with their own EmotivID.
 */
const QString username = "a EmotivID";
const QString password = "a password";
const QString license =  "";

Training::Training(QObject *parent) : QObject(parent)
{
    connect(&client, &CortexClient::connected, this, &Training::onConnected);
    connect(&client, &CortexClient::disconnected, this, &Training::onDisconnected);
    connect(&client, &CortexClient::errorReceived, this, &Training::onErrorReceived);

    connect(&client, &CortexClient::getUserLoginOk, this, &Training::onGetUserLogin);
    connect(&client, &CortexClient::logoutOk, this, &Training::onLogout);
    connect(&client, &CortexClient::loginOk, this, &Training::onLogin);
    connect(&client, &CortexClient::authorized, this, &Training::onAuthorized);

    connect(&client, &CortexClient::getDetectionInfoOk, this, &Training::onGetDetectionInfo);
    connect(&client, &CortexClient::subscribeOk, this, &Training::onSubscribe);
    connect(&client, &CortexClient::trainingOk, this, &Training::onTraining);
    connect(&client, &CortexClient::streamDataReceived, this, &Training::onStreamDataReceived);

    connect(&finder, &HeadsetFinder::headsetsFound, this, &Training::onHeadsetsFound);
    connect(&creator, &SessionCreator::sessionCreated, this, &Training::onSessionCreated);
}

void Training::start(const QString &detection)
{
    this->detection = detection;
    actionIndex = 0;
    trainingFailure = 0;
    client.open();
}

void Training::onConnected()
{
    qInfo() << "Connected to Cortex.";
    client.getUserLogin();
}

void Training::onDisconnected()
{
    qInfo() << "Disconnected.";
    QCoreApplication::quit();
}

void Training::onErrorReceived(const QString &method, int code, const QString &error)
{
    qCritical() << "Cortex returned an error:";
    qCritical() << "\t" << method << code << error;
    QCoreApplication::quit();
}

void Training::onGetUserLogin(const QStringList &usernames)
{
    if (usernames.isEmpty()) {
        // no one is logged in, so no need for a logout
        onLogout();
    } else {
        onLogin();
    }
}

void Training::onLogout()
{
    // now, we can login
    client.login(username, password, clientId, clientSecret);
}

void Training::onLogin()
{
    if (license.isEmpty()) {
        client.authorize();
    } else {
        client.authorize(clientId, clientSecret, license);
    }
}

void Training::onAuthorized(const QString &token)
{
    qInfo() << "Authorize successful, token: " << token;
    if (token.size()) {
        client.getDetectionInfo(detection);
    }
}

void Training::onGetDetectionInfo(const QStringList &actions,
                                    const QStringList &controls,
                                    const QStringList &events)
{
    this->actions = actions;
    qInfo() << "Information for" << detection << ":";
    qInfo() << "Actions " << actions;
    qInfo() << "Controls" << controls;
    qInfo() << "Events  " << events;
    finder.findHeadsets(&client);
}

void Training::onHeadsetsFound(const QList<Headset> &headsets)
{
    headsetId = headsets.first().id;
    finder.clear();
    creator.createSession(&client, headsetId, "");
}

void Training::onSessionCreated(const QString &sessionId)
{
    this->sessionId = sessionId;
    creator.clear();
    client.subscribe(sessionId, "sys");
}

void Training::onSubscribe(const QString &sid)
{
    qInfo() << "Subscription to sys stream successful, sid" << sid;
    client.training(sessionId, detection, action(), "start");
}

void Training::onTraining(const QString &msg)
{
    Q_UNUSED(msg);
    // this signal is not important
    // instead we need to watch the events from the sys stream
}

void Training::onStreamDataReceived(const QString &sessionId, const QString &stream,
                                    double time, const QJsonArray &data)
{
    Q_UNUSED(sessionId);
    Q_UNUSED(stream);
    Q_UNUSED(time);
    //qDebug() << " * sys data:" << data;

    if (isEvent(data, "Started")) {
        qInfo() << "";
        qInfo() << "Please, focus on the action" << action().toUpper()
                << "for a few seconds.";
    }
    else if (isEvent(data, "Succeeded")) {
        // the training of this action is a success
        // we "accept" it, and then we will receive the "Completed" event
        client.training(sessionId, detection, action(), "accept");
    }
    else if (isEvent(data, "Failed")) {
        retryAction();
    }
    else if (isEvent(data, "Completed")) {
        qInfo() << "Well done! You successfully trained " << action();
        nextAction();
    }
}

void Training::nextAction()
{
    actionIndex++;
    trainingFailure = 0;

    if (actionIndex < 3 && actionIndex < actions.size()) {
        // ok, let's train the next action
        client.training(sessionId, detection, action(), "start");
    }
    else {
        // that's enough training for today
        qInfo() << "Done.";
        QCoreApplication::quit();
    }
}

void Training::retryAction()
{
    trainingFailure++;

    if (trainingFailure < 3) {
        qInfo() << "Sorry, it didn't work. Let's try again.";
        client.training(sessionId, detection, action(), "start");
    }
    else {
        qInfo() << "It seems you are struggling with this action. Let's try another one.";
        nextAction();
    }
}

bool Training::isEvent(const QJsonArray &data, const QString &event)
{
    for (const QJsonValue &val : data) {
        QString str = val.toString();
        if (str.endsWith(event)) {
            return true;
        }
    }
    return false;
}
