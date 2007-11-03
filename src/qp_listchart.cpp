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

#include <qpainter.h>

#include "qp_libparted.h"
#include "qp_listchart.moc"
#include "qp_options.h"

#define MIN_HDWIDTH         190
#define MIN_HDHEIGHT         70
#define MIN_PARTITION_WIDTH  16

class QP_ChartItem {
public:
    QP_PartWidget *partwidget;
    QP_PartInfo *partinfo;
    int width;
};

QP_ListChart::QP_ListChart(QWidget *parent, const char *name, WFlags f)
    :QP_PartList(parent, name, f) {

    /*---prevent memory leak when you call ::clear method!---*/
    partlist.setAutoDelete(true);
    logilist.setAutoDelete(true);

    /*---prevent a segfualt in ::clear method!---*/
    _selPartInfo = NULL;

    container = new QWidget(this);

    setMinimumWidth(MIN_HDWIDTH);
    setMinimumHeight(MIN_HDHEIGHT);
}

QP_ListChart::~QP_ListChart() {
}

void QP_ListChart::setselPartInfo(QP_PartInfo *selpartinfo) {
    /*---if a partition was selected remove the selection to it---*/
    /*---please note that in ::clear() selPartInfo is "nulled!"---*/
    if (selPartInfo()) {
        /*---redraw the (un)selected partition---*/
        QP_PartWidget *selwidget = selPartInfo()->partwidget;
        selwidget->setSelected(false);
        selwidget->update();
    }

    /*---call the ancestor to change the selection---*/
    QP_PartList::setselPartInfo(selpartinfo);

    /*---redraw the selected partition---*/
    QP_PartWidget *selwidget = selPartInfo()->partwidget;
    selwidget->setSelected(true);
    selwidget->update();
}

void QP_ListChart::clear() {
    /*--just "unselect" a partition. This prevent segfault in setselpartinfo---*/
    _selPartInfo = NULL;
    
    QP_ChartItem *p = NULL;

    /*---destroy logical partition widgets attached---*/
    for (p = (QP_ChartItem *)logilist.first(); p; p = (QP_ChartItem *)logilist.next()) 
        delete p->partwidget;

    /*---destroy primary/extended partition widgets attached---*/
    p = NULL;
    for (p = (QP_ChartItem *)partlist.first(); p; p = (QP_ChartItem *)partlist.next()) 
        delete p->partwidget;

    /*---clear the pointer list of partition  ---*/
    /*---this is usefull to avoid memory leak!---*/
    logilist.clear();
    partlist.clear();
}

void QP_ListChart::addPrimary(QP_PartInfo *partinfo) {
    /*---append to the partition list---*/
    //partlist.append(partinfo); aleste
    QP_ChartItem *item = new QP_ChartItem();
    item->partinfo = partinfo;
    partlist.append(item); //aleste

    if (partinfo->type == QTParted::primary) {
        /*---create a new primary partition  ---*/
        partinfo->partwidget = new QP_Partition(partinfo, container);
    } else {
        /*---create a new extended partition ---*/
        partinfo->partwidget = new QP_Extended(partinfo, container);
        extpartinfo = partinfo;
    }

    //aleste
    item->partwidget = partinfo->partwidget;

    /*---show the widget---*/
    partinfo->partwidget->show();

    /*---connect sigPopup and sigSelectPart---*/
    setSignals(partinfo->partwidget);
}

void QP_ListChart::addLogical(QP_PartInfo *partinfo) {
    /*---append to the partition list---*/
    QP_ChartItem *item = new QP_ChartItem();
    item->partinfo = partinfo;
    logilist.append(item);

    QP_Extended *extended = (QP_Extended *)extpartinfo->partwidget;

    /*---add a logical partition the the extended---*/
    partinfo->partwidget = extended->addLogical(partinfo);
    item->partwidget = partinfo->partwidget;//aleste
    
    /*---show the widget---*/
    partinfo->partwidget->show();

    /*---connect sigPopup and sigSelectPart---*/
    setSignals(partinfo->partwidget);
}

