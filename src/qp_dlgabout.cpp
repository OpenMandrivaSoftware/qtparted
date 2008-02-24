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

#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

#include "qp_dlgabout.moc"
#include "qp_options.h"

QP_dlgAbout::QP_dlgAbout(const QPixmap &icon, const QString &content, QWidget *par)
    :QDialog(par, 0) {
	QLabel *l;
	QVBoxLayout *vb = new QVBoxLayout(this);
	
	QHBoxLayout *hb = new QHBoxLayout();
	vb->setMargin(8);
	vb->addLayout(hb);

	QVBoxLayout *col = new QVBoxLayout();
	hb->addLayout(col);

	col = new QVBoxLayout();
	hb->addLayout(col,1);
	l = new QLabel(this);
	l->setPixmap(icon);
	col->addWidget(l);

	col = new QVBoxLayout();
	hb->addLayout(col,1);
	l = new QLabel(this);
	l->setText(content);
	col->addWidget(l);

	hb = new QHBoxLayout();
	vb->addLayout(hb,1);
	QPushButton *pb_ok = new QPushButton(tr("&OK"), this);
	connect(pb_ok, SIGNAL(clicked()), SLOT(close()));
	hb->addWidget(pb_ok);
}
