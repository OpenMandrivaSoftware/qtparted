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

#include <qapplication.h>
#include <qsplitter.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qradiobutton.h>
#include <qstatusbar.h>
#include <qtoolbar.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qmessagebox.h>
#include "qp_common.h"
#include "qp_window.h"
#include "qp_filesystem.h"
#include "qp_dlgabout.h"
#include "qp_options.h"
#include "qp_fswrap.h"

#include "xpm/tool_disk.xpm"
#include "xpm/tool_property.xpm"
#include "xpm/tool_delete.xpm"
#include "xpm/tool_resize.xpm"
#include "xpm/tool_move.xpm"
#include "xpm/tool_quit.xpm"
#include "xpm/tool_undo.xpm"
#include "xpm/tool_save.xpm"
#include "xpm/tool_new.xpm"
#include "xpm/tool_format.xpm"
#include "xpm/tool_whatthis.xpm"
#include "../data/qtparted.xpm"



QP_MainWindow::QP_MainWindow(QP_Settings *qpsettings, QWidget *parent, const char *name)
                  :QMainWindow(parent, name) {
    setCaption(QString("%1 v%2").arg(PACKAGE).arg(VERSION));

    /*---load the setting from disk---*/
    settings = qpsettings;

    createAction();
    addToolBar();
    addMenuBar();
    addStatusBar();

    /*---init "enabled" status on menuitem---*/
    InitMenu();

    /*---create the dialog used for "create" new partition---*/
    dlgcreate = new QP_dlgCreate(this, "dlgCreate");

    /*---create the dialog used for "format" existing partition---*/
    dlgformat = new QP_dlgFormat(this, "dlgFormat");

    /*---create the dialog used for "resize" partition---*/
    dlgresize = new QP_dlgResize(this, "dlgResize");

    /*---create the dialog used for "progress"---*/
    dlgprogress = new QP_dlgProgress(this, "dlgProgress");

    /*---create the dialog used for "configuration"---*/
    dlgconfig = new QP_dlgConfig(this, "dlgConfig");

    /*---create the dialog used for "device property"---*/
    dlgdevprop = new QP_dlgDevProperty(this, "dlgDevProperty");

    /*---this is the central widget of the window (where i will attach the qsplitter---*/
    central = new QVBox(this);
    setCentralWidget(central);



    /*---the window is so designed:
     *
     * .------------------------------.
     * |QSplitter     |               |
     * |              |               |
     * | +----------+ | +-----------+ |
     * | |NavView   | | |DiskView   | |
     * | |          | | |           | |
     * | |          | | |           | |
     * | +----------+ | +-----------+ |
     * |              |               |
     * `------------------------------'
     */

    
    /*---make the vertical splitter---*/
    QSplitter *navSplit = new QSplitter(central);
    navSplit->setOpaqueResize(true); //I like opaque resize ;-)

    /*---add the navigator view (on the left) to the splitter---*/
    navview = new QP_NavView(navSplit, qpsettings);
    /*---i like that navview will not be always resized!---*/
    navSplit->setResizeMode(navview, QSplitter::KeepSize);
    /*---connect the selected signal (when user, for example, select /dev/hda)---*/
    connect(navview, SIGNAL(sigSelectDevice(QP_Device *)),
            this, SLOT(slotSelectDevice(QP_Device *)));

    /*---add the DiskView widget---*/
    diskview = new QP_DiskView(navSplit);
    /*---emitted when you select a partition---*/
    connect(diskview, SIGNAL(sigSelectPart(QP_PartInfo *)),
            this, SLOT(slotSelectPart(QP_PartInfo *)));
    /*---emit when the user want to popup the context menu---*/
    connect(diskview, SIGNAL(sigPopup(QPoint)),
            this, SLOT(slotPopup(QPoint)));    
    connect(diskview, SIGNAL(sigDevicePopup(QPoint)),
            this, SLOT(slotDevicePopup(QPoint)));    
    /*---connect the sigTimer used for dlgprogress during "update progressbar"---*/
    connect(diskview, SIGNAL(sigTimer(int, QString, QString)),
            dlgprogress, SLOT(slotTimer(int, QString, QString)));
    /*---connect the sigTimer used for dlgprogress during "commit operations"---*/
    connect(diskview, SIGNAL(sigOperations(QString, QString, int, int)),
            dlgprogress, SLOT(slotOperations(QString, QString, int, int)));
    /*---connect the sigDiskChanged used for undo/commit---*/
    connect(diskview, SIGNAL(sigDiskChanged()),
            this, SLOT(slotDiskChanged()));
}     


QP_MainWindow::~QP_MainWindow() {
}

void QP_MainWindow::init() {
    emit sigSplashInfo(tr("Getting devices"));
    navview->init();

    /*---now populate the disks menu---*/
    buildDisksMenu();

    /*---load user settings---*/
    emit sigSplashInfo(tr("load user settings"));
    loadSettings();

    emit sigSplashInfo(tr("Ready"));
}

