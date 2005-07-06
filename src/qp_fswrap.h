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

#include <config.h>

#include <qstring.h>
#include <qstringlist.h>
#include <stdarg.h>
#include <qobject.h>
#include "qp_libparted.h"
#include "qptypes.h"

#define swab16(x) ((u16)( (((u16)(x) & (u16)0x00ffU) << 8) | (((u16)(x) & (u16)0xff00U) >> 8) ))

#define swab32(x) ((u32)( (((u32)(x) & (u32)0x000000ffUL) << 24) | \
                          (((u32)(x) & (u32)0x0000ff00UL) <<  8) | \
                          (((u32)(x) & (u32)0x00ff0000UL) >>  8) | \
                          (((u32)(x) & (u32)0xff000000UL) >> 24) ))
#define swab64(x) ((u64)(  (u64)(((u64)(x) & (u64)0x00000000000000ffULL) << 56) | \
                           (u64)(((u64)(x) & (u64)0x000000000000ff00ULL) << 40) | \
                           (u64)(((u64)(x) & (u64)0x0000000000ff0000ULL) << 24) | \
                           (u64)(((u64)(x) & (u64)0x00000000ff000000ULL) <<  8) | \
                           (u64)(((u64)(x) & (u64)0x000000ff00000000ULL) >>  8) | \
                           (u64)(((u64)(x) & (u64)0x0000ff0000000000ULL) >> 24) | \
                           (u64)(((u64)(x) & (u64)0x00ff000000000000ULL) >> 40) | \
                           (u64)(((u64)(x) & (u64)0xff00000000000000ULL) >> 56) )) 

// ------------- CPU TO XXX ---------------
#ifdef ENDIAN_BIG // BIG ENDIAN
#  define CpuToLe16(a) (swab16(a))
#  define CpuToBe16(a) (a)
#  define CpuToLe32(a) (swab32(a)) 
#  define CpuToBe32(a) (a)
#  define CpuToLe64(a) (swab64(a))
#  define CpuToBe64(a) (a)
#else // LITTLE_ENDIAN
#  define CpuToLe16(a) (a)
#  define CpuToBe16(a) (swab16(a))
#  define CpuToLe32(a) (a)
#  define CpuToBe32(a) (swab32(a))
#  define CpuToLe64(a) (a)
#  define CpuToBe64(a) (swab64(a))
#endif // ENDIAN

#define CpuToLe(a) \
    ((sizeof(a) == 8) ? CpuToLe64(a) : \
    ((sizeof(a) == 4) ? CpuToLe32(a) : \
    ((sizeof(a) == 2) ? CpuToLe16(a) : \
    (a))))

#define CpuToBe(a) \
    ((sizeof(a) == 8) ? CpuToBe64(a) : \
    ((sizeof(a) == 4) ? CpuToBe32(a) : \
    ((sizeof(a) == 2) ? CpuToBe16(a) : \
    (a))))

// ------------- XXX TO CPU --------------
#define Le16ToCpu(a) CpuToLe16(a)
#define Le32ToCpu(a) CpuToLe32(a)
#define Le64ToCpu(a) CpuToLe64(a)

#define Be16ToCpu(a) CpuToBe16(a)
#define Be32ToCpu(a) CpuToBe32(a)
#define Be64ToCpu(a) CpuToBe64(a)

#define LeToCpu(a) \
    ((sizeof(a) == 8) ? Le64ToCpu(a) : \
    ((sizeof(a) == 4) ? Le32ToCpu(a) : \
    ((sizeof(a) == 2) ? Le16ToCpu(a) : \
    (a))))

#define BeToCpu(a) \
    ((sizeof(a) == 8) ? Be64ToCpu(a) : \
    ((sizeof(a) == 4) ? Be32ToCpu(a) : \
    ((sizeof(a) == 2) ? Be16ToCpu(a) : \
    (a))))

