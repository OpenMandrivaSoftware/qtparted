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
#include "qp_options.h"


QP_ListExternalTools::QP_ListExternalTools() {
    /*---prevent from memory leak: when list are cleared destroy exttool object!---*/
    lstTools.setAutoDelete(true);
    lstToolsOld.setAutoDelete(true);
}

QP_ListExternalTools::~QP_ListExternalTools() {
}

void QP_ListExternalTools::add(QString name, QString path, QString description) {
    QP_ExternalTool *exttool = new QP_ExternalTool(name, path, description);
    lstTools.append(exttool);

    QP_ExternalTool *exttool2 = new QP_ExternalTool(name, path, description);
    lstToolsOld.append(exttool2);
}

QString QP_ListExternalTools::getPath(QString name) {
    QP_ExternalTool *p;
    for (p = (QP_ExternalTool *)lstTools.first(); p; p = (QP_ExternalTool *)lstTools.next()) {
        if (p->name().compare(name) == 0) {
            return p->path();
        }
    }    
    return QString::null;
}

void QP_ListExternalTools::setPath(QString name, QString path) {
    QP_ExternalTool *p;
    for (p = (QP_ExternalTool *)lstTools.first(); p; p = (QP_ExternalTool *)lstTools.next()) {
        if (p->name().compare(name) == 0) {
            p->setPath(path);
        }
    }    
}

QString QP_ListExternalTools::getDescription(QString name) {
    QP_ExternalTool *p;
    for (p = (QP_ExternalTool *)lstTools.first(); p; p = (QP_ExternalTool *)lstTools.next()) {
        if (p->name().compare(name) == 0) {
            return p->description();
        }
    }    

    return QString::null;
}

void QP_ListExternalTools::apply() {
    lstToolsOld.clear();
    QP_ExternalTool *p;
    for (p = (QP_ExternalTool *)lstTools.first(); p; p = (QP_ExternalTool *)lstTools.next()) {
        QP_ExternalTool *p_new = new QP_ExternalTool(p->name(), p->path(), p->description());
        lstToolsOld.append(p_new);
    }
}

void QP_ListExternalTools::cancel() {
    lstTools.clear();
    QP_ExternalTool *p;
    for (p = (QP_ExternalTool *)lstToolsOld.first(); p; p = (QP_ExternalTool *)lstToolsOld.next()) {
        QP_ExternalTool *p_new = new QP_ExternalTool(p->name(), p->path(), p->description());
        lstTools.append(p_new);
    }
}