void QP_ListChart::draw() {
    /*---in this method partitions are resized to fit the container   ---*/
    /*---only primaries and extended partitions are resized!          ---*/
    /*---Logicals partitions are resized inside draw_extended method! ---*/

    
    /*---return if the partition list is empty... for example at startup ;)---*/
    if (partlist.count() == 0) return;

    /*---totwidth contain the minimal width of the QP_ListChart---*/
    int totwidth = 0;

    /*---the first "for" loop is needed to calculate the totwidth---*/
    QP_ChartItem *p = NULL;
    for (p = (QP_ChartItem *)partlist.first(); p; p = (QP_ChartItem *)partlist.next()) {
        totwidth += p->width = MIN_PARTITION_WIDTH;
        if (totwidth > container->width())
            qFatal("Error calculating size of QP_ListChar! %d\n", container->width());
    }


    /*---resize the QP_ListChart---*/
    if (totwidth > MIN_HDWIDTH)
        setMinimumWidth(totwidth);


    /*---this "for" loop is needed to calculate how much partitions can grow---*/
    /*---it loop until partition fit the container                          ---*/
    p = NULL;
    while (totwidth < container->width()) {
        if (p == NULL)
            p = (QP_ChartItem *)partlist.first();

        /*---calculate the size of the partition---*/
        float mbsize = p->partinfo->mb_end() - p->partinfo->mb_start();

        /*---calculate the width of the partition---*/
        int newwidth = (int)((mbsize*container->width())/mb_hdsize() + 1);

        /*---if the actual width is less then the width calculate increment it---*/
        if (p->width < newwidth) {
            p->width++;
            totwidth++;
        }

        p = (QP_ChartItem *)partlist.next();
    }

    /*---the last "for" loop is needed to resize the partitions with the calculated width---*/
    int lastleft = 0;
    totwidth = 0;
    for (p = (QP_ChartItem *)partlist.first(); p; p = (QP_ChartItem *)partlist.next()) {
        QP_PartWidget *partwidget = p->partwidget;
        partwidget->setGeometry(lastleft, 0, p->width, container->height());
        lastleft += p->width;

        if ((p->partinfo->type == QTParted::extended)
        &&  (logilist.count()  != 0)) {
            /*---loop inside for size for logicals partitions ;)---*/
            draw_extended();

            /*---draw_extended changed the extended size!---*/
            if (p->width != partwidget->width())
                lastleft += (partwidget->width() - p->width);
        }
        totwidth += partwidget->width();
        if ((partwidget->x()+partwidget->width()) > container->width()) {
            container->setMinimumWidth(totwidth);
            setMinimumWidth(totwidth+12);
        }
    }
}

void QP_ListChart::draw_extended() {
    /*taccon: cambiare extended->container!*/
    /*---in this method logical partitions are resized to fit the container---*/

    /*---get extended partition---*/
    QP_Extended *extended = (QP_Extended *)extpartinfo->partwidget;

    /*---totwidth contain the minimal width of the Extended partition---*/
    int totwidth = 0;

    /*---the first "for" loop is needed to calculate the totwidth---*/
    QP_ChartItem *p = NULL;
    for (p = (QP_ChartItem *)logilist.first(); p; p = (QP_ChartItem *)logilist.next()) {
        totwidth += p->width = MIN_PARTITION_WIDTH;
        //if (totwidth > extended->container->width())
        //    qFatal("Error calculating size of QP_ListChar! %d\n", container->width());
    }

    /*---if the width of the widget is not enough for extended resize it!---*/
    if (totwidth != extended->container->width())
        extended->setMinimumWidth(extended->width()+(totwidth - extended->container->width()));

    /*---this "for" loop is needed to calculate how much partitions can grow---*/
    /*---it loop until partition fit the container                          ---*/
    p = NULL;
    while (totwidth < extended->container->width()) {
        if (p == NULL)
            p = (QP_ChartItem *)logilist.first();

        /*---calculate the size of the extended partition---*/
        float extsize = extpartinfo->end - extpartinfo->start;

        /*---calculate the size of the partition---*/
        float mbsize = p->partinfo->end - p->partinfo->start;

        /*---calculate the width of the partition---*/
        int newwidth = (int)((mbsize*extended->container->width())/extsize + 1);

        /*---if the actual width is less then the width calculate increment it---*/
        if (p->width < newwidth) {
            p->width++;
            totwidth++;
        }

        p = (QP_ChartItem *)logilist.next();
    }

    /*---the last "for" loop is needed to resize the partitions with the calculated width---*/
    int lastleft = 0;
    for (p = (QP_ChartItem *)logilist.first(); p; p = (QP_ChartItem *)logilist.next()) {
        QP_PartWidget *partwidget = p->partwidget;
        partwidget->setGeometry(lastleft, 0, p->width, extended->container->height());
        lastleft += p->width;
    }
}

void QP_ListChart::paintEvent(QPaintEvent *) {
    QPainter paint(this);
    QColor color = Qt::white;
    paint.fillRect(2, 2, width()-4, MIN_HDHEIGHT-4, color);
}

void QP_ListChart::resizeEvent(QResizeEvent *e) {
    int w = e->size().width();
    //int h = e->size().height();
    container->setGeometry(6, 6, w-12, MIN_HDHEIGHT-12);
    draw();
}

void QP_ListChart::mouseReleaseEvent(QMouseEvent *e) {
    /*---call the "sigDevicePopup", used to display the popupmenu context---*/
    if (device() && e->button() == Qt::RightButton) 
        emit sigDevicePopup(mapToGlobal(e->pos()));

}

void QP_ListChart::setSignals(QP_PartWidget *partwidget) {
    connect(partwidget, SIGNAL(sigPopup(QPoint)),
            this, SIGNAL(sigPopup(QPoint)));
    connect(partwidget, SIGNAL(sigSelectPart(QP_PartInfo *)),
            this, SIGNAL(sigSelectPart(QP_PartInfo *)));
}