void QP_MainWindow::refreshDiskView() {
    /*---show a progress dialog for long operation---*/
    InitProgressDialog();

    /*---refresh diskview widget!---*/
    diskview->refresh();

    /*---destroy the progress dialog---*/
//    DoneProgressDialog();
    
    /*---closethe progress dialog---*/
    dlgprogress->hide();
}

void QP_MainWindow::setpopupmenu(QPopupMenu *popupmenu) {
    _popupmenu = popupmenu;
}

QPopupMenu* QP_MainWindow::popupmenu() {
    return _popupmenu;
}

void QP_MainWindow::createAction() {
    /*---Undo button (used in File menu)---*/
    actUndo = new QAction(this, "Undo button");
    actUndo->setText(tr("&Undo"));
    actUndo->setIconSet(QPixmap(tool_undo));
    actUndo->setToolTip(tr("Undo"));
    actUndo->setWhatsThis(tr("Undo last operation"));
    actUndo->setEnabled(false);
    connect(actUndo, SIGNAL(activated()),
            this, SLOT(slotUndo()));

    /*---Commit button (used in File menu)---*/
    actCommit = new QAction(this, "Commit button");
    actCommit->setText(tr("&Commit"));
    actCommit->setIconSet(QPixmap(tool_save));
    actCommit->setToolTip(tr("Commit"));
    actCommit->setWhatsThis(tr("Commit all operations"));
    actCommit->setEnabled(false);
    connect(actCommit, SIGNAL(activated()),
            this, SLOT(slotCommit()));

    /*---Quit button (used in File menu)---*/
    actQuit = new QAction(this, "Quit button");
    actQuit->setText(tr("&Quit"));
    actQuit->setIconSet(QPixmap(tool_quit));
    actQuit->setToolTip(tr("Quit"));
    actQuit->setWhatsThis(tr("Quit from the application"));
    actQuit->setEnabled(true);
    connect(actQuit, SIGNAL(activated()),
            qApp, SLOT(quit()));

    /*---Property button (used in operations menu)---*/
    actProperty = new QAction(this, "property button");
    actProperty->setText(tr("&Property"));
    actProperty->setIconSet(QPixmap(tool_property));
    actProperty->setToolTip(tr("Property"));
    actProperty->setWhatsThis(tr("Property of the selected partition"));
    actProperty->setEnabled(false);
    connect(actProperty, SIGNAL(activated()),
            this, SLOT(slotProperty()));

    /*---Create button (used in operations menu)---*/
    actCreate = new QAction(this, "create button");
    actCreate->setText(tr("&Create"));
    actCreate->setIconSet(QPixmap(tool_new));
    actCreate->setToolTip(tr("Create"));
    actCreate->setWhatsThis(tr("Create a new partition"));
    actCreate->setEnabled(false);
    connect(actCreate, SIGNAL(activated()),
            this, SLOT(slotCreate()));


    /*---Format button (used in operations menu)---*/
    actFormat = new QAction(this, "format button");
    actFormat->setText(tr("&Format"));
    actFormat->setIconSet(QPixmap(tool_format));
    actFormat->setToolTip(tr("Format"));
    actFormat->setWhatsThis(tr("Format a new partition"));
    actFormat->setEnabled(false);
    connect(actFormat, SIGNAL(activated()),
            this, SLOT(slotFormat()));

    /*---Resize button (used in operations menu)---*/
    actResize = new QAction(this, "resize button");
    actResize->setText(tr("&Resize"));
    actResize->setIconSet(QPixmap(tool_resize));
    actResize->setToolTip(tr("Resize"));
    actResize->setWhatsThis(tr("Resize a partition"));
    actResize->setEnabled(false);
    connect(actResize, SIGNAL(activated()),
            this, SLOT(slotResize()));

    /*---Move button (used in operations menu)---*/
    actMove = new QAction(this, "move button");
    actMove->setText(tr("&Move"));
    actMove->setIconSet(QPixmap(tool_move));
    actMove->setToolTip(tr("Move"));
    actMove->setWhatsThis(tr("Move a partition"));
    actMove->setEnabled(false);
    connect(actMove, SIGNAL(activated()),
            this, SLOT(slotMove()));

    /*---Delete button (used in operations menu)---*/
    actDelete = new QAction(this, "Delete button");
    actDelete->setText(tr("&Delete"));
    actDelete->setIconSet(QPixmap(tool_delete));
    actDelete->setToolTip(tr("Delete"));
    actDelete->setWhatsThis(tr("Delete a new partition"));
    actDelete->setEnabled(false);
    connect(actDelete, SIGNAL(activated()),
            this, SLOT(slotDelete()));

    /*---Config button (used in options menu)---*/
    actConfig = new QAction(this, "Config button");
    actConfig->setText(tr("&Configuration..."));
    actConfig->setIconSet(QPixmap(tool_property));
    actConfig->setToolTip(tr("Configuration of QTParted"));
    actConfig->setWhatsThis(tr("Open the configuration dialog"));
    actConfig->setEnabled(true);
    connect(actConfig, SIGNAL(activated()),
            this, SLOT(slotConfig()));
   
    /*---What this button (used in toolbutton bar)---*/
    actWhatThis = new QAction(this, "Whatsthis button");
    actWhatThis->setText(tr("What's &This"));
    actWhatThis->setIconSet(QPixmap(tool_whatthis));
    actWhatThis->setToolTip(tr("What's This"));
    actWhatThis->setWhatsThis(tr("Enter <b>What's This</b> mode"
                              "It enables the user to ask for help "
                              "about widgets on the screen."));
    actWhatThis->setEnabled(true);
    actWhatThis->setAccel(SHIFT+Key_F1);
    connect(actWhatThis, SIGNAL(activated()),
            this, SLOT(slotWhatsThis()));

    /*---About button (used in operations help)---*/
    actAbout = new QAction(this, "About QTParted");
    actAbout->setText(tr("About &QTParted"));
    actAbout->setToolTip(tr("About QTParted"));
    actAbout->setWhatsThis(tr("Information about QTParted"));
    connect(actAbout, SIGNAL(activated()),
            this, SLOT(slotAbout()));

    /*---About QT button (used in operations help)---*/
    actAboutQT = new QAction(this, "About QT Button");
    actAboutQT->setText(tr("About Q&T"));
    actAboutQT->setToolTip(tr("About QT"));
    actAboutQT->setWhatsThis(tr("Information about QT"));
    connect(actAboutQT, SIGNAL(activated()),
            this, SLOT(slotAboutQT()));

    /*---disk navigator property button (used when you right click on a device---*/
    actNavProperty = new QAction(this, "NavProperty button");
    actNavProperty->setText(tr("&Device property"));
    actNavProperty->setIconSet(QPixmap(tool_disk));
    actNavProperty->setToolTip(tr("See the property of this disk device"));
    actNavProperty->setWhatsThis(tr("Some information about the disk device"));
    actNavProperty->setEnabled(true);
    connect(actNavProperty, SIGNAL(activated()),
            this, SLOT(slotNavProperty()));

    /*---disk navigator make partition table (used when you right click on a device---*/
    actNavPartTable = new QAction(this, "NavPartTable button");
    actNavPartTable->setText(tr("&Make a new Partition Table"));
    actNavPartTable->setIconSet(QPixmap(tool_format));
    actNavPartTable->setToolTip(tr("This will make the partition table"));
    actNavPartTable->setWhatsThis(tr("You're going to loose all partition data! Are you sure? :)"));
    actNavPartTable->setEnabled(true);
    connect(actNavPartTable, SIGNAL(activated()),
            this, SLOT(slotNavPartTable()));
}

