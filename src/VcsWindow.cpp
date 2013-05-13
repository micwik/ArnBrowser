#include "VcsWindow.hpp"
#include "ui_VcsWindow.h"


VcsWindow::VcsWindow( QSettings* appSettings, QWidget* parent) :
    QDialog( parent),
    _ui( new Ui::VcsWindow)
{
    _ui->setupUi( this);

    this->setWindowTitle( QString("VCS"));
}


VcsWindow::~VcsWindow()
{
    delete _ui;
}
