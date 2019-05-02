#include "screenmatewidget.h"
#include <QApplication>

#include <QTextCodec>
#include <QSystemTrayIcon>
#include <QMessageBox>

/**
 * @brief setLocalization установка поддержки кириллицы
 * @param name кодировка исходного текста
 */
void setLocalization(const char *name)
{
    QTextCodec *codec = QTextCodec::codecForName(name);
    QTextCodec::setCodecForTr(codec);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setLocalization("UTF-8");

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(
                    0,
                    QApplication::applicationName(),
                    "couldn't detect any system tray on this platform");
        return EXIT_FAILURE;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    ScreenmateWidget w;
    w.show();

    return a.exec();
}