void QP_MainWindow::addMenuBar() {
    /*---File menu---*/
    QPopupMenu *mnuFile = new QPopupMenu;
    menuBar()->insertItem(tr("&File"), mnuFile);
    actUndo->addTo(mnuFile);
    actCommit->addTo(mnuFile);
    //
    mnuFile->insertSeparator();
    //
    actQuit->addTo(mnuFile);
   

    /*---Action menu---*/
    mnuOperations = new QPopupMenu;
    mnuOperations->setCheckable(true);
    menuBar()->insertItem(tr("&Operations"), mnuOperations);
    actProperty->addTo(mnuOperations);
    actCreate->addTo(mnuOperations);
    actFormat->addTo(mnuOperations);
    actResize->addTo(mnuOperations);
    actMove->addTo(mnuOperations);
    actDelete->addTo(mnuOperations);
    //
    mnuOperations->insertSeparator();
    //
    mnuSetActiveID = mnuOperations->insertItem(tr("Set Active..."));
    mnuOperations->connectItem(mnuSetActiveID, this, SLOT(slotSetActive()));
    mnuOperations->setItemEnabled(mnuSetActiveID, false);

    mnuSetHiddenID = mnuOperations->insertItem(tr("Set Hidden..."));
    mnuOperations->connectItem(mnuSetHiddenID, this, SLOT(slotSetHidden()));
    mnuOperations->setItemEnabled(mnuSetHiddenID, false);


    /*---set popupmenu for the action menu!---*/
    setpopupmenu(mnuOperations);


    /*---Disk menu---*/
    mnuDisks = new QPopupMenu;
    menuBar()->insertItem(tr("&Disks"), mnuDisks);


    /*---Device menu---*/
    mnuDevice = new QPopupMenu;
    mnuDevice->setEnabled(false);
    menuBar()->insertItem(tr("&Device"), mnuDevice);
    actUndo->addTo(mnuDevice);
    actCommit->addTo(mnuDevice);


    /*---Options menu---*/
    QPopupMenu *mnuOptions = new QPopupMenu;
    menuBar()->insertItem(tr("&Options"), mnuOptions);
    actConfig->addTo(mnuOptions);

   
    /*---Help menu---*/
    QPopupMenu *mnuHelp = new QPopupMenu;
    menuBar()->insertItem(tr("&Help"), mnuHelp);
    actWhatThis->addTo(mnuHelp);
    mnuHelp->insertSeparator();
    actAbout->addTo(mnuHelp);
    actAboutQT->addTo(mnuHelp);
    

    /*---disk navigator popup menu---*/
    _navpopupmenu = new QPopupMenu;
    actNavProperty->addTo(_navpopupmenu);
    actNavPartTable->addTo(_navpopupmenu);
}

