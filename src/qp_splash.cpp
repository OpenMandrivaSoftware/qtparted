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
#include <qpixmap.h>
#include <qfontmetrics.h>
#include <qpainter.h>

#include "qp_splash.h"
#include "qp_options.h"


QP_Splash::QP_Splash(QWidget *parent, const char *name)
    :QVBox(parent, name, WType_Modal | WStyle_Customize | WStyle_NoBorder | WDestructiveClose) {

    setMargin(0);
    setSpacing(0);

    QLabel *picLabel = new QLabel(this);
    QPixmap pixmap;
    if (pixmap.load(DATADIR "/pics/qtp_splash.png"));
        picLabel->setPixmap(pixmap);

    m_infoBox = new QLabel(this);
    m_infoBox->setMargin(2);
    m_infoBox->setPaletteBackgroundColor(black);
    m_infoBox->setPaletteForegroundColor(white);

}


QP_Splash::~QP_Splash() {
}


void QP_Splash::mousePressEvent(QMouseEvent *) {
  close();
}


void QP_Splash::addInfo(const QString &s) {
    m_infoBox->setText(s);
    qApp->processEvents();
}
