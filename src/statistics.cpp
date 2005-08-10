/***************************************************************************
                         statistics.cpp  -  description
                             -------------------
    begin                : Wed Sep  4 19:21:54 UTC 2002
    copyright            : (C) 2002 by Francois Dupoux
    email                : fdupoux@partimage.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/vfs.h>
#include <mntent.h>
#include <sys/mount.h>
#include <sys/param.h>  // MAXPATHLEN

#include <qmessagebox.h>
#include "statistics.h"
#include "qp_filesystem.h"
#include "qp_options.h"
#include "qp_common.h"

#define TMP_MOUNTPOINT "/tmp/mntqp"
#define KBYTE_SECTORS 2

//------------------------------------------------
PedSector space_stats(QP_PartInfo *partinfo) {
	unsigned long a;
	a = getFsUsedKiloBytes(partinfo);

	/*printf ("device(%s)=%lu KB used\n", partinfo->partname().latin1(), a);*/
    if (a == 0) return -1;
    else return a * KBYTE_SECTORS;
}

//------------------------------------------------
int my_mount(QP_PartInfo *partinfo, const char *szMountPoint) {
	// security: if already mounted
	umount(szMountPoint);

	int nRes;

    char type[255];

    if (((partinfo->fsspec->name().compare("fat32") == 0)
      || (partinfo->fsspec->name().compare("fat16") == 0))) {
        strcpy(type, "vfat");
    } else {
        strcpy(type, partinfo->fsspec->name().latin1());
    }
	nRes=mount(partinfo->partname().latin1(),
               szMountPoint, type, MS_NOATIME | MS_RDONLY, NULL);
	/*fprintf (stderr, "debug: try mount %s as %s --> %d\n", partinfo->partname().latin1(), 
            type, nRes);*/
	if (nRes == 0) // if success
		return 0; // success

	return -1; // failure
}

//------------------------------------------------
int isMounted(QP_PartInfo *partinfo, char *szMountPoint) {
    FILE *f;
    struct mntent *mnt;
    bool bMounted;
    char szBuf1 [MAXPATHLEN+1];
    char szBuf2 [MAXPATHLEN+1];

    // init
    bMounted = false;
    memset(szBuf1, 0, MAXPATHLEN);
    memset(szBuf2, 0, MAXPATHLEN);

    if ((f = setmntent (MOUNTED, "r")) == 0)
      return (-1);

    // loop: compare device with each mounted device
    while (((mnt = getmntent (f)) != 0) && (!bMounted)) {

        /*---try to see if the shortname match---*/
        if ((partinfo->shortname().compare(mnt->mnt_fsname) == 0)) {
            bMounted = true;
            strcpy(szMountPoint, mnt->mnt_dir);
        }
        
        /*---if there is devfs try to see if the longname match---*/
        if (isDevfsEnabled()) {
            if ((partinfo->longname().compare(mnt->mnt_fsname) == 0)) {
                bMounted = true;
                strcpy(szMountPoint, mnt->mnt_dir);
            }
        }
    }

    endmntent (f);

    return bMounted;
}

//------------------------------------------------
unsigned long getFsUsedKiloBytes(QP_PartInfo *partinfo) {
    struct statfs sfs;
    char szMountPoint[MAXPATHLEN+1];
    unsigned long long a, b, c;
    int bMounted;
    int bToBeUnmounted;
    int nRes;
    unsigned long lResult = 0; // to be returned

    // init
    memset(szMountPoint, 0, MAXPATHLEN+1);
    bToBeUnmounted = false;

    bMounted = isMounted(partinfo, szMountPoint);
    
    /*---if not mounted -> mount it---*/
    if (!bMounted) {
        mkdir (TMP_MOUNTPOINT, 755);
        memset(szMountPoint, 0, sizeof(szMountPoint)-1);
        snprintf(szMountPoint, sizeof(szMountPoint)-1, "%s", TMP_MOUNTPOINT);
        nRes = my_mount(partinfo, TMP_MOUNTPOINT);
        /*if (nRes != 0)
            fprintf (stderr, "debug: cannot mount device %s\n", partinfo->partname().latin1());*/
        bToBeUnmounted = (nRes == 0);
        bMounted =(nRes == 0);
    }

    // if the partition is mounted (naturally or not)
    if (bMounted) {
        if (statfs(szMountPoint, &sfs) != -1) {
            a = (sfs.f_blocks - sfs.f_bavail); // used blocks count
            b = a * sfs.f_bsize; // used bytes count
            c = b / 1024LL; // user KiloBytes count
            lResult = c;
        } /*else {
            fprintf (stderr, "debug: statfs error on device %s\n", partinfo->partname().latin1());
        }*/

        if (bToBeUnmounted) {
            nRes = umount(szMountPoint);
            if (nRes != 0) {
                QString label = QString(QObject::tr("Cannot umount partition device: %1."
                                       "Please do it by hand first to commit the changes!"))
                                .arg(partinfo->partname());
                QMessageBox::information(NULL, PROG_NAME, label);
            }
        }

        return lResult; // success
    } else {
      // error -> partition not mounted
      return 0L; // failure
    }
}
