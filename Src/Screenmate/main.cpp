#include "screenmatewidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ScreenmateWidget w;
    w.show();

    return a.exec();
}
