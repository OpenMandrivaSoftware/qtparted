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

#include "qp_listview.moc"
#include "qp_filesystem.h"
#include "qp_options.h"

#include <config.h>

#define MIN_HDWIDTH 190


/*----------QP_ListViewItem----------------------------------------------------------*/
/*---                                                                             ---*/
QP_ListViewItem::QP_ListViewItem(QListView *parent,
        const QString &number,
        const QString &diskName,
        const QString &fstype,
        const QString &status,
        const QString &sizeStr,
        const QString &usedStr,
        const QString &startStr,
        const QString &endStr,
        const QString &label,
        QP_PartInfo *pinfo)
    :QListViewItem(parent, number, diskName, fstype, status, sizeStr, usedStr, startStr, endStr) {

    partinfo = pinfo;
    partinfo->listviewitem = this;

    setPixmap(0, pinfo->fsspec->pixmap());
    setText(8, label);
}

QP_ListViewItem::QP_ListViewItem(QListViewItem *parent, 
        const QString &number,
        const QString &diskName, 
        const QString &fstype,
        const QString &status,
        const QString &sizeStr,
        const QString &usedStr,
        const QString &startStr,
        const QString &endStr,
        const QString &label,
        QP_PartInfo *pinfo)
    :QListViewItem(parent, number, diskName, fstype, status, sizeStr, usedStr, startStr, endStr) {

    partinfo = pinfo;
    partinfo->listviewitem = this;

    setPixmap(0, pinfo->fsspec->pixmap());
    setText(8, label);
}
/*-----------------------------------------------------------------------------------*/





/*----------QP_RealListView----------------------------------------------------------*/
/*---                                                                             ---*/
QP_RealListView::QP_RealListView(QWidget *parent, const char *name)
    :QListView(parent, name) {

    itemextended = NULL;

    setRootIsDecorated(true);
    setAllColumnsShowFocus(true);

    addColumn(tr("Number"));
    addColumn(tr("Partition"));
    setColumnAlignment(1, AlignCenter);
    addColumn(tr("Type"));
    addColumn(tr("Status"));
    addColumn(tr("Size"));
    setColumnAlignment(4, AlignRight);
    addColumn(tr("Used space"));
    setColumnAlignment(5, AlignRight);
    addColumn(tr("Start"));
    setColumnAlignment(6, AlignRight);
    addColumn(tr("End"));
    setColumnAlignment(7, AlignRight);
#ifdef LABELS_SUPPORT
    addColumn(tr("Label")); // number 8
#endif // LABELS_SUPPORT

    // a little more space between columns
    setItemMargin(3);

    /*---get if user want to change selectior or want to popup a menu---*/
    connect(this, SIGNAL(selectionChanged(QListViewItem *)),
                  SLOT(selectionChanged(QListViewItem *)));
    connect(this, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)),
                  SLOT(rightButtonClicked(QListViewItem *, const QPoint &, int)));
}

QP_RealListView::~QP_RealListView() {
}

void QP_RealListView::setDevice(QP_Device *device) {
    _device = device;
}

void QP_RealListView::addPrimary(QP_PartInfo *partinfo, int number) {
    /*---get the filesystem---*/
    QString fstype = partinfo->fsspec->name();

    /*---force "extended" label if the type is extended---*/
    if (partinfo->type == QTParted::extended) fstype = "extended";

    /*---set label for start, end and size---*/
    QString startStr = MB2String(partinfo->mb_start());
    QString endStr = MB2String(partinfo->mb_end());
    QString sizeStr = MB2String(partinfo->mb_end()-partinfo->mb_start());
    QString snumber; snumber.sprintf("%.2d", number);
    QString name = partinfo->partname();
    QString label = partinfo->label();
    
    QString status;
    if (partinfo->isActive()) status = QString(tr("Active"));
                      else status = QString::null;

    if (partinfo->isHidden()) {
        if (!status.isNull()) status += "/";
        status += QString(tr("Hidden"));
    }
    
    QString usedStr;
    if (partinfo->min_size == -1)
        usedStr = QString(tr("N/A"));
    else
        usedStr = MB2String(partinfo->mb_min_size());

    /*---if doesn't exit a partition table make "fake" listitem---*/
    if (!_device->partitionTable()) {
        fstype = QString::null;
        startStr = QString::null;
        endStr = QString::null;
        sizeStr = QString::null;
        usedStr = QString::null;
        snumber = QString("01");
        status = QString(tr("Empty"));
        name = QString(tr("Partition table"));

    }

    /*---make a new line (every line is a primary or an extended partition)---*/
    QP_ListViewItem *item = new QP_ListViewItem(this, 
                                                snumber,
                                                name,
                                                fstype,
                                                status,
                                                sizeStr, usedStr, startStr, endStr,
                                                label,
                                                partinfo);

    /*---if you add an extended... just save the pointer---*/
    if (partinfo->type == QTParted::extended) itemextended = item;
}

