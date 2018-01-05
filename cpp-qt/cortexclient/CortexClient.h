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
#ifndef CORTEXCLIENT_H
#define CORTEXCLIENT_H

#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>
#include <QWebSocket>
#include "Headset.h"

/*
 * A simple client for the Cortex service.
 *
 */
class CortexClientPrivate;
class CortexClient : public QObject
{
    Q_OBJECT
public:
    explicit CortexClient(QObject *parent = nullptr);
    virtual ~CortexClient();
public slots:
    void open();
    void close();

    // login / logout
    void getUserLogin();
    void login(const QString &username, const QString &password,
               const QString &clientId, const QString &clientSecret);
    void logout(const QString &username);

    // get an authorization token
    void authorize();
    void authorize(const QString &clientId, const QString &clientSecret, const QString &license);
    void setToken(const QString &token);
    QString token();
    // void newToken();

    void queryProfiles();
    void setupProfiles(const QString &headset, const QString &profile, const QString &status);

    // list all the headsets connected to your device
    void queryHeadsets();

    // open a session, so we can then get data from a headset
    // you need a license to activate the session
    void querySessions();
    void createSession(const QString &headsetId, bool activate);
    void sessionStartRecord(const QString &sessionId, const QString &recordingName, const QString &recordingNote, const QString &recordingSubject);
    void sessionStopRecord(const QString &sessionId, const QString &recordingName, const QString &recordingNote, const QString &recordingSubject);
    void updateSession(const QString &sessionId,  const QString &status, const QString &recordingName, const QString &recordingNote, const QString &recordingSubject);
    void sessionAddTags(const QString &sessionId, const QStringList& tags);
    void sessionRemoveTags(const QString &sessionId, const QStringList& tags);
    void closeSession(const QString &sessionId);
    void updateSessionNote(const QString &sessionId, const QString &note, const QString &record);
    //void injectMarker();

    // subscribe to a data stream
    void subscribe(const QString &sessionId, const QString &stream);
    void unsubscribe(const QString &sessionId, const QString &stream);

    // methods for training
    void getDetectionInfo(const QString &detection);
    void training(const QString &sessionId, const QString &detection,
                  const QString &action, const QString &control);
    bool isConnected();
signals:
    void connected();
    void disconnected();

    void queryHeadsetsOk(const QList<Headset> &headsets);
    void getUserLoginOk(const QStringList &usernames);
    void loginOk();
    void logoutOk();
    void authorized(const QString &authToken);

    void createSessionOk(const QString &sessionId);
    void closeSessionOk();

    void subscribeOk(const QString &sessionId);
    void unsubscribeOk(const QString &msg);

    void getDetectionInfoOk(QStringList &actions,
                            QStringList &controls, QStringList &events);
    void trainingOk(const QString &msg);

    // we received an error message in response to a RPC request
    void errorReceived(const QString &method, int code, const QString &error);

    // we received data from a data stream
    void streamDataReceived(const QString &sessionId, const QString &stream,
                            double time, const QJsonArray &data);

private slots:
    void onError(QAbstractSocket::SocketError error);
    void onSslErrors(const QList<QSslError> &errors);
    void onTextMessageReceived(const QString &message);
    void onBinaryMessageReceived(const QByteArray &message);

private:
    // a generic method to send a RPC request to Cortex
    void sendRequest(const QString &method, const QJsonObject &params = QJsonObject());

    // handle the response to a RPC request
    void handleResponse(const QString &method, const QJsonValue &result);
    void handleGetDetectionInfo(const QJsonValue &result);

    void emitError(const QString &method, const QJsonObject &obj);

private:
    Q_DECLARE_PRIVATE(CortexClient)
    QScopedPointer<CortexClientPrivate> d_ptr;

};

#endif // CORTEXCLIENT_H
