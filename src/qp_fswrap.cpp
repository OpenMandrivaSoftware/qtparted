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

#include <qregexp.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <qapplication.h>

#include "qp_fswrap.h"
#include "qp_actlist.h"
#include "qp_common.h"
#include "qp_options.h"
#include "qp_debug.h"

#define NOTFOUND tr("command not found")
#define TMP_MOUNTPOINT "/tmp/mntqp"

#define my_min(a,b) ((a)<(b) ? (a):(b))

bool QP_FSWrap::fs_open(QString cmdline) {

    /*---this force stderr to stdout---*/
    QString dupcmdline = QString("%1 %2")
                    .arg(cmdline)
                    .arg(QString("2>&1"));

    /*---open a pipe from the command line---*/
    fp = popen(dupcmdline, "r");

    if (fp)
        return true;
    else
        return false;
}

char * QP_FSWrap::fs_getline() {
    bool rc = fgets(line, sizeof line, fp);

    if (rc) return line;
    else    return NULL;
}

int QP_FSWrap::fs_close() {
    return fclose(fp);
}

QP_FSWrap * QP_FSWrap::fswrap(QString name) {
    if (name.compare("ntfs") == 0) {
        QP_FSWrap *fswrap = new QP_FSNtfs();
        return fswrap;
    } else
    if (name.compare("jfs") == 0) {
        QP_FSWrap *fswrap = new QP_FSJfs();
        return fswrap;
    } else
    if (name.compare("ext3") == 0) {
        QP_FSWrap *fswrap = new QP_FSExt3();
        return fswrap;
    } else
    if (name.compare("xfs") == 0) {
        QP_FSWrap *fswrap = new QP_FSXfs();
        return fswrap;
    }
    else
        return NULL;
}

QString QP_FSWrap::get_label(PedPartition *part, QString name) {
    if (name.compare("ntfs") == 0) {
        return QP_FSNtfs::_get_label(part);
    } else
    if (name.compare("jfs") == 0) {
        return QP_FSJfs::_get_label(part);
    } else
    if (name.compare("ext3") == 0) {
        return QP_FSExt3::_get_label(part);
    } else
    if (name.compare("xfs") == 0) {
        return QP_FSXfs::_get_label(part);
    }
    if (name.compare("fat16") == 0) {
        return QP_FSFat16::_get_label(part);
    }
    if (name.compare("fat32") == 0) {
        return QP_FSFat32::_get_label(part);
    }
    if (name.compare("ext2") == 0) {
        return QP_FSExt2::_get_label(part);
    }
    if (name.compare("reiserfs") == 0) {
        return QP_FSExt2::_get_label(part);
    }
    else
        return QString::null;
}

bool QP_FSWrap::read_sector(PedPartition *part, PedSector offset, PedSector count, char *buffer) {
    /*---open a new device, read a sector and close it---*/
    if (!ped_device_open(part->geom.dev))
        return false;

    if (!ped_geometry_read(&part->geom, buffer, offset, count)) {
        ped_device_close (part->geom.dev);
        return false;
    }
    
    if (!ped_device_close(part->geom.dev))
        return false;

    return true;
}

bool QP_FSWrap::qpMount(QString device) {
    char szcmdline[200];
    bool error = false;

    /*---just to be sure! :)---*/
    qpUMount(device);
    
    /*---mount the partition---*/
    sprintf(szcmdline, "%s %s", device.latin1(), TMP_MOUNTPOINT);
    QString cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(MOUNT))
            .arg(szcmdline);
    
    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }

    char *cline;
    while ((cline = fs_getline())) {
        QString line = QString(cline);

        QRegExp rx;
        rx = QRegExp("^mount: (.*)$");
        if (rx.search(line) == 0) {
            QString capError = rx.cap(1);
            _message = capError;
            error = true;
        }
    }
    fs_close();
    
    return !error;
}

