#ifndef HEADSET_H
#define HEADSET_H

#include <QString>
#include <QJsonObject>

/*
 * Store basic information about a Emotiv headset.
 *
 */
class Headset
{
public:
    Headset();
    Headset(const QJsonObject &jheadset);

    void fromJson(const QJsonObject &jheadset);
    QString toString();

public:
    QString id;
    QString label;
    QString connectedBy;
    QString status;
};

#endif // HEADSET_H
