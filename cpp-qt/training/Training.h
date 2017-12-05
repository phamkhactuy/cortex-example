#ifndef TRAINING_H
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
    void start(QString detection);

private slots:
    void onConnected();
    void onDisconnected();
    void onErrorReceived(QString method, int code, QString error);

    void onGetDetectionInfoOk(QStringList actions,
                              QStringList controls,
                              QStringList events);

    void onHeadsetsFound(const QList<Headset> &headsets);
    void onSessionCreated(QString token, QString sessionId);
    void onSubscribeOk(QString sid);
    void onTrainingOk(QString msg);
    void onStreamDataReceived(QString sessionId, QString stream,
                              double time, const QJsonArray &data);

private:
    QString action() {
        return actions.at(actionIndex);
    }
    void nextAction();
    void retryAction();
    bool isEvent(const QJsonArray &data, QString event);

private:
    CortexClient client;
    HeadsetFinder finder;
    SessionCreator creator;

    QString detection;
    QStringList actions;

    QString headsetId;
    QString token;
    QString sessionId;
    int actionIndex;
    int trainingFailure;
};

#endif // TRAINING_H
