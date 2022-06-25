#include "controlpannel.h"

#include <QApplication>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    ControlPannel w;
    w.show();
    return a.exec();
}
