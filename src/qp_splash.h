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

/* About QP_Splash class:
 *
 * This class is used for the splash screen of QTParted
 * Almost all code is bring from the splash screen of K3b (by Sebastian Trueg)
 */


#include <qvbox.h>
#include <qlabel.h>
#include <qstring.h>
#include <qevent.h>

#ifndef QP_SPLASH_H
#define QP_SPLASH_H

class QP_Splash : public QVBox {
Q_OBJECT

public:
    QP_Splash(QWidget *parent = 0, const char *name = 0);
    ~QP_Splash();

public slots:
    void addInfo(const QString &);

protected:
    void mousePressEvent(QMouseEvent *);

private:
    QLabel *m_infoBox;
};

#endif
