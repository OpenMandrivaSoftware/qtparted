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

/* About QP_FSWrap class
 *
 * This is a class that "wrap" some filesystem not supported by libparted
 */

#ifndef QP_FSWRAP_H
#define QP_FSWRAP_H

#include <qstring.h>
#include <qstringlist.h>
#include <stdarg.h>
#include <qobject.h>
#include "qp_libparted.h"

class QP_FSNtfs;

class QP_FSWrap : public QObject {
Q_OBJECT

public:
    /*---resize(libparted, readonly?, device, sectors_start, sectors_end), resize the partition---*/
    virtual bool resize(QP_LibParted *, bool, QP_PartInfo *, PedSector, PedSector) {return false;}

    /*---move(device, start sectors, end sectors), move the partition---*/
    virtual bool move(QString, PedSector, PedSector) {return false;}

    /*---mkpartfs(device, label) create a new partition---*/
    virtual bool mkpartfs(QString, QString) {return false;}

    /*---return a string with the latest error---*/
    virtual QString message() {return _message;}

    /*---return the name of the filesystem---*/
    virtual QString fsname() {return QString::null;}

    /*---return the right wrapper (if a wrapper exist ;-))---*/
    static QP_FSWrap *fswrap(QString name);

    int wrap_resize;
    bool wrap_move;
    bool wrap_copy;
    bool wrap_create;

protected:
    bool qpMount(QString device);
    bool qpUMount(QString device);
    bool fs_open(QString cmdline);
    char *fs_getline();
    int fs_close();
    QString _message;

private:
    char line[255];
    FILE *fp;

signals:
    /*---emitted when there is need to update a progress bar---*/
    void sigTimer(int, QString, QString);
};

class QP_FSNtfs : public QP_FSWrap {
Q_OBJECT

public:
    QP_FSNtfs();
    bool resize(QP_LibParted *, bool, QP_PartInfo *, PedSector, PedSector);
    bool mkpartfs(QString dev, QString label);
    QString fsname();

private:
    bool ntfsresize(bool, QString dev, PedSector newsize); //this is the true ntfsresize wrapper
};

class QP_FSJfs : public QP_FSWrap {
Q_OBJECT

public:
    QP_FSJfs();
    bool resize(QP_LibParted *, bool write, QP_PartInfo *, PedSector, PedSector);
    bool mkpartfs(QString dev, QString label);
    QString fsname();
    
private:
    bool jfsresize(bool, QP_PartInfo *, PedSector newsize); //this is the true jfsresize wrapper
};

class QP_FSExt3 : public QP_FSWrap {
Q_OBJECT

public:
    QP_FSExt3();
    bool mkpartfs(QString dev, QString label);
    QString fsname();
};

class QP_FSXfs : public QP_FSWrap {
Q_OBJECT

public:
    QP_FSXfs();
    bool mkpartfs(QString dev, QString label);
    bool resize(QP_LibParted *, bool write, QP_PartInfo *, PedSector, PedSector);
    QString fsname();
    
private:
    bool xfsresize(bool, QP_PartInfo *, PedSector newsize); //this is the true xfsresize wrapper
};

#endif
