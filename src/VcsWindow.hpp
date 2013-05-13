#ifndef VCSWINDOW_HPP
#define VCSWINDOW_HPP

#include "ArnLib/Arn.hpp"
#include <QDialog>

class QSettings;
class QCloseEvent;

namespace Ui {
class VcsWindow;
}


class VcsWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit VcsWindow( QSettings* appSettings, QWidget* parent = 0);
    ~VcsWindow();
    
private:
    Ui::VcsWindow* _ui;
};

#endif // VCSWINDOW_HPP
