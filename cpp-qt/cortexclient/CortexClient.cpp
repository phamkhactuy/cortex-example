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
#include "CortexClient.h"

#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QtDebug>
#include <QUrl>
#include <QWebSocket>
#include <QSslError>

class CortexClientPrivate
{
public:
    explicit CortexClientPrivate() {}
    virtual ~CortexClientPrivate(){}


    QWebSocket socket;
    int nextRequestId;

    // the key is a request id
    // the value is the method of the request
    QMap<int, QString> methodForRequestId;
    QString token;
};

// utility function
QStringList arrayToStringList(const QJsonArray &array)
{
    QStringList list;
    for (const QJsonValue &val : array) {
        list.append(val.toString());
    }
    return list;
}

CortexClient::CortexClient(QObject *parent)
    : QObject(parent), d_ptr(new CortexClientPrivate())
{
    Q_D(CortexClient);
    d->nextRequestId = 1;

    // forward the connected/disconnected signals
    connect(&d->socket, &QWebSocket::connected, this, &CortexClient::connected);
    connect(&d->socket, &QWebSocket::disconnected, this, &CortexClient::disconnected);

    // handle errors
    connect(&d->socket, static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error),
            this, &CortexClient::onError, Qt::DirectConnection);
    connect(&d->socket, &QWebSocket::sslErrors, this, &CortexClient::onSslErrors, Qt::DirectConnection);

    // handle incomming text messages
    connect(&d->socket, &QWebSocket::textMessageReceived, this, &CortexClient::onTextMessageReceived);
    connect(&d->socket, &QWebSocket::binaryMessageReceived, this, &CortexClient::onBinaryMessageReceived);
}

CortexClient::~CortexClient()
{

}

QString CortexClient::token()
{
    Q_D(CortexClient);
    return d->token;
}

void CortexClient::setToken(const QString &token)
{
    Q_D(CortexClient);
    d->token = token;
}

void CortexClient::onError(QAbstractSocket::SocketError error)
{
    Q_D(CortexClient);
    qCritical() << "Socket error:" << d->socket.errorString();
}

void CortexClient::onSslErrors(const QList<QSslError> &errors)
{
    Q_D(CortexClient);
    for (const QSslError &error : errors) {
        qCritical() << "SSL error:" << error.errorString();
    }
    //QWebSocket::ignoreSslErrors();
    d->socket.ignoreSslErrors();
}

void CortexClient::open()
{
    Q_D(CortexClient);
    d->socket.open(QUrl("wss://emotivcortex.com:54321"));
}

void CortexClient::close()
{
    Q_D(CortexClient);
    d->socket.close();
    d->nextRequestId = 1;
    d->methodForRequestId.clear();
}

bool CortexClient::isConnected()
{
    Q_D(CortexClient);
    return (d->socket.state() == QAbstractSocket::ConnectedState);
}

void CortexClient::queryHeadsets()
{
    sendRequest("queryHeadsets");
}

void CortexClient::getUserLogin()
{
    sendRequest("getUserLogin");
}

void CortexClient::login(const QString &username, const QString &password,
                         const QString &clientId, const QString &clientSecret)
{
    QJsonObject params;
    params["username"] = username;
    params["password"] = password;
    params["client_id"] = clientId;
    params["client_secret"] = clientSecret;
    sendRequest("login", params);
}

void CortexClient::logout(const QString& username)
{
    QJsonObject params;
    params["username"] = username;
    sendRequest("logout", params);
}

void CortexClient::authorize()
{
    QJsonObject params;
    params["debit"] = 0;
    sendRequest("authorize", params);
}

void CortexClient::authorize(const QString &clientId, const QString &clientSecret, const QString &license)
{
    QJsonObject params;
    params["client_id"] = clientId;
    params["client_secret"] = clientSecret;
    params["license"] = license;
    params["debit"] = 1;
    sendRequest("authorize", params);
}

void CortexClient::queryProfiles()
{
    Q_D(CortexClient);
    QJsonObject params;
    params["_auth"] = d->token;
    sendRequest("queryProfile", params);
}

void CortexClient::setupProfiles(const QString &headset, const QString &profile, const QString &status)
{
    Q_D(CortexClient);
    QJsonObject params;
    params["_auth"] = d->token;
    params["headset"] = headset;
    params["profile"] = profile;
    params["status"] = status;
    sendRequest("setupProfile", params);
}

void CortexClient::querySessions()
{
    Q_D(CortexClient);
    QJsonObject params;
    params["_auth"] = d->token;
    sendRequest("querySessions", params);
}

