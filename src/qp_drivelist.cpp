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

#include <qpixmap.h>
#include <qmessagebox.h>
#include <qstrlist.h>
#include <sys/param.h>  // MAXPATHLEN

#include "qp_drivelist.moc"
#include "qp_libparted.h"
#include "qp_common.h"
#include "qp_options.h"
#include "xpm/tool_disk.xpm"

QP_DriveList::QP_DriveList(QWidget *parent, const char *name, QP_Settings *settings)
    :QListView(parent, name) {

    setAllColumnsShowFocus(true);
    addColumn(tr("Device"));
    setSelectionMode(Single);

    /*---init the selected device---*/
    _selDevice = NULL;

    devlist = new QP_DevList(settings);
    
//    buildView();

    connect(this, SIGNAL(selectionChanged(QListViewItem *)),
            this, SLOT(slotListSelected(QListViewItem *)));
    connect(this, SIGNAL(contextMenuRequested(QListViewItem *, const QPoint &, int)),
            this, SLOT(slotPopUp(QListViewItem *, const QPoint &, int )));

}

QP_DriveList::~QP_DriveList() { 
}

void QP_DriveList::setPopup(QPopupMenu *popup) {
    _popup = popup;
}

void QP_DriveList::buildView() {
    /*---make the "root" of the listview---*/
    QListViewItem *ideRoot = new QListViewItem(this, tr("Disks"));

    /*---get a list of all available devices---*/
    devlist->getDevices();

    //QStrList lstdrives = QP_LibParted::device_probe();
    if (devlist->devlist.count() == 0) {
        QMessageBox::information(this, "QTParted",
                QString(tr("No device found. Maybe you're not using root user?")));
    }

    /*---make the group menu---*/
    _agDevices = new QActionGroup(this, 0);
    _agDevices->setExclusive(true);
    connect(_agDevices, SIGNAL(selected(QAction *)), this, SLOT(slotActionSelected(QAction *)));

    /*---add every device found---*/
    QP_Device *p;
    for (p = (QP_Device *)devlist->devlist.first();
            p; p = (QP_Device *)devlist->devlist.next()) {

        /*---get the device name---*/
        QString st = p->shortname();

        /*---add to the listview---*/
        QListViewItem *item = addDevice(st, ideRoot);

        /*---add to the group menu---*/
    	QAction *actDisk = new QAction(st, QIconSet(), QString::null/*accell*/, 0, _agDevices, 0, _agDevices->isExclusive());
        
        QP_DeviceNode *devicenode = new QP_DeviceNode();
        devicenode->action = actDisk;
        devicenode->listitem = item;

        p->setData((void *)devicenode);
    }

    if (ideRoot->childCount())
        ideRoot->setOpen( true );
    else
        delete ideRoot;
}

QListViewItem *QP_DriveList::addDevice(QString dev, QListViewItem *parent) {
    QListViewItem *item = new QListViewItem(parent, dev);
    item->setPixmap(0, QPixmap(tool_disk));

    return item;
}

QActionGroup *QP_DriveList::agDevices() {
    return _agDevices;
}

QP_Device *QP_DriveList::selDevice() {
    return _selDevice;
}

void QP_DriveList::slotListSelected(QListViewItem *item) {
    /*---if the item selected is not the root---*/
    if (!item->childCount()) {
        /*---get the device name (ex. /dev/hda)---*/
        QString name = item->text(0);

        /*---scan for every menu in the group---*/
        QP_Device *dev;
        for (dev = (QP_Device *)devlist->devlist.first(); dev; dev = (QP_Device *)devlist->devlist.next()) {
            /*---get the device node from devlist---*/
            QP_DeviceNode *p = (QP_DeviceNode *)dev->data();

            /*---the name match, so select it!---*/
            if (p->action->text().compare(name) == 0) {
                p->action->setOn(true);

                /*---and of course selecte the "selected device"---*/
                _selDevice = dev;
            }
        }
        emit sigSelectDevice(_selDevice);
    }
}

void QP_DriveList::slotActionSelected(QAction *action) {
    /*---get the device name (ex. /dev/hda)---*/
    QString name = action->text();

    /*---scan for every menu in the group---*/
    QP_Device *dev;
    for (dev = (QP_Device *)devlist->devlist.first(); dev; dev = (QP_Device *)devlist->devlist.next()) {
        /*---get the device node from devlist---*/
        QP_DeviceNode *p = (QP_DeviceNode *)dev->data();

        /*---the name match, so select it!---*/
        if (p->listitem->text(0).compare(name) == 0) {
            setSelected (p->listitem, true);

            /*---and of course selecte the "selected device"---*/
            _selDevice = dev;
        }
    }
}

void QP_DriveList::slotPopUp(QListViewItem *item, const QPoint & point, int) {
    if (item && _popup) _popup->popup(point);
}
