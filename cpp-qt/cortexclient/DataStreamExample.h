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
#ifndef DATASTREAMEXAMPLE_H
#define DATASTREAMEXAMPLE_H

#include <QObject>
#include "CortexClient.h"
#include "HeadsetFinder.h"
#include "SessionCreator.h"
#include "Headset.h"

/*
 * Connects to a headset and displays the data from a stream.
 *
 */
class DataStreamExample : public QObject
{
    Q_OBJECT

public:
    explicit DataStreamExample(QObject *parent = nullptr);

    // you need a license if you want to get the EEG data
    void start(const QString &stream, const QString &license = "");

private slots:
    void onConnected();
    void onDisconnected();
    void onErrorReceived(const QString& method, int code, const QString &error);

    void onGetUserLogin(const QStringList &usernames);
    void onLogout();
    void onLogin();
    void onAuthorized(const QString &token);

    void onHeadsetsFound(const QList<Headset> &headsets);
    void onSessionCreated(const QString &sessionId);

    void onSubscribe(const QString &sid);
    void onUnsubscribe(const QString &msg);

    void onStreamDataReceived(const QString &sessionId, const QString &stream,
                              double time, const QJsonArray &data);

    void onCloseSession();

protected:
    void timerEvent(QTimerEvent *event);

private:
    CortexClient client;
    HeadsetFinder finder;
    SessionCreator creator;

    QString license;
    QString stream;
    QString headsetId;    
    QString sessionId;
    double nextDataTime;
    int timerId;
};

#endif // DATASTREAMEXAMPLE_H
