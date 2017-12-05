#include "HeadsetFinder.h"

HeadsetFinder::HeadsetFinder(QObject *parent) : QObject(parent) {
    client = nullptr;
    timerId = 0;
}

void HeadsetFinder::clear() {
    if (client) {
        disconnect(client, 0, this, 0);
        client = nullptr;
    }
    timerId = 0;
}

void HeadsetFinder::findHeadsets(CortexClient* client) {
    this->client = client;
    connect(client, &CortexClient::queryHeadsetsOk, this, &HeadsetFinder::onQueryHeadsetsOk);
    timerId = startTimer(1000);
}

void HeadsetFinder::timerEvent(QTimerEvent *event) {
    if (event->timerId() == timerId) {
        qInfo() << "Looking for headsets...";
        client->queryHeadsets();
    }
}

void HeadsetFinder::onQueryHeadsetsOk(const QList<Headset> &headsets) {
    if (headsets.isEmpty()) {
        //qInfo() << "No headset found. Please, connect a headset.";
    }
    else {
        qInfo() << headsets.size() << "headset(s) found:";
        for (Headset hs : headsets) {
            qInfo() << "\t" << hs.toString();
        }

        killTimer(timerId);
        emit headsetsFound(headsets);
    }
}