void QP_RealListView::addLogical(QP_PartInfo *partinfo, int number) {
    // Not supported by DOS-style partition tables
    //  const char *nameStr = ped_partition_get_name( part );
    //  QString name( nameStr ? nameStr : "" );

    /*---set label for start, end and size---*/
    QString startStr = MB2String(partinfo->mb_start());
    QString endStr = MB2String(partinfo->mb_end());
    QString sizeStr = MB2String(partinfo->mb_end()-partinfo->mb_start());
    QString snumber; snumber.sprintf("%.2d", number);
    QString label = partinfo->label();
    
    QString status;
    if (partinfo->isActive()) status = QString(tr("Active"));
                      else status = QString::null;

    if (partinfo->isHidden()) {
        if (!status.isNull()) status += "/";
        status += QString(tr("Hidden"));
    }

    QString usedStr;
    if (partinfo->min_size == -1)
        usedStr = QString(tr("N/A"));
    else
        usedStr = MB2String(partinfo->mb_min_size());
    
//    printf("position: %2d %s\n", partinfo->position, partinfo->partname().latin1());

    /*---make a new line (every line is a logical partition)---*/
    new QP_ListViewItem(itemextended,
                        snumber,
                        partinfo->partname(),
                        partinfo->fsspec->name(),
                        status,
                        sizeStr, usedStr, startStr, endStr,
                        label,
                        partinfo);

    /*---open the extended tree---*/
    itemextended->setOpen(true);
}


void QP_RealListView::selectionChanged(QListViewItem *i) {
    /*---emit the sigSelectPart signal---*/
    QP_ListViewItem *di = static_cast<QP_ListViewItem *>(i);
    emit sigSelectPart(di->partinfo);
}


void QP_RealListView::rightButtonClicked(QListViewItem *, const QPoint &point, int ) {
    /*---emit the sigPopup signal---*/
  emit sigPopup(point);
}
/*-----------------------------------------------------------------------------------*/





/*----------QP_ListView--------------------------------------------------------------*/
/*---                                                                             ---*/
QP_ListView::QP_ListView(QWidget *parent, const char *name, WFlags f)
    :QP_PartList(parent, name, f) {

    /*---init progressive number---*/
    number = 0;

    /*---listview is the "real" listview!---*/
    listview = new QP_RealListView(this);

    /*---QP_ListView must expand both horizontal or verticaly!---*/
    setMinimumWidth(MIN_HDWIDTH);
    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

    /*---connect sigPopup and sigSelectPart with the signals of the RealListView---*/
    connect(listview, SIGNAL(sigSelectPart(QP_PartInfo *)),
            this, SIGNAL(sigSelectPart(QP_PartInfo *)));
    connect(listview, SIGNAL(sigPopup(QPoint)),
            this, SIGNAL(sigPopup(QPoint)));
}

QP_ListView::~QP_ListView() {
    delete listview;
}

void QP_ListView::setselPartInfo(QP_PartInfo *selpartinfo) {
    /*---call the ancestor to change the selection---*/
    QP_PartList::setselPartInfo(selpartinfo);

    /*---just a wrap to the QP_RealListView---*/
    listview->setSelected((QListViewItem *)selpartinfo->listviewitem, true);
}

void QP_ListView::setDevice(QP_Device *device) {
    QP_PartList::setDevice(device);
    listview->setDevice(device);
}

void QP_ListView::clear() {
    /*---init progressive number---*/
    number = 0;

    /*---call the ancestor to init the selected partinfo---*/
    QP_PartList::setselPartInfo(NULL);

    /*---just a wrap to the QP_RealListView---*/
    listview->clear();
}

void QP_ListView::addPrimary(QP_PartInfo *partinfo) {
    /*---just a wrap to the QP_RealListView---*/
    listview->addPrimary(partinfo, ++number);
}

void QP_ListView::addLogical(QP_PartInfo *partinfo) {
    /*---just a wrap to the QP_RealListView---*/
    listview->addLogical(partinfo, ++number);
}

void QP_ListView::resizeEvent(QResizeEvent *event) {
    /*---resize QP_RealListView to fit the ListView wrapper---*/
    listview->resize(event->size());
}
/*-----------------------------------------------------------------------------------*/