void QP_MainWindow::addStatusBar() {
    /*---This functions is dummy! i have not yet decided if a statusbar is useful!---*/

    /*---make an hbox and add it in the statusbar---*/
    QHBox *hbox = new QHBox(statusBar());
    hbox->setMargin(5);
    hbox->setSpacing(6);

    /*---make a label, size it, set the text... and attach it to the hbox---*/
    QLabel *lbl1 = new QLabel(hbox, "message");
    lbl1->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    lbl1->setAlignment(AlignLeft);
    QFont boldfont; boldfont.setWeight(QFont::Bold);
    lbl1->setFont(boldfont);
    lbl1->setText("Message");
    lbl1->setMinimumHeight(lbl1->sizeHint().height());
    lbl1->setText("QTParted :)");
    QToolTip::add(lbl1, "Message area");
    
    /*---add a separator in the statusbar---*/
    QFrame *frame = new QFrame(hbox, "frame");
    frame->setFrameShadow(QFrame::Sunken);
    frame->setFrameShape(QFrame::VLine);

    /*---make a label, size it, set the text... and attach it to the hbox---*/
    QLabel *lbl2 = new QLabel(hbox, "message");
    lbl2->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    lbl2->setAlignment(AlignLeft);
    boldfont.setWeight(QFont::Bold);
    lbl2->setFont(boldfont);
    lbl2->setText("Message");
    lbl2->setMinimumHeight(lbl2->sizeHint().height());
    lbl2->setText("by Zanac (c) 2002-2003");
    QToolTip::add(lbl2, "Message area");
    
    frame = new QFrame(hbox, "frame");
    frame->setFrameShadow(QFrame::Sunken);
    frame->setFrameShape(QFrame::VLine);

    QLabel *lblmsg = new QLabel(hbox, "message");
    lblmsg->setAlignment(AlignLeft);
    boldfont.setWeight(QFont::Bold);
    lblmsg->setFont(boldfont);
    lblmsg->setText("Message");
    lblmsg->setMinimumHeight(lblmsg->sizeHint().height());
    lblmsg->setText(QString::null);
    QToolTip::add(lblmsg, "Message area");

    /*---add the hbox to the statusbar---*/
    statusBar()->addWidget(hbox, 1);

    /*---add the "grip" on the right of the statusbar---*/
    statusBar()->setSizeGripEnabled(true);
}

void QP_MainWindow::addToolBar() {
    /*---Operations toolbar---*/
    QToolBar *toolUndoCommit = new QToolBar(this, "toolUndoCommit");
    actUndo->addTo(toolUndoCommit);
    actCommit->addTo(toolUndoCommit);

    /*---Operations toolbar---*/
    QToolBar *toolOperations = new QToolBar(this, "toolOperations");
    actProperty->addTo(toolOperations);
    actCreate->addTo(toolOperations);
    actFormat->addTo(toolOperations);
    actResize->addTo(toolOperations);
    actMove->addTo(toolOperations);
    actDelete->addTo(toolOperations);

    /*---What's this toolbar---*/
    QToolBar *toolWhatThis = new QToolBar(this, "toolWhatThis");
    actWhatThis->addTo(toolWhatThis);
}

void QP_MainWindow::buildDisksMenu() {
    navview->agDevices()->addTo(mnuDisks);
    navview->setPopup(_navpopupmenu);
}

void QP_MainWindow::loadSettings() {
    /*---set the layout---*/
    diskview->setLayout(settings->layout());
}

