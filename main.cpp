#include <QCoreApplication>
#include "tests.h"

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

    Tests tests;
    QTest::qExec(&tests);

    //return a.exec();
}