#define NTFS_GETU8(p)      (*(u8*)(p))
#define NTFS_GETU16(p)     ((u16)Le16ToCpu(*(u16*)(p)))
#define NTFS_GETU24(p)     ((u32)NTFS_GETU16(p) | ((u32)NTFS_GETU8(((char*)(p))+2)<<16))
#define NTFS_GETU32(p)     ((u32)Le32ToCpu(*(u32*)(p)))
#define NTFS_GETU40(p)     ((u64)NTFS_GETU32(p)|(((u64)NTFS_GETU8(((char*)(p))+4))<<32))
#define NTFS_GETU48(p)     ((u64)NTFS_GETU32(p)|(((u64)NTFS_GETU16(((char*)(p))+4))<<32))
#define NTFS_GETU56(p)     ((u64)NTFS_GETU32(p)|(((u64)NTFS_GETU24(((char*)(p))+4))<<32))
#define NTFS_GETU64(p)     ((u64)Le64ToCpu(*(u64*)(p)))

#define NTFS_GETS8(p)        ((*(s8*)(p)))
#define NTFS_GETS16(p)       ((s16)Le16ToCpu(*(s16*)(p)))
#define NTFS_GETS24(p)       (NTFS_GETU24(p) < 0x800000 ? (s32)NTFS_GETU24(p) : (s32)(NTFS_GETU24(p) - 0x1000000))
#define NTFS_GETS32(p)       ((s32)Le32ToCpu(*(s32*)(p)))
#define NTFS_GETS40(p)       (((s64)NTFS_GETU32(p)) | (((s64)NTFS_GETS8(((char*)(p))+4)) << 32))
#define NTFS_GETS48(p)       (((s64)NTFS_GETU32(p)) | (((s64)NTFS_GETS16(((char*)(p))+4)) << 32))
#define NTFS_GETS56(p)       (((s64)NTFS_GETU32(p)) | (((s64)NTFS_GETS24(((char*)(p))+4)) << 32))
#define NTFS_GETS64(p)	     ((s64)NTFS_GETU64(p))

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
    
    /*---return the "resize" minimal size---*/
    virtual PedSector min_size(QString) {return _min_size;}

    /*---return the right wrapper (if a wrapper exist ;-))---*/
    static QP_FSWrap *fswrap(QString);

    /*---return the label---*/
    static QString get_label(PedPartition *, QString);

    /*---read a sector---*/
    static bool read_sector(PedPartition *, PedSector, PedSector, char *buffer);

    int wrap_resize;
    bool wrap_move;
    bool wrap_copy;
    bool wrap_create;
    bool wrap_min_size;

protected:
    bool qpMount(QString device);
    bool qpUMount(QString device);
    bool fs_open(QString cmdline);
    char *fs_getline();
    int fs_close();
    QString _message;
    PedSector _min_size;

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
    PedSector min_size(QString);
    QString fsname();
    static QString _get_label(PedPartition *);

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
    static QString _get_label(PedPartition *);
    
private:
    bool jfsresize(bool, QP_PartInfo *, PedSector newsize); //this is the true jfsresize wrapper
};

class QP_FSExt3 : public QP_FSWrap {
Q_OBJECT

public:
    QP_FSExt3();
    bool mkpartfs(QString dev, QString label);
    QString fsname();
    static QString _get_label(PedPartition *);
};

class QP_FSXfs : public QP_FSWrap {
Q_OBJECT

public:
    QP_FSXfs();
    bool mkpartfs(QString dev, QString label);
    bool resize(QP_LibParted *, bool write, QP_PartInfo *, PedSector, PedSector);
    QString fsname();
    static QString _get_label(PedPartition *);
    
private:
    bool xfsresize(bool, QP_PartInfo *, PedSector newsize); //this is the true xfsresize wrapper
};

class QP_FSFat16 : public QP_FSWrap {
Q_OBJECT

public:
    static QString _get_label(PedPartition *);
};

class QP_FSFat32 : public QP_FSWrap {
Q_OBJECT

public:
    static QString _get_label(PedPartition *);
};

class QP_FSExt2 : public QP_FSWrap {
Q_OBJECT

public:
    static QString _get_label(PedPartition *);
};

class QP_FSReiserFS : public QP_FSWrap {
Q_OBJECT

public:
    static QString _get_label(PedPartition *);
};

#endif