void QP_MainWindow::slotCreate() {
    /*---init of the dialog box used for create new partition---*/
    dlgcreate->init_dialog();

    /*---get the maxsize of the partition selected. This is the max PedSector size that u can create---*/
    PedSector maxsize = diskview->selPartInfo()->end - diskview->selPartInfo()->start;
    dlgcreate->setMaxSize(maxsize);

    /*---set the filesystem list---*/
    dlgcreate->setFileSystem(diskview->filesystem);

    /*---loop into filesystemlist, and add it to combobox if it has create flag---*/
    QP_FileSystemSpec *p;
    for (p = (QP_FileSystemSpec*)diskview->filesystem->filesystemlist.first(); 
            p; p = (QP_FileSystemSpec*)diskview->filesystem->filesystemlist.next())

        /*---test if u you can create partition with filesystem             ---
         *---check also that the size is enough for this kind of filesystem,---
         *---for example fat32 has minimal size of 512Mbyte                 ---*/
        if ((p->create()
        &&  (p->minFsSize() <= maxsize))) {
            if (!(navview->selDevice()->isBusy() && p->fswrap()))
                dlgcreate->addFileSystem(p->name());
        }

    /*---the "addType" method of dlgcreate add new type to the combobox---*/
    
    /*---the user want to create a primary/extended partition---*/
    if (diskview->selPartInfo()->type == QTParted::primary) {
        dlgcreate->addTypePrimary();

        /*---'cause there isn't any extended partition add it in the combo box---*/
        if (!diskview->libparted->has_extended) 
            dlgcreate->addTypeExtended();
    } else
        /*---else... add a logical!---*/
        dlgcreate->addTypeLogical();

    /*---show dialog, and get return code (useful for getting "accepted"/"cancel" of dialog---*/
    int code = dlgcreate->show_dialog();
    if (code == QDialog::Accepted) {
        PedSector start;
        PedSector end;

        /*---the user want to make the partition at the beginning or at the end of free space?---*/
        if (dlgcreate->radioBegin->isChecked()) {
            start = diskview->selPartInfo()->start,
            end = start + dlgcreate->Size();
        } else {
            end = diskview->selPartInfo()->end;
            start = end - dlgcreate->Size();
        }

        QP_FileSystemSpec *fsspec = diskview->filesystem->nameToFSSpec(dlgcreate->fileSystemName());
        diskview->libparted->mkpartfs(dlgcreate->type(),
                                      fsspec,
                                      start,
                                      end,
                                      dlgcreate->Label());

        /*---refresh diskview widget!---*/
        refreshDiskView();
    }
}

void QP_MainWindow::slotFormat() {
    /*---there are not selected partitions!---*/
    if (!diskview->selPartInfo()) return;

    if (diskview->selPartInfo()->isVirtual()) {
        QString label = QString(tr("This is a virtual partition. You cannot alter it: use undo instead."));
        QMessageBox::information(this, PROG_NAME, label);

        return ;
    }

    /*---init of the dialog box used for formatting an existing partition---*/
    dlgformat->init_dialog();

    /*---get how big is the partition---*/
    PedSector maxsize = diskview->selPartInfo()->end - diskview->selPartInfo()->start;

    /*---loop into filesystemlist, and add it to combobox if it has create flag---*/
    QP_FileSystemSpec *p;
    for (p = (QP_FileSystemSpec*)diskview->filesystem->filesystemlist.first(); 
            p; p = (QP_FileSystemSpec*)diskview->filesystem->filesystemlist.next())
        /*---test if u you can create partition with filesystem             ---
         *---check also that the size is enough for this kind of filesystem,---
         *---for example fat32 has minimal size of 512Mbyte                 ---*/
        if ((p->create()
        &&  (p->minFsSize() <= maxsize))
        &&  (  (p->maxFsSize() >= maxsize)
            || (p->maxFsSize() == 0))) {
            dlgformat->addFileSystem(p->name());
        }

    /*---show dialog, and get return code (useful for getting "accepted"/"cancel" of dialog---*/
    int code = dlgformat->show_dialog();
    if (code == QDialog::Accepted) {
        QP_FileSystemSpec *fsspec = diskview->filesystem->nameToFSSpec(dlgformat->fileSystemName());
        diskview->libparted->mkfs(diskview->selPartInfo(), fsspec, dlgformat->Label());

        /*---refresh diskview widget!---*/
        refreshDiskView();
    }
}

void QP_MainWindow::slotResize() {
    /*---there are not selected partitions!---*/
    if (!diskview->selPartInfo()) return;

    ShowMoveResizeDialog(QTParted::resize);
}

void QP_MainWindow::slotMove() {
    /*---there are not selected partitions!---*/
    if (!diskview->selPartInfo()) return;

    ShowMoveResizeDialog(QTParted::move);
}