void CortexClient::createSession(const QString &headsetId, bool activate)
{
    Q_D(CortexClient);
    QJsonObject params;
    params["_auth"] = d->token;
    params["headset"] = headsetId;
    params["status"] = activate ? "active" : "open";
    sendRequest("createSession", params);
}

void CortexClient::closeSession(const QString &sessionId)
{
    Q_D(CortexClient);
    QJsonObject params;
    params["_auth"] = d->token;
    params["session"] = sessionId;
    params["status"] = "close";
    sendRequest("updateSession", params);
}

void CortexClient::updateSession(const QString &sessionId,  const QString &status, const QString &recordingName, const QString &recordingNote, const QString &recordingSubject)
{
    Q_D(CortexClient);
    QJsonObject params;
    params["_auth"] = d->token;
    params["session"] = sessionId;
    params["status"] = status;
    if (recordingName.size())
        params["recordingName"] = recordingName;
    if (recordingNote.size())
    params["recordingNote"] = recordingNote;
    if (recordingSubject.size())
        params["recordingSubject"] = recordingSubject;
    sendRequest("updateSession", params);
}

void CortexClient::updateSessionNote(const QString &sessionId, const QString &note, const QString &record)
{
    Q_D(CortexClient);
    QJsonObject params;
    params["_auth"] = d->token;
    params["session"] = sessionId;
    params["note"] = "note";
    params["record"] = "record";
    sendRequest("updateNote", params);
}

void CortexClient::sessionStartRecord(const QString &sessionId, const QString &recordingName, const QString &recordingNote, const QString &recordingSubject)
{
    Q_D(CortexClient);
    QJsonObject params;
    params["_auth"] = d->token;
    params["session"] = sessionId;
    params["status"] = "startRecord";
    if (recordingName.size())
        params["recordingName"] = recordingName;
    if (recordingNote.size())
    params["recordingNote"] = recordingNote;
    if (recordingSubject.size())
        params["recordingSubject"] = recordingSubject;
    sendRequest("updateSession", params);
}

void CortexClient::sessionStopRecord(const QString &sessionId, const QString &recordingName, const QString &recordingNote, const QString &recordingSubject)
{
    Q_D(CortexClient);
    QJsonObject params;
    params["_auth"] = d->token;
    params["session"] = sessionId;
    params["status"] = "stopRecord";
    if (recordingName.size())
        params["recordingName"] = recordingName;
    if (recordingNote.size())
        params["recordingNote"] = recordingNote;
    if (recordingSubject.size())
        params["recordingSubject"] = recordingSubject;
    sendRequest("updateSession", params);
}

void CortexClient::sessionAddTags(const QString &sessionId, const QStringList& tags)
{
    Q_D(CortexClient);
    return ;
    QJsonObject params;
    params["_auth"] = d->token;
    params["session"] = sessionId;
    params["status"] = "addTags";
    /// TODO:
    //params["tags"] = tags;
    sendRequest("updateSession", params);
}

void CortexClient::sessionRemoveTags(const QString &sessionId, const QStringList& tags)
{
    Q_D(CortexClient);
    QJsonObject params;
    params["_auth"] = d->token;
    params["session"] = sessionId;
    params["status"] = "removeTags";
    sendRequest("updateSession", params);
}

void CortexClient::subscribe(const QString &sessionId, const QString &stream)
{
    Q_D(CortexClient);
    QJsonObject params;
    QJsonArray streamArray;

    streamArray.append(stream);
    params["_auth"] = d->token;
    params["session"] = sessionId;
    params["streams"] = streamArray;

    sendRequest("subscribe", params);
}

void CortexClient::unsubscribe(const QString &sessionId, const QString &stream)
{
    Q_D(CortexClient);
    QJsonObject params;
    QJsonArray streamArray;

    streamArray.append(stream);
    params["_auth"] = d->token;
    params["session"] = sessionId;
    params["streams"] = streamArray;

    sendRequest("unsubscribe", params);
}

void CortexClient::getDetectionInfo(const QString &detection)
{
    Q_D(CortexClient);
    QJsonObject params;
    params["_auth"] = d->token;
    params["detection"] = detection;
    sendRequest("getDetectionInfo", params);
}

void CortexClient::training(const QString &sessionId, const QString &detection,
                            const QString &action, const QString &control)
{
    Q_D(CortexClient);
    QJsonObject params;
    params["_auth"] = d->token;
    params["session"] = sessionId;
    params["detection"] = detection;
    params["action"] = action;
    params["status"] = control;
    sendRequest("training", params);
}

