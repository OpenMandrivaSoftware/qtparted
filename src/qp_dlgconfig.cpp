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

#include <qcombobox.h>
#include <qlineedit.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "qp_exttools.h"
#include "qp_dlgconfig.moc"
#include "qp_common.h"
#include "qp_options.h"

QP_dlgConfig::QP_dlgConfig(QWidget *p):Ui_QP_UIConfig() {
    /*---clear combo box used for external tools---*/
    cmbExtTools->clear();

    QP_ExternalTool *p;
    for (p = (QP_ExternalTool *)lstExternalTools->lstTools.first(); p;
            p = (QP_ExternalTool *)lstExternalTools->lstTools.next())
        cmbExtTools->insertItem(p->name());

    /*---connect the combo type slot---*/
    connect(cmbExtTools, SIGNAL(activated(int)),
            this, SLOT(slotToolChanged(int)));

    /*---connect the path text changed slot---*/
    connect(txtPath, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotPathChanged(const QString &)));
}

QP_dlgConfig::~QP_dlgConfig() {
}

void QP_dlgConfig::init_dialog() {
    slotToolChanged(0);
}

int QP_dlgConfig::show_dialog() {
    return exec();
}

int QP_dlgConfig::layout() {
    return cmbLayout->currentItem();
}

void QP_dlgConfig::setLayout(int layout) {
    cmbLayout->setCurrentItem(layout);
}

void QP_dlgConfig::slotToolChanged(int) {
    QString path = lstExternalTools->getPath(cmbExtTools->currentText());
    txtPath->setText(path);

    QString tooltip = lstExternalTools->getDescription(cmbExtTools->currentText());
    QToolTip::add(txtPath, tooltip);
    QWhatsThis::add(txtPath, tooltip);
}

void QP_dlgConfig::slotPathChanged(const QString &path) {
    lstExternalTools->setPath(cmbExtTools->currentText(), path);
}