void QP_MainWindow::ShowMoveResizeDialog(QTParted::actType moveresize) {
    //FIXME: extended partition can be resized also on the "left"

    QP_PartInfo *partinfo = diskview->selPartInfo();

    if (partinfo->isVirtual()) {
        QString label = QString(tr("This is a virtual partition. You cannot alter it: use undo instead."));
        QMessageBox::information(this, PROG_NAME, label);

        return ;
    }

    if ((partinfo->t_start == -1)
     || (partinfo->t_end   == -1)) {
        QString label = QString(tr("An internal error happened during partition scan.\n"
                                   "This means that you cannot resize or move this "
                                   "partition."));
        QMessageBox::information(this, PROG_NAME, label);

        return ;
    }

    /*---get info about the selected partition---*/
    PedSector StartPart = partinfo->start;
    PedSector EndPart = partinfo->end;
    PedSector GrowStartPart = partinfo->t_start;
    PedSector GrowEndPart = partinfo->t_end;
    PedSector MinPart = partinfo->min_size;
    if (partinfo->fsspec->minFsSize() >= MinPart)   //if the filesystem minsize is >... use it!
        MinPart = partinfo->fsspec->minFsSize();
    if (partinfo->fsspec->fswrap()
    &&  partinfo->fsspec->fswrap()->wrap_resize == RS_ENLARGE) //the filesystem can only be enlarged.
        MinPart = EndPart - StartPart;

    PedSector MaxPart = partinfo->fsspec->maxFsSize();

    /*---set the info that were just get---*/
    dlgresize->setGrowStartPartSector(GrowStartPart);
    dlgresize->setGrowEndPartSector(GrowEndPart);
    dlgresize->setEndPartSector(EndPart);
    dlgresize->setStartPartSector(StartPart);
    dlgresize->setMinPartSector(MinPart);
    dlgresize->setMaxPartSector(MaxPart);

    /*---init of the dialog box used for resize/move a partition---*/
    dlgresize->init_dialog(moveresize);

    /*---show dialog, and get return code (useful for getting "accepted"/"cancel" of dialog---*/
    int code = dlgresize->show_dialog();

    /*---the user clicked ok?---*/
    if (code == QDialog::Accepted) {
        QString label;

        /*---call libparted resize/move function!---*/
        bool rc;
        if (moveresize == QTParted::resize)
            rc = diskview->selPartInfo()->resize(dlgresize->_StartPartSector,
                                                 dlgresize->_EndPartSector);
        else
            rc = diskview->selPartInfo()->move(dlgresize->_StartPartSector,
                                               dlgresize->_EndPartSector);

        if (!rc) {
            QString label = QString(diskview->libparted->message());
            QMessageBox::information(this, PROG_NAME, label);
        }

        /*---refresh diskview widget!---*/
        refreshDiskView();
    }
}

void QP_MainWindow::InitMenu() {
    actProperty->setEnabled(false);
    actCreate->setEnabled(false);
    actFormat->setEnabled(false);
    actResize->setEnabled(false);
    actMove->setEnabled(false);
    actDelete->setEnabled(false);
    mnuOperations->setItemEnabled(mnuSetActiveID, false);
    mnuOperations->setItemEnabled(mnuSetHiddenID, false);
}

void QP_MainWindow::InitProgressDialog() {
    /*---initialize the progress dialog and show it---*/
    dlgprogress->init_dialog();
    dlgprogress->show();

    /*---just update GUI---*/
    qApp->processEvents();
}

void QP_MainWindow::DoneProgressDialog() {
    /*---update GUI and disconnect sigTimer---*/
    qApp->processEvents();

    dlgprogress->exec();
}

void QP_MainWindow::slotDelete() {
    /*---there are not selected partitions!---*/
    if (!diskview->selPartInfo()) return;

    if (diskview->selPartInfo()->isVirtual()) {
        QString label = QString(tr("This is a virtual partition. You cannot alter it: use undo instead."));
        QMessageBox::information(this, PROG_NAME, label);

        return ;
    }
    
    /*---call the libparted wrapper to remove the selected partition---*/
    if (!diskview->libparted->rm(diskview->selPartInfo()->num)) {
        QMessageBox::information(this, PROG_NAME, diskview->libparted->message());
        return ;
    }

    /*---refresh diskview widget!---*/
    refreshDiskView();
}

void QP_MainWindow::slotConfig() {
    /*---init of the configuration dialog---*/
    dlgconfig->init_dialog();

    /*---get the layout---*/
    int layout = settings->layout();

    /*---set the current layout---*/
    dlgconfig->setLayout(layout);

    int code = dlgconfig->show_dialog();
    if (code == QDialog::Accepted) {
        int layout = dlgconfig->layout();
        settings->setLayout(layout);
        diskview->setLayout(layout);

        lstExternalTools->apply();
    } else {
        lstExternalTools->cancel();
    }
}

