#include "LorretyClient.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LorretyClient w;
    w.show();
    return a.exec();
}
