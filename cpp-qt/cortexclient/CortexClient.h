#ifndef CORTEXCLIENT_H
#define CORTEXCLIENT_H

#include <QObject>
#include <QWebSocket>
#include <QString>
#include <QList>
#include <QStringList>
#include <QMap>
#include <QJsonObject>
#include <QSslError>
#include "Headset.h"

/*
 * A simple client for the Cortex service.
 *
 */
class CortexClient : public QObject
{
    Q_OBJECT

public:
    explicit CortexClient(QObject *parent = nullptr);

public slots:
    void open();
    void close();

    // list all the headsets connected to your device
    void queryHeadsets();

    // login / logout
    void getUserLogin();
    void login(QString username, QString password,
               QString clientId, QString clientSecret);
    void logout(QString username);

    // get an authorization token
    void authorize();
    void authorize(QString clientId, QString clientSecret, QString license);

    // open a session, so we can then get data from a headset
    void createSession(QString token, QString headsetId, bool activate);
    void closeSession(QString token, QString sessionId);

    // subscribe to a data stream
    void subscribe(QString token, QString sessionId, QString stream);
    void unsubscribe(QString token, QString sessionId, QString stream);

    // methods for training
    void getDetectionInfo(QString detection);
    void training(QString token, QString sessionId, QString detection,
                  QString action, QString control);

signals:
    void connected();
    void disconnected();

    void queryHeadsetsOk(const QList<Headset> &headsets);
    void getUserLoginOk(const QStringList &usernames);
    void loginOk();
    void logoutOk();
    void authorizeOk(QString authToken);
    void createSessionOk(QString sessionId);
    void closeSessionOk();
    void subscribeOk(QString sessionId);
    void unsubscribeOk(QString msg);
    void getDetectionInfoOk(QStringList actions,
                            QStringList controls, QStringList events);
    void trainingOk(QString msg);

    // we received an error message in response to a RPC request
    void errorReceived(QString method, int code, QString error);

    // we received data from a data stream
    void streamDataReceived(QString sessionId, QString stream,
                            double time, const QJsonArray &data);

private slots:
    void onError(QAbstractSocket::SocketError error);
    void onSslErrors(const QList<QSslError> &errors);
    void onMessageReceived(QString message);

private:
    // a generic method to send a RPC request to Cortex
    void sendRequest(QString method, QJsonObject params = QJsonObject());

    // handle the response to a RPC request
    void handleResponse(QString method, const QJsonValue &result);
    void handleGetDetectionInfo(const QJsonValue &result);

    void emitError(QString method, const QJsonObject &obj);

private:
    QWebSocket socket;
    int nextRequestId;

    // the key is a request id
    // the value is the method of the request
    QMap<int, QString> methodForRequestId;
};

#endif // CORTEXCLIENT_H