bool QP_FSWrap::qpUMount(QString device) {
    char szcmdline[200];
    bool error = false;

    /*---umount the partition---*/
    sprintf(szcmdline, "%s", device.latin1());
    QString cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(UMOUNT))
            .arg(szcmdline);
    
    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }

    char *cline;
    while ((cline = fs_getline())) {
        QString line = QString(cline);

        QRegExp rx;
        rx = QRegExp("^mount: (.*)$");
        if (rx.search(line) == 0) {
            QString capError = rx.cap(1);
            _message = capError;
            error = true;
        }
    }
    fs_close();

    return !error;
}



/*---NTFS WRAPPER-----------------------------------------------------------------*/
QP_FSNtfs::QP_FSNtfs() {
    wrap_resize = false;
    wrap_move = false;
    wrap_copy = false;
    wrap_create = false;
    wrap_min_size = false;

    /*---check if the wrapper is installed---*/
    QString cmdline = QString("which %1")
                    .arg(lstExternalTools->getPath(MKNTFS));
    fs_open(cmdline);
    char *cline;
    while ((cline = fs_getline()))
        wrap_create = true;
    fs_close();

    /*---check if the wrapper is installed---*/
    cmdline = QString("which %1")
            .arg(lstExternalTools->getPath(NTFSRESIZE));
    fs_open(cmdline);


    while ((cline = fs_getline())) {
        wrap_resize = RS_SHRINK | RS_ENLARGE;
        wrap_min_size = true;
    }
    fs_close();
}

bool QP_FSNtfs::resize(QP_LibParted *_libparted, bool write, QP_PartInfo *partinfo, PedSector new_start, PedSector new_end) {
    /*---pointer to libparted---*/
    QP_LibParted *libparted = _libparted;
    
	showDebug("%s", "Resizing a filesystem using a wrapper\n");

    /*---the user want to shrink or enlarge the partition?---*/
    /*---the user want to shrink!---*/
    if (new_end < partinfo->end) {
        /*---update the filesystem---*/
	    showDebug("%s", "shrinking filesystem...\n");
        if (!ntfsresize(write, partinfo->partname(), new_end - new_start - 4*MEGABYTE_SECTORS)) {
	        showDebug("%s", "shrinking filesystem ko\n");
            return false;
        }

        /*---and now update geometry of the partition---*/
        showDebug("%s", "update geometry...\n");
        if (!libparted->set_geometry(partinfo, new_start, new_end)) {
            showDebug("%s", "update geometry ko\n");
            _message = libparted->message();
            return false;
        } else {
            /*---if you are NOT committing then add in the undo/commit list---*/
            if (!write) {
                showDebug("%s", "operation added to undo/commit list\n");
                PedPartitionType parttype = libparted->type2parttype(partinfo->type);
                PedGeometry geom = libparted->get_geometry(partinfo);
                libparted->actlist->ins_resize(partinfo->num, new_start, new_end, geom, parttype);
            }
            return true;
        }
    /*---the user want to enlarge!---*/
    } else {
        /*---cannot enlarge if we cannot change disk geometry!---*/
        if (partinfo->device()->isBusy()) {
            showDebug("%s", "the device is busy, so you cannot enlarge it\n");
            _message = tr("Cannot enlarge a partition if the disk device is busy");
            return false;
        }
        
        /*---first se the geometry of the partition---*/
        showDebug("%s", "update geometry...\n");
        if (!libparted->set_geometry(partinfo, new_start, new_end)) {
            showDebug("%s", "update geometry ko\n");
            _message = libparted->message();
            return false;
        } else {
            /*---if you are NOT committing then add in the undo/commit list---*/
            if (!write) {
                showDebug("%s", "operation added to undo/commit list\n");
                PedPartitionType parttype = libparted->type2parttype(partinfo->type);
                PedGeometry geom = libparted->get_geometry(partinfo);
                libparted->actlist->ins_resize(partinfo->num, new_start, new_end, geom, parttype);
            }
        }

        if (write) {
            /*---and now update the filesystem!---*/
	        showDebug("%s", "enlarge filesystem...\n");
            if (!ntfsresize(write, partinfo->partname(), new_end - new_start - 4*MEGABYTE_SECTORS)) {
	            showDebug("%s", "enlarge filesystem ko\n");
                return false;
            } else {
                return true;
            }
        } else {
            return true;
        }
    }
}

