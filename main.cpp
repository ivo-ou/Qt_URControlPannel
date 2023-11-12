#include "urcontrolpannel.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    URControlPannel w;
    w.show();
    return a.exec();
}
