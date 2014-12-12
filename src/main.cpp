#include <QApplication>
#include "MainWindow.hpp"
#include <QIcon>

#if QT_VERSION >= 0x050000
    const QString QtVerTxt = "(Qt5)";
#else
    const QString QtVerTxt = "(Qt4)";
#endif

    extern const QString ver = "2.5-d4  " + QtVerTxt;  // Don't forget to change in setup.iss -file


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIcon appIcon("://pic/arn_browser_128.png");
    a.setWindowIcon( appIcon);

    MainWindow w;
    w.show();
    return a.exec();
}