bool QP_FSNtfs::ntfsresize(bool write, QString dev, PedSector newsize) {
    char szcmdline[200];

    /*---init of the error message---*/
    _message = QString::null;

    /*---calculate size of the partition in bytes---*/
    PedSector size = (PedSector)((newsize-1)*512);

    /*---read-only test---*/
    sprintf(szcmdline, "-n -ff -s %lld %s", size, dev.latin1());
    QString cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(NTFSRESIZE))
            .arg(szcmdline);
    
    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }

    bool error = false;
    char *cline;
    while ((cline = fs_getline())) {
       QString line = QString(cline);

       QRegExp rx;

       if (!error) {
           rx = QRegExp("^ERROR.*: (.*)");
           if (rx.search(line) == 0) {
               QString captured = rx.cap(1);
               _message = QString(captured);
               error = true;
           }
       }

       if (!error) {
           rx = QRegExp("^The volume end is fragmented.*");
           if (rx.search(line) == 0) {
               _message = QString("The partition is fragmented.");
               error = true;
           }
       }

       /*---progress bar!---*/
       QString linesub = line;
#ifdef QT30COMPATIBILITY
       linesub.replace(QRegExp("\r"), " ");
#else
       linesub.replace(QChar('\r'), " ");
#endif
       //example: 34,72 percent completed
       rx = QRegExp("^.* (\\d*),(\\d*) percent completed.*$");
       if (rx.search(linesub) == 0) {
           QString capIntPercent = rx.cap(1);
           printf("letto: %s\n", capIntPercent.latin1());
           //QString capFloatPercent = rx.cap(2);
           
           bool rc;
           int iPerc = capIntPercent.toInt(&rc) - 1;
           if (iPerc < 0) iPerc = 0; //the percente completed run many times, but i don't want it reach 100%
           
           emit sigTimer(iPerc, QString(tr("Resizing in progress.")), QString::null);
       }

       //BETA: change could with might with ntfsresize 1.9
       //rx = QRegExp("^Now You could resize at \\d* bytes or (\\d*) .*");
       rx = QRegExp("^.*You ..... resize at \\d* bytes or (\\d*) .*");
       if (rx.search(line) == 0) {
           QString captured = rx.cap(1);
           _message = QString("The partition is fragmented. Try to defragment it, or resize to %1MB")
                       .arg(captured);
           error = true;
       }
    }
    fs_close();

    if (error) return false;

    /*---if the user want to run a readonly test just return true---*/
    if (!write) return true;


    /*---ok, the readonly test seems ok... now we resize it!---*/
    sprintf(szcmdline, "-ff -s %lld %s", size, dev.latin1());
    cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(NTFSRESIZE))
            .arg(szcmdline);
    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }

    bool success = false;
    while ((cline = fs_getline())) {
       QString line = QString(cline);
       QRegExp rx;
       
       /*---progress bar!---*/
       QString linesub = line;
#ifdef QT30COMPATIBILITY
       linesub.replace(QRegExp("\r"), " ");
#else
       linesub.replace(QChar('\r'), " ");
#endif
       //example: 34,72 percent completed
       rx = QRegExp("^.* (\\d*),(\\d*) percent completed.*$");
       if (rx.search(linesub) == 0) {
           QString capIntPercent = rx.cap(1);
           //QString capFloatPercent = rx.cap(2);
           
           bool rc;
           int iPerc = capIntPercent.toInt(&rc) - 1;
           if (iPerc < 0) iPerc = 0; //the percente completed run many times, but i don't want it reach 100%
           
           emit sigTimer(iPerc, QString(tr("Resizing in progress.")), QString::null);
       }

       //rx = QRegExp("^Successfully resized NTFS on device");
       rx = QRegExp("^.*[Ss]uccessfully.*");
       if (rx.search(line) == 0)
           success = true;
       rx = QRegExp("^Nothing to do: NTFS volume size is already OK.");
       if (rx.search(line) == 0)
           success = true;

       rx = QRegExp("^Syncing device.*");
       if (rx.search(line) == 0) {
           emit sigTimer(99, QString(tr("Syncing device.")), QString::null);
       }
           
       rx = QRegExp("^ERROR.*: (.*)");
       if (rx.search(line) == 0) {
           QString captured = rx.cap(1);
           _message = QString(captured);
       }
    }
    fs_close();

    if (success) return true;
    else {
        if (_message.isNull()) _message = QString("An error occured! :(");
        return false;
    }
}

