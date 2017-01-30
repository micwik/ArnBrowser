// Copyright (C) 2010-2016 Michael Wiklund.
// All rights reserved.
// Contact: arnlib@wiklunden.se
//
// This file is part of the ArnLib - Active Registry Network.
// Parts of ArnLib depend on Qt and/or other libraries that have their own
// licenses. Usage of these other libraries is subject to their respective
// license agreements.
//
// GNU General Public License Usage
// This file may be used under the terms of the GNU General Public
// License version 3.0 as published by the Free Software Foundation and appearing
// in the file LICENSE_GPL.txt included in the packaging of this file.
//
// Other Usage
// Alternatively, this file may be used in accordance with the terms and conditions
// contained in a signed written agreement between you and Michael Wiklund.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//

#include <QApplication>
#include "MainWindow.hpp"
#include <QIcon>

#if QT_VERSION >= 0x050000
    const QString QtVerTxt = "(Qt5)";
#else
    const QString QtVerTxt = "(Qt4)";
#endif

    extern const QString ver = "3.1  " + QtVerTxt;  // Don't forget to change in setup.iss -files


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIcon appIcon("://pic/arn_browser_128.png");
    a.setWindowIcon( appIcon);

    MainWindow w;
    w.show();
    return a.exec();
}