void QP_MainWindow::slotProperty() {
    /*---there are not selected partitions!---*/
    if (!diskview->selPartInfo()) return;

    /*---make a label with information about the partition selected---*/
    QString label;
    
    QString type;
         if (diskview->selPartInfo()->type == QTParted::primary)
         type.sprintf(tr("primary"));
    else if (diskview->selPartInfo()->type == QTParted::logical)
         type.sprintf(tr("logical"));
    else type.sprintf(tr("extended"));
    
    label = QString(tr("this is a %1 partition\n"
                    "it start at: %2 and "
                    "end at: %3\n"
                    "it can grow left: %4 and "
                    "right: %5\n"
                    "the filesystem is: %6"
                    "label: %7\n"))
                            .arg(type)
                            .arg(diskview->selPartInfo()->mb_start())
                            .arg(diskview->selPartInfo()->mb_end())
                            .arg(diskview->selPartInfo()->mb_t_start())
                            .arg(diskview->selPartInfo()->mb_t_end())
                            .arg(diskview->selPartInfo()->fsspec->name())
                            .arg(diskview->selPartInfo()->label());

    QMessageBox::information(this, PROG_NAME, label);
}

void QP_MainWindow::slotWhatsThis() {
    whatsThis();
}

void QP_MainWindow::slotAbout() {
    QString content = QString("<h3>%1 v%2</h3>\n").arg(PROG_NAME).arg(VERSION);
    content += "<br>\n";
    content += "Copyright (C) 2003<br>\n";
    content += "By Vanni Brutto<br>\n";
    content += "zanac4ever@virgilio.it<br>\n";
    content += HOMEPAGE;
    content += "<br>\n";
    content += "<br>\n";
    content += "QTParted is free software, covered by the GNU General Public License, and you are welcome to change it and/or distribute copies of it under certain conditions.<br>\n";
    content += "<br>\n";
    content += "A Partition Magic Clone written in C++ using the Qt GUI Toolkit.<br>\n";
    content += "<br>\n";
    content += "See the AUTHORS file for more info about contributors.<br>\n";

    QP_dlgAbout *w = new QP_dlgAbout(qtparted_xpm, content, this);
    w->setCaption(tr("About"));
    w->exec();
    delete w;
}

void QP_MainWindow::slotAboutQT() {
    QMessageBox::aboutQt(this);
}

void QP_MainWindow::slotNavProperty() {
    /*---init the dialog and show it!---*/
    dlgdevprop->init_dialog();
    
    QP_Device *seldev = navview->selDevice();

    dlgdevprop->setShortName(seldev->shortname());
    dlgdevprop->setLongName(seldev->longname());
    dlgdevprop->setDevfsEnabled(seldev->devfsEnabled());

    if (!seldev->canUpdateGeometry()) 
        dlgdevprop->setState(tr("Device status: readonly."));
    else if (seldev->isBusy()) 
        dlgdevprop->setState(tr("Device status: busy."));
    else
        dlgdevprop->setState(tr("Device status: available."));
    
    
    if (seldev->partitionTable()) {
        dlgdevprop->setPartitionTable(tr("detected"));
    } else {
        dlgdevprop->setPartitionTable(tr("none"));
    }
    

    dlgdevprop->show_dialog();
}

void QP_MainWindow::slotNavPartTable() {
    QString label = QString(tr("You're going to make a new partition table. Are you sure?\nAll data will be lost!"));
    QMessageBox mb(PROG_NAME,
                  label,
                  QMessageBox::Information,
                  QMessageBox::Yes,
                  QMessageBox::No | QMessageBox::Default | QMessageBox::Escape,
                  QMessageBox::NoButton);

    /*---yes, the user is sure :) ---*/
    if (mb.exec() == QMessageBox::Yes) {
        bool rc = navview->selDevice()->newPartTable();
        
        if (rc) {
            slotSelectDevice(navview->selDevice());
        } else {
            QString label = QString(tr("It was not possible to make a new partition table."));
            QMessageBox::information(this, PROG_NAME, label);
        }

        
    }
}