bool QP_FSNtfs::mkpartfs(QString dev, QString label) {
    char szcmdline[200];
    QString cmdline;

    /*---init of the error message---*/
    _message = QString::null;

    /*---prepare the command line---*/
    if (label.isEmpty())
        sprintf(szcmdline, "-f -s 512 %s", dev.latin1());
    else
        sprintf(szcmdline, "-f -s 512 -L %s %s", label.latin1(), dev.latin1());
    cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(MKNTFS))
            .arg(szcmdline);

    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }
    

    bool success = false;
    char *cline;
    while ((cline = fs_getline())) {
       QString line = QString(cline);

       QRegExp rx;
       rx = QRegExp("^mkntfs completed successfully. Have a nice day.");
       if (rx.search(line) == 0)
           success = true;

       rx = QRegExp("^ERROR.*: (.*)");
       if (rx.search(line) == 0) {
           QString captured = rx.cap(1);
           _message = QString(captured);
           success = false;
       }
    }
    fs_close();

    return success;
}

PedSector QP_FSNtfs::min_size(QString dev) {
    char szcmdline[200];
    QString cmdline;
    PedSector size = -1;

    /*---init of the error message---*/
    _message = QString::null;

    /*---prepare the command line---*/
    sprintf(szcmdline, "-f -i %s", dev.latin1());
    cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(NTFSRESIZE))
            .arg(szcmdline);

    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }
    

    bool success = false;
    char *cline;
    while ((cline = fs_getline())) {
       QString line = QString(cline);

       QRegExp rx;
       rx = QRegExp("^.*You ..... resize at (\\d*) bytes or (\\d*) .*");
       if (rx.search(line) == 0) {
           QString captured = rx.cap(1);
           sscanf(captured.latin1(), "%lld", &size);
           size/=512;
           size+=8*MEGABYTE_SECTORS;

           success = true;
       }
    }
    fs_close();

    return size;
}

QString QP_FSNtfs::fsname() {
    return QString("ntfs");
}

QString QP_FSNtfs::_get_label(PedPartition *) {
    return QString::null;
}



/*---JFS WRAPPER-----------------------------------------------------------------*/
QP_FSJfs::QP_FSJfs() {
    wrap_resize = RS_ENLARGE;
    wrap_move = false;
    wrap_copy = false;
    wrap_create = false;
    wrap_min_size = false;

    /*---check if the wrapper is installed---*/
    QString cmdline = QString("which %1")
                    .arg(lstExternalTools->getPath(MKFS_JFS));
    fs_open(cmdline);

    char *cline;
    while ((cline = fs_getline()))
        wrap_create = true;
    fs_close();
    
}