void CortexClient::sendRequest(const QString &method, const QJsonObject &params)
{
    Q_D(CortexClient);
    QJsonObject request;

    // build the request
    request["jsonrpc"] = "2.0";
    request["id"] = d->nextRequestId;
    request["method"] = method;
    request["params"] = params;

    // send the json message
    QString message = QJsonDocument(request).toJson();
    //qDebug() << " * send    " << message;
    d->socket.sendTextMessage(message);

    // remember the method used for this request
    d->methodForRequestId.insert(d->nextRequestId, method);
    d->nextRequestId++;
}
void CortexClient::onBinaryMessageReceived(const QByteArray &message)
{
    Q_D(CortexClient);
    qDebug() << " * Binary received" << message.size();
}

void CortexClient::onTextMessageReceived(const QString &message)
{
    //qDebug() << " * received" << message;
    Q_D(CortexClient);
    // parse the json message
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "error, failed to parse the json message: " << message;
        return;
    }

    QJsonObject response = doc.object();
    int id = response.value("id").toInt(-1);
    QString sid = response.value("sid").toString();

    if (id != -1) {
        // this is a RPC response, we get the method from the id
        // we must know the method in order to understand the result
        QString method = d->methodForRequestId.value(id);
        QJsonValue result = response.value("result");
        QJsonValue error = response.value("error");

        d->methodForRequestId.remove(id);

        if (error.isObject()) {
            emitError(method, error.toObject());
        } else {
            handleResponse(method, result);
        }
    } else if (! sid.isEmpty()) {
        // this message has a sid (subscription id)
        // so this is some data from a data stream
        double time = response.value("time").toDouble();
        QJsonArray data;
        QString stream;

        // find the data field inside the response
        for (auto it = response.begin(); it != response.end(); ++it) {
            QString key = it.key();
            QJsonValue value = it.value();

            if (key != "sid" && key != "time" && value.isArray()) {
                stream = key;
                data = value.toArray();
            }
        }
        emit streamDataReceived(sid, stream, time, data);
    }
}

void CortexClient::handleResponse(const QString &method, const QJsonValue &result)
{
    Q_D(CortexClient);
    if (method == "queryHeadsets") {
        QList<Headset> headsets;

        for (const QJsonValue &val : result.toArray()) {
            QJsonObject jheadset = val.toObject();
            Headset hs(jheadset);
            headsets.append(hs);
        }
        emit queryHeadsetsOk(headsets);
    }
    else if (method == "getUserLogin")
    {
        QStringList usernames;
        QJsonArray array = result.toArray();
        for (const QJsonValue &val : array) {
            usernames.append(val.toString());
        }
        emit getUserLoginOk(usernames);
    }
    else if (method == "login") {
        emit loginOk();
    }
    else if (method == "logout") {
        emit logoutOk();
    }
    else if (method == "authorize") {
        QString token = result.toObject().value("_auth").toString();
        d->token = token;
        emit authorized(token);
    }
    else if (method == "createSession") {
        QString sessionId = result.toObject().value("id").toString();
        emit createSessionOk(sessionId);
    }
    else if (method == "updateSession") {
        QString sessionId = result.toObject().value("id").toString();
        emit closeSessionOk();
    }
    else if (method == "subscribe") {
        QJsonObject sub = result.toArray().first().toObject();
        QString sid = sub.value("sid").toString();

        if (sid.isEmpty()) {
            // it is actually an error!
            emitError(method, sub);
        } else {
            emit subscribeOk(sid);
        }
    }
    else if (method == "unsubscribe") {
        QJsonObject unsub = result.toArray().first().toObject();
        QString msg = unsub.value("message").toString();
        emit unsubscribeOk(msg);
    }
    else if (method == "getDetectionInfo") {
        handleGetDetectionInfo(result);
    }
    else if (method == "training") {
        emit trainingOk(result.toString());
    }
    else {
        // unknown method, so we don't know how to interpret the result
        qCritical() << "unkown RPC method:" << method << result;
    }
}

void CortexClient::handleGetDetectionInfo(const QJsonValue &result)
{
    QJsonArray jactions = result.toObject().value("actions").toArray();
    QJsonArray jcontrols = result.toObject().value("controls").toArray();
    QJsonArray jevents = result.toObject().value("events").toArray();

    QStringList actions = arrayToStringList(jactions);
    QStringList controls = arrayToStringList(jcontrols);
    QStringList events = arrayToStringList(jevents);
    emit getDetectionInfoOk(actions,
                            controls,
                            events);
}

void CortexClient::emitError(const QString &method, const QJsonObject &obj)
{
    int code = obj.value("code").toInt();
    QString error = obj.value("message").toString();
    emit errorReceived(method, code, error);
}
