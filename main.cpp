#include <QApplication>

#include "AppWindow.h"

int main( int argc, char * argv[] )
{
    QApplication a(argc, argv);
    AppWindow w;
    w.show();
    return a.exec();
}