bool QP_FSJfs::resize(QP_LibParted *_libparted, bool write, QP_PartInfo *partinfo, PedSector new_start, PedSector new_end) {
    /*---pointer to libparted---*/
    QP_LibParted *libparted = _libparted;
    
    showDebug("%s", "Resizing a filesystem using a wrapper\n");

    /*---the user want to shrink or enlarge the partition?---*/
    /*---the user want to shrink!---*/
    if (new_end < partinfo->end) {
        /*---update the filesystem---*/
        showDebug("%s", "shrinking filesystem not supported with jfs...\n");
        return false;
    /*---the user want to enlarge!---*/
    } else {
        /*---cannot enlarge if we cannot change disk geometry!---*/
        if (partinfo->device()->isBusy()) {
            showDebug("%s", "the device is busy, so you cannot enlarge it\n");
            _message = tr("Cannot enlarge a partition if the disk device is busy");
            return false;
        }
        
        /*---first se the geometry of the partition---*/
        showDebug("%s", "update geometry...\n");
        if (!libparted->set_geometry(partinfo, new_start, new_end)) {
            showDebug("%s", "update geometry ko\n");
            _message = libparted->message();
            return false;
        } else {
            /*---if you are NOT committing then add in the undo/commit list---*/
            if (!write) {
                showDebug("%s", "operation added to undo/commit list\n");
                PedPartitionType parttype = libparted->type2parttype(partinfo->type);
                PedGeometry geom = libparted->get_geometry(partinfo);
                libparted->actlist->ins_resize(partinfo->num, new_start, new_end, geom, parttype);
            }
        }

        if (write) {
            /*---and now update the filesystem!---*/
            showDebug("%s", "enlarge filesystem...\n");
            if (!jfsresize(write, partinfo, new_end - new_start)) {
                showDebug("%s", "enlarge filesystem ko\n");
                return false;
            } else {
                return true;
            }
        } else {
            return true;
        }
    }
}

bool QP_FSJfs::jfsresize(bool write, QP_PartInfo *partinfo, PedSector) {
    char szcmdline[200];
    QString cmdline;

    bool error = false;

    if (!write) return true;

    /*---init of the error message---*/
    _message = QString::null;

    if (!qpMount(partinfo->partname()))
        return false;


    /*---do the resize!---*/
    sprintf(szcmdline, "-o remount,resize= %s", TMP_MOUNTPOINT);
    cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(MOUNT))
            .arg(szcmdline);
    
    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }

    char *cline;
    while ((cline = fs_getline())) {
       QString line = QString(cline);

       QRegExp rx;
       rx = QRegExp("^mount: (.*)$");
       if (rx.search(line) == 0) {
           QString capError = rx.cap(1);
           _message = capError;
           error = true;
       }
    }
    fs_close();

    if (error) return false;
    
    if (!qpUMount(partinfo->partname()))
        return false;

    return true;
    /*fino a qui*/
}

bool QP_FSJfs::mkpartfs(QString dev, QString label) {
    char szcmdline[200];
    QString cmdline;

    /*---init of the error message---*/
    _message = QString::null;

    /*---prepare the command line---*/
    if (label.isEmpty())
        sprintf(szcmdline, "-q %s", dev.latin1());
    else
        sprintf(szcmdline, "-q -L %s %s", label.latin1(), dev.latin1());
    cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(MKFS_JFS))
            .arg(szcmdline);

    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }
    

    bool success = false;
    char *cline;
    while ((cline = fs_getline())) {
       QString line = QString(cline);

       QRegExp rx;
       rx = QRegExp("^Format completed successfully.");
       if (rx.search(line) == 0)
           success = true;
    }
    fs_close();

    return success;
}

QString QP_FSJfs::fsname() {
    return QString("jfs");
}

QString QP_FSJfs::_get_label(PedPartition *) {
    return QString::null;
}



/*---EXT3 WRAPPER----------------------------------------------------------------*/
QP_FSExt3::QP_FSExt3() {
    wrap_min_size = false;
    wrap_resize = false;
    wrap_move = false;
    wrap_copy = false;
    wrap_create = false;

    /*---check if the wrapper is installed---*/
    QString cmdline = QString("which %1")
                    .arg(lstExternalTools->getPath(MKFS_EXT3));
    fs_open(cmdline);

    char *cline;
    while ((cline = fs_getline()))
        wrap_create = true;
    fs_close();
    
}

