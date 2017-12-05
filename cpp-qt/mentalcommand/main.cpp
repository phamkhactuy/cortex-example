#include <QCoreApplication>
#include "DataStreamExample.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    DataStreamExample dse;

    dse.start("com");
    return a.exec();
}