void QP_MainWindow::slotSelectPart(QP_PartInfo *partinfo) {
    actProperty->setEnabled(true);

    QP_Device *selDevice = navview->selDevice();

    /*---if the device has not partition table                           ---
     *---or if the device is in "readonly"...                            ---
     *---or the user made some not commited change and the device is busy---*/
    if (!selDevice->partitionTable()
    ||  !selDevice->canUpdateGeometry()
    ||  (diskview->canUndo() && selDevice->isBusy())) {
        mnuOperations->setItemEnabled(mnuSetActiveID, false);
        mnuOperations->setItemChecked(mnuSetActiveID, partinfo->isActive());
        mnuOperations->setItemEnabled(mnuSetHiddenID, false);
        mnuOperations->setItemChecked(mnuSetHiddenID, partinfo->isHidden());

        actCreate->setEnabled(false);
        actFormat->setEnabled(false);
        actResize->setEnabled(false);
        actMove->setEnabled(false);
        actDelete->setEnabled(false);

        return;
    }
    
    /*---check/uncheck the active flag on the menu popup---*/
    mnuOperations->setItemEnabled(mnuSetActiveID, partinfo->canBeActive());
    mnuOperations->setItemChecked(mnuSetActiveID, partinfo->isActive());
    mnuOperations->setItemEnabled(mnuSetHiddenID, partinfo->canBeHidden());
    mnuOperations->setItemChecked(mnuSetHiddenID, partinfo->isHidden());

    if (diskview->selPartInfo()->fsspec == diskview->filesystem->free()) {
        /*---you cannot create more then 4 primary partions, but more then 4 logical!    ---*/
        /*---please note that the test is > 4 'cause there is the free virtual partitions---*/
        if ((diskview->libparted->partlist.count() > 4)
        && (diskview->selPartInfo()->type != QTParted::logical)) {
            actCreate->setEnabled(false);
        } else {
            actCreate->setEnabled(true);
        }
        actFormat->setEnabled(false);
        actResize->setEnabled(false);
        actMove->setEnabled(false);
        actDelete->setEnabled(false);
    } else {
        bool resize = partinfo->fsspec->resize();
        bool move = partinfo->fsspec->move();
        actCreate->setEnabled(false);
        actDelete->setEnabled(true);
        actMove->setEnabled(move);

        if (partinfo->type == QTParted::extended) {
            actResize->setEnabled(true);
            actFormat->setEnabled(false);
        } else {
            actResize->setEnabled(resize);
            actFormat->setEnabled(true);
        }
    }
}

void QP_MainWindow::slotDevicePopup(QPoint pos) {
    _navpopupmenu->popup(pos);
}

void QP_MainWindow::slotPopup(QPoint pos) {
    //TODO: check, can i remove _navpopupmenu->popup?
    if (!navview->selDevice()->partitionTable())
        _navpopupmenu->popup(pos);
    else
        popupmenu()->popup(pos);
}

void QP_MainWindow::slotSelectDevice(QP_Device *dev) {
    /*---initialization of menuitem---*/
    InitMenu();

    /*---enable menu device---*/
    mnuDevice->setEnabled(true);

    if (dev->partitionTable())
        /*---show a progress dialog for long operation---*/
        InitProgressDialog();


    /*---change the device---*/
    diskview->setDevice(dev);

    
    if (dev->partitionTable())
        /*---closethe progress dialog---*/
        dlgprogress->hide();
}

void QP_MainWindow::slotSetActive() {
    QString oldActive;
    QString newActive;
    QString noactive;
    QString label;
    
    noactive = QString(tr("(No Active on Drive)"));

    if (!diskview->libparted->partActive())
        oldActive = noactive;
    else
        oldActive = diskview->libparted->partActive()->partname();

    bool active = mnuOperations->isItemChecked(mnuSetActiveID);
    if (active)
        newActive = noactive;
    else
        newActive = diskview->selPartInfo()->partname();
    
    label = QString(tr("Currently active partition: %1\n"
                       "Change active partition to: %2\n\n"
                       "Are you sure you want to change the active partition?"))
                                   .arg(oldActive)
                                   .arg(newActive);

    QMessageBox mb(PROG_NAME,
                  label,
                  QMessageBox::Information,
                  QMessageBox::Yes,
                  QMessageBox::No | QMessageBox::Default | QMessageBox::Escape,
                  QMessageBox::NoButton);

    /*---yes, the user is sure :) ---*/
    if (mb.exec() == QMessageBox::Yes) {
        diskview->selPartInfo()->setActive(!active);

        /*---refresh diskview widget!---*/
        refreshDiskView();
    }
}

void QP_MainWindow::slotSetHidden() {
    /*------*/
    diskview->selPartInfo()->setHidden(!diskview->selPartInfo()->isHidden());

    /*---refresh diskview widget!---*/
    refreshDiskView();
}

void QP_MainWindow::slotUndo() {
    diskview->undo();
}

void QP_MainWindow::slotCommit() {
    QString label = QString(tr("You're commiting all changes. Warning, you can lost data!\n"
                               "Make sure also that you're not commiting a busy device...\n"
                               "In other word PLEASE UMOUNT ALL PARTITIONS before commiting changes!"));
    
    QMessageBox mb(PROG_NAME,
                  label,
                  QMessageBox::Information,
                  QMessageBox::Yes,
                  QMessageBox::No | QMessageBox::Default | QMessageBox::Escape,
                  QMessageBox::NoButton);

    /*---yes, the user is sure :) ---*/
    if (mb.exec() == QMessageBox::No) {
        return ;
    }
    
    /*---show a progress dialog for long operation---*/
    InitProgressDialog();
    
    diskview->commit();

    /*---destroy the progress dialog---*/
    DoneProgressDialog();
}

void QP_MainWindow::slotDiskChanged() {
    if (diskview->canUndo()) {
        actUndo->setEnabled(true);
        actCommit->setEnabled(true);
    } else {
        actUndo->setEnabled(false);
        actCommit->setEnabled(false);
    }
}