bool QP_FSExt3::mkpartfs(QString dev, QString label) {
    char szcmdline[200];
    QString cmdline;

    /*---init of the error message---*/
    _message = QString::null;

    /*---prepare the command line---*/
    if (label.isEmpty())
        sprintf(szcmdline, "%s", dev.latin1());
    else
        sprintf(szcmdline, "-L %s %s", label.latin1(), dev.latin1());
    cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(MKFS_EXT3))
            .arg(szcmdline);

    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }
    

    bool writenode = false;
    bool success = false;
    char *cline;
    while ((cline = fs_getline())) {
       QString line = QString(cline);

       QRegExp rx;
       rx = QRegExp("^Writing inode tables");
       if (rx.search(line) == 0) {
           writenode = true;
       }

       rx = QRegExp("^Creating journal");
       if (rx.search(line) == 0) {
           writenode = false;
           emit sigTimer(90, QString(tr("Writing superblocks and filesystem.")), QString::null);
       }

       if (writenode) {
           QString linesub = line;

#ifdef QT30COMPATIBILITY
            linesub.replace(QRegExp("\b"), " ");
#else
            linesub.replace(QChar('\b'), " ");
#endif
           rx = QRegExp("^.* (\\d*)/(\\d*) .*$");
           if (rx.search(linesub) == 0) {
               QString capActual = rx.cap(1);
               QString capTotal = rx.cap(2);
               
               bool rc;
               int iActual = capActual.toInt(&rc);
               int iTotal = capTotal.toInt(&rc);

               int iPerc = iActual*80/iTotal; //The percentual is calculated in 80% ;)
               emit sigTimer(iPerc, QString(tr("Writing inode tables.")), QString::null);
           }
       }

       rx = QRegExp("^Writing superblocks and filesystem accounting information: done");
       if (rx.search(line) == 0)
           success = true;
    }
    fs_close();

    if (!success) _message = QString(tr("There was a problem with mkfs.ext3."));

    return success;
}

QString QP_FSExt3::fsname() {
    return QString("ext3");
}

QString QP_FSExt3::_get_label(PedPartition *) {
    return QString::null;
}

/*---XFS WRAPPER-----------------------------------------------------------------*/
QP_FSXfs::QP_FSXfs() {
    wrap_min_size = false;
    wrap_resize = false;
    wrap_move = false;
    wrap_copy = false;
    wrap_create = false;

    /*---check if the wrapper is installed---*/
    QString cmdline = QString("which %1")
                    .arg(lstExternalTools->getPath(MKFS_XFS));
    fs_open(cmdline);

    char *cline;
    while ((cline = fs_getline()))
        wrap_create = true;
    fs_close();
    

    /*---check if the wrapper is installed---*/
    cmdline = QString("which %1")
                    .arg(lstExternalTools->getPath(XFS_GROWFS));
    fs_open(cmdline);

    while ((cline = fs_getline()))
        wrap_resize = RS_ENLARGE;
    fs_close();
}

bool QP_FSXfs::mkpartfs(QString dev, QString label) {
    char szcmdline[200];
    QString cmdline;

    /*---init of the error message---*/
    _message = QString::null;

    /*---prepare the command line---*/
    if (label.isEmpty())
        sprintf(szcmdline, "-f %s", dev.latin1());
    else
        sprintf(szcmdline, "-f -L %s %s", label.latin1(), dev.latin1());
    cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(MKFS_XFS))
            .arg(szcmdline);

    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }    

    bool success = false;
    char *cline;
    while ((cline = fs_getline())) {
       QString line = QString(cline);

       QRegExp rx;
       rx = QRegExp("^realtime =.*");
       if (rx.search(line) == 0)
           success = true;
    }
    fs_close();

    if (!success) _message = QString(tr("There was a problem with mkfs.xfs."));
    return success;
}

