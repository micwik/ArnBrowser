ArnBrowser Todo
===============

Major
-----

* Markdown editor.


Minor
-----

* Support for QStringList type (edit, view and as "set").



Done in 4.0
-----------

* Qt6 & ArnLib 4.0 adaptions
* Changed to use SyncMode::ExplicitMaster, then ArnServer is allways Master.
* Added support for SubEnum
* Changed to use c++11
* Added ArnLib config import with QMAKEFEATURES etc
* Added loadBinFile & saveBinFile
* Added font select in settings


Done in 3.1.5
-----------

* Added filter function.
* Added ctrl+g and ctrl+shift+g as key bindings for searching forward and backwards
* Term window: Added row counter. Settings: Added max rows option.
* Changed terminal font to monospace


Done in 3.1
-----------

* Context menu.
* Consistent display of "set" and "bitSet" values.
* Terminal history (redo) & search text window.
* Code edit search by key (Ctrl-F ...)
* Added double-click start terminal / editor


Done in 3.0
-----------

* Close and ReConnect to any Arn Server.
* Release button to restart view and release all synced ArnObjects.
  Can also be used at folder to release sub tree.
* New symbol to diff only known properties (leaves).
* Settings with generator for hash to login system.
* Settings with personal information to be used at server for identification (session).
* Chat dialog to server.
* Login dialog.
* Create and delete folders.
