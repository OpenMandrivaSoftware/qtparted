/*
    qtparted - a frontend to libparted for manipulating disk partitions
    Copyright (C) 2002-2003 Vanni Brutto

    Vanni Brutto <zanac (-at-) libero dot it>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* About main.cpp
 *
 * This is the begin... and the end! :) :)
 *
 * Of course i'm joking! Here QTParted create the mainwindow and show it.
 */

#include <qapplication.h>
#include <qtranslator.h>
#include <qmessagebox.h>
#include <qtextcodec.h>
#include <qtimer.h>
#include "qp_libparted.h"
#include "qp_splash.h"
#include "qp_window.h"
#include "qp_settings.h"
#include "qp_common.h"
#include "qp_options.h"
#include "debug.h"

QP_MainWindow *mainwindow;

void checkDevfs() {
    int rc;

    rc = isDevfsEnabled();
    showDebug("isDevfsEnabled() == %d\n", rc);

    if (rc == -1) {
        QString label = QString(QObject::tr("Cannot read /proc/partitions, QTParted cannot be used!.\n"
                                            "Please use a kernel with /proc/partitions support."));
        QMessageBox::information(NULL, PROG_NAME, label);
        exit(1);
    } else {
        flagDevfsEnabled = bool(rc);
    }
}


int main(int argc, char *argv[])
{
// This allows to run QtParted with QtEmbedded without having
// to pass parameters "-qws".
#ifdef Q_WS_QWS // Frame Buffer 
   QApplication app(argc, argv, QApplication::GuiServer);
#else // X11
   QApplication app(argc, argv);
#endif // Q_WS_QWS
   //QApplication app(argc, argv, "qtparted");

    /*---install translation file for application strings---*/
    QTranslator *translator = new QTranslator(0);
    translator->load(QString(DATADIR "/locale/qtparted_") + QString(QTextCodec::locale()));
    app.installTranslator(translator);

    /*---initialize the debug system---*/
    g_debug.open();
    showDebug("QtParted debug logfile (http://qtparted.sf.net) version %s\n---------\n", VERSION);

    /*---check the Parted version---*/
    if (!QP_LibParted::checkForParted())
        return EXIT_FAILURE;

    /*---check if the kernel support devfs---*/
    checkDevfs();

     QP_Settings settings;

    mainwindow = new QP_MainWindow(&settings, 0, "QP_MainWindow");
    app.setMainWidget(mainwindow);

    QP_Splash *splash = new QP_Splash(mainwindow);
    splash->connect(mainwindow, SIGNAL(sigSplashInfo(const QString &)),
                    SLOT(addInfo(const QString &)));

    // kill the splash after 3 seconds
    QTimer::singleShot(3000, splash, SLOT(close()));
    splash->show();

    mainwindow->init();
    mainwindow->show();

    bool rc = app.exec();

    delete mainwindow;

    g_debug.close();
    return rc;
}
