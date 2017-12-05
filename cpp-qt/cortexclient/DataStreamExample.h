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
    void start(QString stream, QString license = "");

private slots:
    void onConnected();
    void onDisconnected();
    void onErrorReceived(QString method, int code, QString error);

    void onHeadsetsFound(const QList<Headset> &headsets);
    void onSessionCreated(QString token, QString sessionId);

    void onSubscribeOk(QString sid);
    void onUnsubscribeOk(QString msg);

    void onStreamDataReceived(QString sessionId, QString stream,
                              double time, const QJsonArray &data);

    void onCloseSessionOk();

protected:
    void timerEvent(QTimerEvent *event);

private:
    CortexClient client;
    HeadsetFinder finder;
    SessionCreator creator;

    QString license;
    QString stream;
    QString headsetId;
    QString token;
    QString sessionId;
    double nextDataTime;
    int timerId;
};

#endif // DATASTREAMEXAMPLE_H
