#ifndef TRAINING_H
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
#define TRAINING_H

#include <QObject>
#include <QList>
#include "CortexClient.h"
#include "HeadsetFinder.h"
#include "SessionCreator.h"

/*
 * Training for the mental command or the facial expressions.
 *
 */
class Training : public QObject
{
    Q_OBJECT

public:
    explicit Training(QObject *parent = nullptr);

    // detection must be "mentalCommand" or "facialExpression"
    void start(const QString &detection);

private slots:
    void onConnected();
    void onDisconnected();
    void onErrorReceived(const QString &method, int code, const QString &error);

    void onGetUserLogin(const QStringList &usernames);
    void onLogout();
    void onLogin();
    void onAuthorized(const QString &token);

    void onGetDetectionInfo(const QStringList &actions,
                              const QStringList &controls,
                              const QStringList &events);

    void onHeadsetsFound(const QList<Headset> &headsets);
    void onSessionCreated(const QString &sessionId);
    void onSubscribe(const QString &sid);
    void onTraining(const QString &msg);
    void onStreamDataReceived(const QString &sessionId, const QString &stream,
                              double time, const QJsonArray &data);

private:
    QString action() {
        return actions.at(actionIndex);
    }
    void nextAction();
    void retryAction();
    bool isEvent(const QJsonArray &data, const QString &event);

private:
    CortexClient client;
    HeadsetFinder finder;
    SessionCreator creator;

    QString detection;
    QStringList actions;

    QString headsetId;    
    QString sessionId;
    int actionIndex;
    int trainingFailure;
};

#endif // TRAINING_H
