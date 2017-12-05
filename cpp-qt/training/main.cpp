#include <QCoreApplication>
#include "Training.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Training t;

    t.start("mentalCommand");
    //t.start("facialExpression");

    return a.exec();
}