bool QP_FSXfs::resize(QP_LibParted *_libparted, bool write, QP_PartInfo *partinfo, PedSector new_start, PedSector new_end) {
    /*---pointer to libparted---*/
    QP_LibParted *libparted = _libparted;
    
	showDebug("%s", "Resizing a filesystem using a wrapper\n");

    /*---the user want to shrink or enlarge the partition?---*/
    /*---the user want to shrink!---*/
    if (new_end < partinfo->end) {
        /*---update the filesystem---*/
	    showDebug("%s", "shrinking filesystem not supported with xfs...\n");
        return false;
    /*---the user want to enlarge!---*/
    } else {
        /*---cannot enlarge if we cannot change disk geometry!---*/
        if (partinfo->device()->isBusy()) {
            showDebug("%s", "the device is busy, so you cannot enlarge it\n");
            _message = tr("Cannot enlarge a partition if the disk device is busy");
            return false;
        }
        
        /*---first se the geometry of the partition---*/
        showDebug("%s", "update geometry...\n");
        if (!libparted->set_geometry(partinfo, new_start, new_end)) {
            showDebug("%s", "update geometry ko\n");
            _message = libparted->message();
            return false;
        } else {
            /*---if you are NOT committing then add in the undo/commit list---*/
            if (!write) {
                showDebug("%s", "operation added to undo/commit list\n");
                PedPartitionType parttype = libparted->type2parttype(partinfo->type);
                PedGeometry geom = libparted->get_geometry(partinfo);
                libparted->actlist->ins_resize(partinfo->num, new_start, new_end, geom, parttype);
            }
        }

        if (write) {
            /*---and now update the filesystem!---*/
	        showDebug("%s", "enlarge filesystem...\n");
            if (!xfsresize(write, partinfo, new_end - new_start)) {
	            showDebug("%s", "enlarge filesystem ko\n");
                return false;
            } else {
                return true;
            }
        } else {
            return true;
        }
    }
}

bool QP_FSXfs::xfsresize(bool write, QP_PartInfo *partinfo, PedSector) {
    char szcmdline[200];
    QString cmdline;

    bool error = false;

    if (!write) return true;

    /*---init of the error message---*/
    _message = QString::null;

    if (!qpMount(partinfo->partname()))
        return false;

    /*---do the resize!---*/
    sprintf(szcmdline, "%s", TMP_MOUNTPOINT);
    cmdline = QString("%1 %2")
            .arg(lstExternalTools->getPath(XFS_GROWFS))
            .arg(szcmdline);
    
    if (!fs_open(cmdline)) {
        _message = QString(NOTFOUND);
        return false;
    }

    error = true;
    char *cline;
    while ((cline = fs_getline())) {
       QString line = QString(cline);

       QRegExp rx;
       rx = QRegExp("^realtime =.*");
       if (rx.search(line) == 0) {
           error = false;
       }
    }
    fs_close();

    if (error) {
        _message = QString(tr("Error during xfs_grow."));
        return false;
    }
    
    if (!qpUMount(partinfo->partname()))
        return false;

    return true;
}

QString QP_FSXfs::fsname() {
    return QString("xfs");
}

QString QP_FSXfs::_get_label(PedPartition *) {
    return QString::null;
}


/*---FAT16 WRAPPER---------------------------------------------------------------*/
QString QP_FSFat16::_get_label(PedPartition *) {
    return QString::null;
}


/*---FAT32 WRAPPER---------------------------------------------------------------*/
QString QP_FSFat32::_get_label(PedPartition *part) 
{
    char buffer[PED_SECTOR_SIZE];
    
    if (!QP_FSWrap::read_sector(part, 0, 1, buffer)) 
    {
        return QString::null;
    } 
    else 
    {
        buffer[PED_SECTOR_SIZE-1] = 0;
        printf("returned buffer: %s\n", buffer);
        return QString::null;
    }
}


/*---EXT2 WRAPPER----------------------------------------------------------------*/
QString QP_FSExt2::_get_label(PedPartition *part) 
{
  char bootsect[512]; // sector number 2 (offset 1024)
  char label[12];

  if (!QP_FSWrap::read_sector(part, 2, 1, bootsect)) 
     return QString::null;
	
  memset(label, 0, sizeof(label));
  strncpy(label, bootsect+120, 11);

  return QString(label);
}


/*---REISERFS WRAPPER------------------------------------------------------------*/
QString QP_FSReiserFS::_get_label(PedPartition *) {
    return QString::null;
}
