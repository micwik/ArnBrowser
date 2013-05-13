#include "ManageWindow.hpp"
#include "ui_ManageWindow.h"
#include <QSettings>
#include <QRegExpValidator>

ManageWindow::ManageWindow( QSettings* appSettings, const QString& path, QWidget* parent) :
    QDialog( parent),
    _ui( new Ui::ManageWindow)
{
    _ui->setupUi( this);
    this->setWindowTitle( QString("Manage ") + path);

    _path          = path;
    _isNewMode     = false;
    _isPersistFile = false;
    _isMandatory   = false;

    QRegExp  rx("^[^/ ]+([^ ]*[^/ ])*$");
    QRegExpValidator*  validator = new QRegExpValidator( rx, this);
    _ui->itemEdit->setValidator( validator);

    //// Logics
    _persistSapi.open("//.sys/Persist/Pipes/CommonPipe");
    connect( &_persistSapi, SIGNAL(rq_lsR(QStringList)), this, SLOT(lsR(QStringList)));
    connect( &_persistSapi, SIGNAL(rq_dbMandatoryLsR(QStringList)), this, SLOT(mandatoryLsR(QStringList)));

    _arnPath.open( path);
    connect( &_arnPath, SIGNAL(arnLinkDestroyed()), this, SLOT(doUpdate()));
    connect( _ui->armDelButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    connect( _ui->folderButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    connect( _ui->typeNormalButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    connect( _ui->typeBidirButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    connect( _ui->typePipeButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    connect( _ui->persistNoneButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    connect( _ui->persistDbButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    connect( _ui->persistFileButton, SIGNAL(clicked()), this, SLOT(doUpdate()));
    //_wt->finished().connect( this, &ManageWindow::doCloseWindow);

    if (!_arnPath.isFolder()) {
        emit _persistSapi.pv_ls( path);  // Will finally trigger update of persist
        emit _persistSapi.pv_dbMandatoryLs( path);  // Will finally trigger update of mandatory
    }

    doTypeUpdate();
}

ManageWindow::~ManageWindow()
{
    delete _ui;
}


void  ManageWindow::doUpdate()
{
    bool  isPathFolder = _arnPath.isFolder();
    bool  isPathOpen   = _arnPath.isOpen();

    _ui->pathEdit->setText( isPathOpen ? _path : QString());

    if (!isPathOpen) {  // Path link has been destroyed
        _isNewMode = false;
        _ui->itemEdit->setText("");
        _ui->itemEdit->setDisabled(true);
        _ui->folderButton->setChecked(false);
        _ui->folderButton->setDisabled(true);
        _ui->armDelButton->setChecked(false);
        _ui->armDelButton->setDisabled(true);
    }
    else if (!_arnPath.isFolder()) {  // Edit non folder item
        _ui->itemEdit->setText( _arnPath.name( ArnLink::NameF()));
        _ui->itemEdit->setDisabled(true);
        _ui->folderButton->setChecked(false);
        _ui->folderButton->setDisabled(true);
        if (_arnPath.isPipeMode()) {

        }
    }
    else if (!_isNewMode) {  // View folder
        _ui->itemEdit->setText("");
        _ui->itemEdit->setDisabled(true);
        _ui->folderButton->setChecked(true);
        _ui->folderButton->setDisabled(true);
        _ui->armDelButton->setChecked(false);
        _ui->armDelButton->setDisabled(true);
    }
    else {  // Edit new non folder item
        _ui->itemEdit->setDisabled(false);
        _ui->folderButton->setDisabled(true);  // No support yet for new folder (will not sync)
        _ui->armDelButton->setChecked(false);
        _ui->armDelButton->setDisabled(true);
    }

    bool  isFolder          = _ui->folderButton->isChecked();
    bool  isTypePersistable = !isFolder && !_ui->typePipeButton->isChecked();
    bool  isPersistDB       = isTypePersistable && (_ui->persistDbButton->isChecked());

    _ui->newButton->setEnabled( !_isNewMode && isPathFolder);
    _ui->saveButton->setEnabled((_isNewMode || !isPathFolder) && isPathOpen);
    _ui->cancelButton->setEnabled( _isNewMode);
    _ui->deleteButton->setEnabled( _ui->armDelButton->isChecked());
    _ui->resetButton->setEnabled(false);  // Default

    _ui->typeSelBox->setDisabled( isFolder || !isPathOpen);
    _ui->persistSelBox->setDisabled( !isTypePersistable || !isPathOpen);
    _ui->mandatoryButton->setDisabled( !isPersistDB);

    ArnItem::Mode  modePath = _arnPath.getMode();
    if (!isPathFolder && isPathOpen) {
        if (modePath.is( modePath.Pipe)) {
            _ui->typeNormalButton->setDisabled(true);
            _ui->typeBidirButton->setDisabled(true);
            _ui->resetButton->setEnabled( true);
        }
        else if (modePath.is( modePath.BiDir)) {
            _ui->typeNormalButton->setDisabled(true);
            _ui->typeBidirButton->setDisabled(false);
            _ui->resetButton->setEnabled( true);
        }
        else {  // Normal type
            _ui->typeNormalButton->setDisabled(false);
            _ui->typeBidirButton->setDisabled(false);
        }

        if (_isPersistFile) {
            _ui->typePipeButton->setDisabled(true);
        }
        else if (modePath.is( modePath.Save)) {
            _ui->typePipeButton->setDisabled(true);
            _ui->persistNoneButton->setDisabled(true);
            _ui->resetButton->setEnabled( true);
        }
        else {  // No persist
            _ui->typePipeButton->setDisabled(false);
            _ui->persistNoneButton->setDisabled(false);
        }
    }

    //_app->triggerUpdate();
}


void  ManageWindow::doTypeUpdate()
{
    ArnItem::Mode  pathMode = _arnPath.getMode();
    if (!_arnPath.isFolder()) {
        if (pathMode.is( pathMode.Pipe))
            _ui->typePipeButton->setChecked(true);
        else if (pathMode.is( pathMode.BiDir))
            _ui->typeBidirButton->setChecked(true);
        else
            _ui->typeNormalButton->setChecked(true);
    }
    else {
        _ui->typeSelBox->setChecked(false);
    }

    doUpdate();
}


void  ManageWindow::doPersistUpdate()
{
    ArnItem::Mode  modePath = _arnPath.getMode();
    if (_isPersistFile)
        _ui->persistFileButton->setChecked(true);
    else if (modePath.is( modePath.Save))
        _ui->persistDbButton->setChecked(true);
    else
        _ui->persistNoneButton->setChecked(true);

    _ui->mandatoryButton->setChecked( _isMandatory);

    doUpdate();
}


void  ManageWindow::timeoutSave()
{
    _arnPath.arnImport( _storeValue);

    doUpdate();
}


void  ManageWindow::timeoutReset()
{
    _arnPath.open( _path);
    _arnPath.arnImport( _storeValue);
    emit _persistSapi.pv_ls( _path);  // Will finally trigger update of persist
    emit _persistSapi.pv_dbMandatoryLs( _path);  // Will finally trigger update of mandatory

    doTypeUpdate();
}


void  ManageWindow::on_newButton_clicked()
{
    _isNewMode = true;
    _isPersistFile = false;

    _ui->itemEdit->setText("");
    _ui->itemEdit->setFocus();
    _ui->folderButton->setChecked(false);
    _ui->typeNormalButton->setChecked(true);
    _ui->persistNoneButton->setChecked(true);

    doUpdate();
}


void  ManageWindow::on_deleteButton_clicked()
{
    if (!_arnPath.isFolder()) {
        emit _persistSapi.pv_rm( _arnPath.path());  // Any persist file is deleted
        if (_arnPath.isSaveMode())
            emit _persistSapi.pv_dbMandatory( _arnPath.path(), false);
        emit _persistSapi.pv_ls( _arnPath.path());
        _arnPath.destroyLink();
    }

    doUpdate();
}


void  ManageWindow::on_saveButton_clicked()
{
    ArnItem  arnItemNew;
    ArnItem*  arnItem;

    if (_isNewMode) {
        _ui->itemEdit->setFocus();
        if (!_ui->itemEdit->hasAcceptableInput())  return;  // Item not valid
        QString  item = _ui->itemEdit->text();
        if (item.isEmpty())  return;  // Item may not be empty

        QString  itemPath = ArnM::addPath( _arnPath.path(), item);
        arnItemNew.open( itemPath);
        arnItem = &arnItemNew;
    }
    else
        arnItem = &_arnPath;
    if (!arnItem->isOpen())  return;

    ArnItem::Mode  mode;

    if (_ui->typeBidirButton->isChecked())
        arnItem->addMode( mode.BiDir);
    else if (_ui->typePipeButton->isChecked())
        arnItem->addMode( mode.Pipe);

    if (_ui->persistDbButton->isChecked()) {
        arnItem->addMode( mode.Save);
        emit _persistSapi.pv_rm( arnItem->path());
    }
    else if (_ui->persistFileButton->isChecked())
        emit _persistSapi.pv_touch( arnItem->path());
    else if (_ui->persistNoneButton->isChecked())
        emit _persistSapi.pv_rm( arnItem->path());

    emit _persistSapi.pv_dbMandatory( arnItem->path(), _ui->mandatoryButton->isChecked());

    emit _persistSapi.pv_ls( arnItem->path());
    emit _persistSapi.pv_dbMandatoryLs( arnItem->path());

    _storeValue = _arnPath.arnExport();
    QTimer::singleShot(300, this, SLOT(timeoutSave()));

    _isNewMode = false;
    doUpdate();
}


void  ManageWindow::on_cancelButton_clicked()
{
    _isNewMode = false;

    doUpdate();
}


void  ManageWindow::on_resetButton_clicked()
{
    if (!_arnPath.isFolder()) {
        _storeValue = _arnPath.arnExport();
        if (_arnPath.isSaveMode())
            emit _persistSapi.pv_dbMandatory( _arnPath.path(), false);
        _arnPath.destroyLink();
        QTimer::singleShot(300, this, SLOT(timeoutReset()));
    }
}


void  ManageWindow::on_folderButton_clicked()
{
}


void  ManageWindow::doCloseWindow()
{
    deleteLater();
}


void  ManageWindow::lsR( QStringList files)
{
    _isPersistFile = false;
    foreach (QString file, files) {
        if (file == _arnPath.path())
            _isPersistFile = true;
    }

    doPersistUpdate();
}


void  ManageWindow::mandatoryLsR( QStringList files)
{
    _isMandatory = false;
    foreach (QString file, files) {
        if (file == _arnPath.path())
            _isMandatory = true;
    }

    doPersistUpdate();
}
