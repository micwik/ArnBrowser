#include <QApplication>
#include "MainWindow.hpp"

#if QT_VERSION >= 0x050000
    const QString QtVerTxt = "(Qt5)";
#else
    const QString QtVerTxt = "(Qt4)";
#endif

    extern const QString ver = "2.5-d1  " + QtVerTxt;  // Don't forget to change in setup.iss -file


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
