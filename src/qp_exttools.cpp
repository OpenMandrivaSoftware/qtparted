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

#include <stdio.h>

#include "qp_exttools.h"

QP_ListExternalTools::QP_ListExternalTools() {
    /*---prevent from memory leak: when list are cleared destroy exttool object!---*/
    //lstTools.setAutoDelete(true);
    //lstToolsOld.setAutoDelete(true);
}

QP_ListExternalTools::~QP_ListExternalTools() {
}

void QP_ListExternalTools::add(QString name, QString path, QString description) {
    QP_ExternalTool *exttool = new QP_ExternalTool(name, path, description);
    lstTools.append(exttool);
}

QString QP_ListExternalTools::getPath(QString name) {
    foreach(QP_ExternalTool *p, lstTools) {
        if (p->name().compare(name) == 0) {
            return p->path();
        }
    }    
    return QString::null;
}

void QP_ListExternalTools::setPath(QString name, QString path) {
    foreach(QP_ExternalTool *p, lstTools) {
        if (p->name().compare(name) == 0) {
            p->setPath(path);
        }
    }    
}

QString QP_ListExternalTools::getDescription(QString name) {
    foreach(QP_ExternalTool *p, lstTools) {
        if (p->name().compare(name) == 0) {
            return p->description();
        }
    }    

    return QString::null;
}
