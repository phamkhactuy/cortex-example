#include "Headset.h"

Headset::Headset() {
}

Headset::Headset(const QJsonObject &jheadset) {
    fromJson(jheadset);
}

void Headset::fromJson(const QJsonObject &jheadset) {
    id = jheadset["id"].toString();
    label = jheadset["label"].toString();
    connectedBy = jheadset["connectedBy"].toString();
    status = jheadset["status"].toString();
}

QString Headset::toString() {
    return QString("%1, %2 (%3)").arg(id, status, connectedBy);
}
