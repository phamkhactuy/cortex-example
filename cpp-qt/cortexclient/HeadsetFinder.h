#ifndef HEADSETFINDER_H
#define HEADSETFINDER_H

#include <QObject>
#include <QTimerEvent>
#include "CortexClient.h"
#include "Headset.h"

/*
 * Find all headsets connected to this device.
 *
 * It looks for connected headsets every second, until it finds one.
 *
 */
class HeadsetFinder : public QObject
{
    Q_OBJECT

public:
    explicit HeadsetFinder(QObject *parent = nullptr);

    void findHeadsets(CortexClient* client);

    // break all connections between this object and the Cortex client
    void clear();

signals:
    void headsetsFound(const QList<Headset> &headsets);

protected:
    void timerEvent(QTimerEvent *event);

private slots:
    void onQueryHeadsetsOk(const QList<Headset> &headsets);

private:
    CortexClient* client;
    int timerId;
};

#endif // HEADSETFINDER_H
