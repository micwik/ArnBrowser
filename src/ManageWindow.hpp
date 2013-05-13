#ifndef MANAGEWINDOW_HPP
#define MANAGEWINDOW_HPP

#include <ArnLib/ArnPersistSapi.hpp>
#include <ArnLib/MQFlags.hpp>
#include <ArnLib/ArnItem.hpp>
#include <QTimer>
#include <QStringList>
#include <QDialog>

class QSettings;
class QCloseEvent;

namespace Ui {
class ManageWindow;
}


class ArnItemUnidir : public ArnItem
{
public:
    explicit ArnItemUnidir( QObject *parent = 0) : ArnItem( parent)
    {
        setForceKeep();
    }
};


class ManageWindow : public QDialog
{
    Q_OBJECT    
public:
    struct Sel {
        enum E {
            None        = 0x00,
            Normal      = 0x00,
            Pipe        = 0x01,
            Bidir       = 0x02,
            PersistDB   = 0x04,
            PersistFile = 0x08,
            Mandatory   = 0x10
        };
        MQ_DECLARE_FLAGS( Sel)
    };

    explicit ManageWindow( QSettings* appSettings, const QString& path, QWidget* parent = 0);
    ~ManageWindow();
    
private slots:
    void  on_newButton_clicked();
    void  on_deleteButton_clicked();
    void  on_saveButton_clicked();
    void  on_cancelButton_clicked();
    void  on_resetButton_clicked();
    void  on_folderButton_clicked();

    void  doCloseWindow();

    void  lsR( QStringList files);
    void  mandatoryLsR( QStringList files);
    void  doTypeUpdate();
    void  doUpdate();
    void  timeoutReset();
    void  timeoutSave();

private:
    void  doPersistUpdate();

    ArnPersistSapi  _persistSapi;
    QByteArray  _storeValue;
    ArnItemUnidir  _arnPath;
    QString  _path;
    bool  _isNewMode;
    bool  _isPersistFile;
    bool  _isMandatory;

    Ui::ManageWindow* _ui;
};

MQ_DECLARE_OPERATORS_FOR_FLAGS( ManageWindow::Sel)

#endif // MANAGEWINDOW_HPP
