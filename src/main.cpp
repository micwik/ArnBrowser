#include <QApplication>
#include "MainWindow.hpp"

extern const QString ver = "2.0";  // Don't forget to change in setup.iss -file


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
