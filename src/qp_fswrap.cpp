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

#include "qp_fswrap.h"
#include "qp_actlist.h"
#include "qp_common.h"
#include "qp_options.h"
#include "qp_debug.h"

#define NOTFOUND tr("command not found")
#define TMP_MOUNTPOINT "/tmp/mntqp"

bool QP_FSWrap::fs_open(QString cmdline, const char *arg, ...) {
    /*---this force stderr to stdout---*/
    int iTot = 0;
    char *t = NULL;
    va_list ap;
    va_start(ap, arg);
    iTot++; //cmdline
    while ((t = (char *)va_arg(ap, const char *))) {
       iTot++;
    }
    iTot++; //NULL
    va_end(ap);

    char **argomenti = new char*[iTot];
    int i = 0;
    argomenti[i++] = (char *)cmdline.latin1();
    va_start(ap, arg);
    argomenti[i++] = (char *)arg;
    while ((t = (char *)va_arg(ap, const char *))) {
        argomenti[i++] = (char *)t;
    }
    argomenti[i] = NULL;
    va_end(ap);

    pid_t pid;
    int help; 

    pipe(pipe1);
    pipe(pipe2);

    pid = fork();

    if (pid == 0) {                      /* child                     */
      close(pipe1[1]);                   /* close write end of pipe 1 */
      close(pipe2[0]);                   /* close read end of pipe 2  */

      dup2(pipe1[0],STDIN_FILENO);       /* pipe1 reading */
      dup2(pipe2[1],STDOUT_FILENO);      /* pipe2 writing */
      dup2(pipe2[1],STDERR_FILENO);      /* pipe2 writing */

      help = execv(argomenti[0], argomenti);
    /*  popen("ftp\n","w");             */
      perror("ftp");
      //printf("\n\n%d\n\n",help);      
      //TODO: test on "help"
    }

    if (pid > 0) {                /* parent  */
      close(pipe1[0]);
      close(pipe2[1]);

      fp_write = fdopen(pipe1[1], "w");
      if (!fp_write) {
          //delete argomenti;
          return false;
      }

      fp_read = fdopen(pipe2[0], "r");
      if (!fp_read) {
          //delete argomenti;
          return false;
      }

    /*    for ( ;; ) {
       printf("\nGive me a Command: ");
       scanf("%s", command);
       printf("priam\n");
       i = 0;
       while (fgets(line, sizeof line, fp)) {
           printf("linea: %s\n", line);
           i++;
           if (i == 3) {
              fprintf(fp2, "%s\n", command);
              fflush(fp2);
           }
          }
      }*/
    }

    //delete argomenti;

    return true;
}

char * QP_FSWrap::fs_getline() {
    bool rc = fgets(line, sizeof line, fp_read);

    if (rc) return line;
    else    return NULL;
}

