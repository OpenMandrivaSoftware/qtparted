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

#include "qp_settings.h"
#include "qp_common.h"
#include "qp_options.h"

QP_Settings::QP_Settings() {
#ifndef QT30COMPATIBILITY
    settings.setPath(PROG_NAME, PROG_NAME);
#endif
    _layout = settings.readNumEntry("/qtparted/layout", 0);

    QString mkntfs_path = settings.readEntry("/qtparted/" MKNTFS, MKNTFS_PATH);
    QString ntfsresize_path = settings.readEntry("/qtparted/" NTFSRESIZE, NTFSRESIZE_PATH);
    QString mkfs_ext3_path = settings.readEntry("/qtparted/" MKFS_EXT3, MKFS_EXT3_PATH);
    QString mkfs_jfs_path = settings.readEntry("/qtparted/" MKFS_JFS, MKFS_JFS_PATH);
    QString mkfs_xfs_path = settings.readEntry("/qtparted/" MKFS_XFS, MKFS_XFS_PATH);

    lstExternalTools->add(MKNTFS, 
                          mkntfs_path,
                          QObject::tr("A program that create NTFS partitions."));
    lstExternalTools->add(NTFSRESIZE, 
                          ntfsresize_path,
                          QObject::tr("A program that resize NTFS partitions."));
    lstExternalTools->add(MKFS_EXT3, 
                          mkfs_ext3_path,
                          QObject::tr("A program that create EXT3 partitions."));
    lstExternalTools->add(MKFS_JFS, 
                          mkfs_jfs_path,
                          QObject::tr("A program that create JFS partitions."));
    lstExternalTools->add(MKFS_XFS, 
                          mkfs_xfs_path,
                          QObject::tr("A program that create XFS partitions."));
}

QP_Settings::~QP_Settings() {
}

int QP_Settings::layout() {
    return _layout;
}

void QP_Settings::setLayout(int layout) {
    settings.writeEntry("/qtparted/layout", layout);
    _layout = layout;

    QString mkntfs_path = lstExternalTools->getPath(MKNTFS);
    QString ntfsresize_path = lstExternalTools->getPath(NTFSRESIZE);
    QString mkfs_ext3_path = lstExternalTools->getPath(MKFS_EXT3);
    QString mkfs_jfs_path = lstExternalTools->getPath(MKFS_JFS);
    QString mkfs_xfs_path = lstExternalTools->getPath(MKFS_XFS);

    settings.writeEntry("/qtparted/" MKNTFS, mkntfs_path);
    settings.writeEntry("/qtparted/" NTFSRESIZE, ntfsresize_path);
    settings.writeEntry("/qtparted/" MKFS_EXT3, mkfs_ext3_path);
    settings.writeEntry("/qtparted/" MKFS_JFS, mkfs_jfs_path);
    settings.writeEntry("/qtparted/" MKFS_XFS, mkfs_xfs_path);
}

time_t QP_Settings::getDevUpdate(QString device) {
    QString entry = QString("%1%2")
                    .arg("/qtparted")
                    .arg(device);
    
    QString time = settings.readEntry(entry, "0");

    long long longTime;
    sscanf(time.latin1(), "%lld", &longTime);

    return (time_t)longTime;
}

void QP_Settings::setDevUpdate(QString device, time_t time) {
    QString entry = QString("%1%2")
                    .arg("/qtparted")
                    .arg(device);
    
    char buf[255];
    sprintf(buf, "%lld", (long long)time);

    settings.writeEntry(entry, buf);
}
