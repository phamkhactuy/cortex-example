#include <QCoreApplication>
#include <QtDebug>
#include "DataStreamExample.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    DataStreamExample dse;

    qInfo() << "";
    qInfo() << "#####";
    qInfo() << "Reminder: to subscribe to the EEG data stream, you must get an appropriate licence from Emotiv.";
    qInfo() << "#####";
    qInfo() << "";

    // TODO use your Emotiv license to get EEG data
    dse.start("eeg", "your Emotiv license goes here");
    return a.exec();
}
