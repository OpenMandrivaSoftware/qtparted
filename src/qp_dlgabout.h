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


/* About QP_dlgAbout class:
 *
 * This is a very simple class that make a small dialog box used for about
 *
 * A lot of code is bring from AboutDlg class made by Justin Karneges for the
 * PSI jabber client.
 *
 */

#ifndef QP_DLGABOUT_H
#define QP_DLGABOUT_H

#include <qdialog.h>

class QP_dlgAbout : public QDialog {
    Q_OBJECT
public:
    QP_dlgAbout(const QPixmap &icon, const QString &content, QWidget *par=0);
};

#endif
