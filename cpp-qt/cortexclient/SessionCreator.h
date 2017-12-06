#ifndef SESSIONCREATOR_H
#define SESSIONCREATOR_H

#include <QObject>
#include "CortexClient.h"

/*
 * Create a session for a headset.
 *
 * It takes care of login/authorize and then create a session.
 * This class doesn't handle errors from the CortexClient object.
 *
 */
class SessionCreator : public QObject
{
    Q_OBJECT

public:
    explicit SessionCreator(QObject *parent = nullptr);

    // create a session for a headset
    void createSession(CortexClient* client, QString headsetId, QString license);

    // break all connections between this object and the Cortex client
    void clear();

signals:
    void sessionCreated(QString token, QString sessionId);

private slots:
    void onGetUserLoginOk(const QStringList &usernames);
    void onLogoutOk();
    void onLoginOk();
    void onAuthorizeOk(QString token);
    void onCreateSessionOk(QString sessionId);

private:
    CortexClient* client;
    QString headsetId;
    QString license;
    QString token;
};

#endif // SESSIONCREATOR_H