int QP_FSWrap::fs_close() {
    int rc1 = fclose(fp_read);
    int rc2 = fclose(fp_write);

    if (rc1 || rc2) return true;
    else return false;
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

bool QP_FSWrap::qpMount(QString device) {
    bool error = false;

    /*---just to be sure! :)---*/
    printf("chiama umount\n");
    qpUMount(device);
    printf("uscito da umount\n");
    
    /*---mount the partition---*/
    printf("desso fa\n");
    if (!fs_open(lstExternalTools->getPath(MOUNT), device.latin1(), TMP_MOUNTPOINT, NULL)) {
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
    bool error = false;

    /*---umount the partition---*/
    printf("prima primax\n");
    if (!fs_open(lstExternalTools->getPath(UMOUNT), device.latin1(), NULL)) {
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

    /*---check if the wrapper is installed---*/
    fs_open("which", lstExternalTools->getPath(MKNTFS), NULL);

    char *cline;
    while ((cline = fs_getline()))
        wrap_create = true;
    fs_close();

    /*---check if the wrapper is installed---*/
    fs_open("which", lstExternalTools->getPath(NTFSRESIZE), NULL);

    while ((cline = fs_getline()))
        wrap_resize = RS_SHRINK | RS_ENLARGE;

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
        if (!ntfsresize(write, partinfo->partname(), new_end - new_start)) {
	        showDebug("%s", "shrinking filesystem ko\n");
            return false;
        }

        /*---and now update geometry of the partition---*/
        showDebug("%s", "update geometry...\n");
        if (!libparted->set_geometry(partinfo, new_start, new_end + 4*MEGABYTE_SECTORS)) {
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
        if (!libparted->set_geometry(partinfo, new_start, new_end + 4*MEGABYTE_SECTORS)) {
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
            if (!ntfsresize(write, partinfo->partname(), new_end - new_start)) {
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
    /*---init of the error message---*/
    _message = QString::null;

    /*---calculate size of the partition in bytes---*/
    PedSector size = (PedSector)((newsize-1)*512);

    /*---read-only test---*/
    char sSize[200];
    sprintf(sSize, "%lld", size);
    if (!fs_open(lstExternalTools->getPath(NTFSRESIZE), "-n", "-ff", "-s", sSize, dev.latin1(), NULL)) {
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

        rx = QRegExp("^Now You could resize at \\d* bytes or (\\d*) .*");
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
    if (!fs_open(lstExternalTools->getPath(NTFSRESIZE), "-ff", "-s", sSize, dev.latin1(), NULL)) {
        _message = QString(NOTFOUND);
        return false;
    }

    bool success = false;
    while ((cline = fs_getline())) {
        QString line = QString(cline);

        QRegExp rx;
        rx = QRegExp("^Successfully resized NTFS on device");
        if (rx.search(line) == 0)
            success = true;
        rx = QRegExp("^Nothing to do: NTFS volume size is already OK.");
        if (rx.search(line) == 0)
            success = true;
        rx = QRegExp("^ERROR.*: (.*)");
        if (rx.search(line) == 0) {
            QString captured = rx.cap(1);
            _message = QString(captured);
            printf("messsss: %s\n", _message.latin1());
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
    /*---init of the error message---*/
    _message = QString::null;

    /*---prepare the command line---*/
    if (label.isEmpty()) {
        if (!fs_open(lstExternalTools->getPath(MKNTFS), "-f", "-s", "512", dev.latin1(), NULL)) {
            _message = QString(NOTFOUND);
            return false;
        }
    }
    else {
        char sLabel[200];
        sprintf(sLabel, "%s", label.latin1());
        if (!fs_open(lstExternalTools->getPath(MKNTFS), "-f", "-s", "512", "-L", sLabel, dev.latin1(), NULL)) {
            _message = QString(NOTFOUND);
            return false;
        }
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

QString QP_FSNtfs::fsname() {
    return QString("ntfs");
}



/*---JFS WRAPPER-----------------------------------------------------------------*/
QP_FSJfs::QP_FSJfs() {
    wrap_resize = RS_ENLARGE;
    wrap_move = false;
    wrap_copy = false;
    wrap_create = false;

    /*---check if the wrapper is installed---*/
    fs_open("which", lstExternalTools->getPath(MKFS_JFS).latin1(), NULL);

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
    bool error = false;

    if (!write) return true;

    /*---init of the error message---*/
    _message = QString::null;

    printf("prima prima\n");
    if (!qpMount(partinfo->partname()))
        return false;

    /*---do the resize!---*/
    printf("prima\n");
    if (!fs_open(lstExternalTools->getPath(MOUNT).latin1(), "-o", "remount,resize=", TMP_MOUNTPOINT, NULL)) {
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
}

bool QP_FSJfs::mkpartfs(QString dev, QString label) {
    /*---init of the error message---*/
    _message = QString::null;

    /*---prepare the command line---*/
    if (label.isEmpty()) {
        if (!fs_open(lstExternalTools->getPath(MKFS_JFS).latin1(), "-q", dev.latin1(), NULL)) {
            _message = QString(NOTFOUND);
            return false;
        }
    } else {
        char sLabel[200];
        sprintf(sLabel, "%s", label.latin1());
        if (!fs_open(lstExternalTools->getPath(MKFS_JFS).latin1(), "-q", "-L", sLabel, dev.latin1(), NULL)) {
            _message = QString(NOTFOUND);
            return false;
        }
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



/*---EXT3 WRAPPER----------------------------------------------------------------*/
QP_FSExt3::QP_FSExt3() {
    wrap_resize = false;
    wrap_move = false;
    wrap_copy = false;
    wrap_create = false;

    /*---check if the wrapper is installed---*/
    fs_open("which", lstExternalTools->getPath(MKFS_EXT3).latin1(), NULL);

    char *cline;
    while ((cline = fs_getline()))
        wrap_create = true;
    fs_close();
}

bool QP_FSExt3::mkpartfs(QString dev, QString label) {
    /*---init of the error message---*/
    _message = QString::null;

    /*---prepare the command line---*/
    if (label.isEmpty()) {
        if (!fs_open(lstExternalTools->getPath(MKFS_EXT3), dev.latin1(), NULL)) {
            _message = QString(NOTFOUND);
            return false;
        }
    } else {
        char sLabel[200];
        sprintf(sLabel, "%s", label.latin1());
        if (!fs_open(lstExternalTools->getPath(MKFS_EXT3), "-L", sLabel, dev.latin1(), NULL)) {
            _message = QString(NOTFOUND);
            return false;
        }
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

    return success;
}

QString QP_FSExt3::fsname() {
    return QString("ext3");
}

/*---XFS WRAPPER-----------------------------------------------------------------*/
QP_FSXfs::QP_FSXfs() {
    wrap_resize = false;
    wrap_move = false;
    wrap_copy = false;
    wrap_create = false;

    /*---check if the wrapper is installed---*/
    fs_open("which", lstExternalTools->getPath(MKFS_XFS).latin1(), NULL);

    char *cline;
    while ((cline = fs_getline()))
        wrap_create = true;
    fs_close();
}

bool QP_FSXfs::mkpartfs(QString dev, QString label) {
    /*---init of the error message---*/
    _message = QString::null;

    /*---prepare the command line---*/
    if (label.isEmpty()) {
        if (!fs_open(lstExternalTools->getPath(MKFS_XFS).latin1(), "-q", "-f", dev.latin1(), NULL)) {
            _message = QString(NOTFOUND);
            return false;
        }
    } else {
        char sLabel[200];
        sprintf(sLabel, "%s", label.latin1());
        if (!fs_open(lstExternalTools->getPath(MKFS_XFS).latin1(), "-q", "-f", "-L", sLabel, dev.latin1(), NULL)) {
            _message = QString(NOTFOUND);
            return false;
        }
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

QString QP_FSXfs::fsname() {
    return QString("xfs");
}
