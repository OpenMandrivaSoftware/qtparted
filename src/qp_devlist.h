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

/* About QP_DevList class:
 *
 * This class keep a list with all devices (ie all hard disk) detected by libparted
 */


#ifndef QP_DEVLIST_H
#define QP_DEVLIST_H

#include <time.h>
#include <qstring.h>
#include <qptrlist.h>
#include "qp_settings.h"


class QP_Device {
public:
    QP_Device(QP_Settings *);
    ~QP_Device();
    bool newPartTable();          //make a new partition table
    QString longname();           //return the longname (only with devfs filesystem)
    void setLongname(QString);    //set the longname
    QString shortname();          //return the shortname (ie /dev/hda, /dev/sda, /etc /etc)
    void setShortname(QString);   //set the shortname
    bool isBusy();                //return if the partition is mounted (ie busy)
    void setIsBusy(bool);         //set if this is busy
    void *data();                 //return stuff ;)
    void setData(void *);         //set data stuff ;)
    bool partitionTable();        //return if has a partition table
    void setPartitionTable(bool); //set if it has a partition table
    bool devfsEnabled();          //return if the devfs is enabled
    void setDevfsEnabled(bool);   //set if the devfs is enabled
    bool canUpdateGeometry();     //return if the geometry of the device can be changed
    void commit();                //the device was commited!

private:
    int convertDevfsNameToShortName(const char *, char *, int);
    QString _longname;
    QString _shortname;
    bool _isBusy;
    void *_data;
    bool _partitionTable;
    bool _devfsEnabled;
    QP_Settings *settings;
};


class QP_DevList {
public:
    QP_DevList(QP_Settings *);
    ~QP_DevList();
    bool devfsEnabled();
    void getDevices();          //probe all devices
    QPtrList<QP_Device> devlist;

private:
    int _getDevfsEnabled();
    bool _devfsEnabled;
    QP_Settings *_settings;
};

#endif

