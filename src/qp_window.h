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

/* About QP_Window class:
 *
 * This class is used to create the mainwindow...
 */


#ifndef QP_MAINWINDOW_H
#define QP_MAINWINDOW_H

#include <qmainwindow.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qtoolbutton.h>
#include <qaction.h>
#include <qpopupmenu.h>

#include "qtparted.h"
#include "qp_libparted.h"
#include "qp_dlgcreate.h"
#include "qp_dlgformat.h"
#include "qp_dlgresize.h"
#include "qp_dlgprogress.h"
#include "qp_dlgconfig.h"
#include "qp_dlgdevprop.h"
#include "qp_diskview.h"
#include "qp_navview.h"
#include "qp_settings.h"


class QP_MainWindow : public QMainWindow {
Q_OBJECT

public:
    QP_MainWindow(QP_Settings *, QWidget *parent, const char *name);
    ~QP_MainWindow();
    void init();                       /*---init the mainwindow        ---*/
    void refreshDiskView();
    void setpopupmenu(QPopupMenu *);
    QPopupMenu *popupmenu();
    QPopupMenu *mnuDisks;
    QPopupMenu *mnuDevice;
    QAction *actUndo;
    QAction *actCommit;
    QAction *actQuit;
    QAction *actProperty;
    QAction *actCreate;
    QAction *actFormat;
    QAction *actResize;
    QAction *actMove;
    QAction *actDelete;
    QAction *actConfig;
    QAction *actWhatThis;
    QAction *actAbout;
    QAction *actAboutQT;
    QAction *actNavProperty;
    QAction *actNavPartTable;
    QP_DiskView *diskview;
    QP_dlgCreate *dlgcreate;            /*---the create dialog         ---*/
    QP_dlgFormat *dlgformat;            /*---the format dialog         ---*/
    QP_dlgResize *dlgresize;            /*---the resize dialog         ---*/
    QP_dlgProgress *dlgprogress;        /*---the progress dialog       ---*/
    QP_dlgConfig *dlgconfig;            /*---the configuration dialog  ---*/
    QP_dlgDevProperty *dlgdevprop;      /*---the device property dialog---*/
    QP_NavView *navview;                /*---the disk navigation widget---*/
    QP_Settings *settings;


protected:
    void createAction();
    void addMenuBar();
    void addStatusBar();
    void addToolBar();
    void buildDisksMenu();
    void loadSettings();
    void ShowMoveResizeDialog(QTParted::actType moveresize);
    void InitMenu();
    void InitProgressDialog();
    void DoneProgressDialog();


private:
    QVBox *central;
    QPopupMenu* _popupmenu;
    QPopupMenu* _navpopupmenu;
    QPopupMenu *mnuOperations;
    int mnuSetActiveID;

protected slots:
    void slotCreate();
    void slotFormat();
    void slotResize();
    void slotMove();
    void slotDelete();
    void slotConfig();
    void slotProperty();
    void slotWhatsThis();
    void slotAbout();
    void slotAboutQT();
    void slotNavProperty();
    void slotNavPartTable();
    void slotSelectPart(QP_PartInfo *);
    void slotDevicePopup(QPoint pos);
    void slotPopup(QPoint pos);
    void slotSelectDevice(QP_Device *);
    void slotSetActive();
    void slotUndo();
    void slotCommit();
    void slotDiskChanged();

signals:
    void sigSplashInfo(const QString &);
};

#endif