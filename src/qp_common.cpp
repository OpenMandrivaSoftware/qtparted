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

#include <sys/param.h>  // MAXPATHLEN
#include <sys/stat.h>   // S_ISLNK
#include <unistd.h>     // readlink
#include <dirent.h>
#include <stdio.h>

#include "qp_common.h"
#include "qp_options.h"

bool flagDevfsEnabled;

QP_ListExternalTools *lstExternalTools = new QP_ListExternalTools();

/*---this function test if the kernel support devfs.
 *   the code was bring from partimage software made by Fran�ois Dupoux---*/
int isDevfsEnabled() {
    FILE *fPart;
    char cBuffer[32768];
    char *cPtr;
    int nSize;

    fPart = fopen("/proc/partitions", "rb");
    if (!fPart) {
        return -1;
    }

    nSize = 0;
    memset(cBuffer, 0, 32767);
    while (!feof(fPart)) {
        cBuffer[nSize] = fgetc(fPart);
        nSize++;
    }

    cPtr = cBuffer;

    fclose(fPart);

    if (strstr(cBuffer, "part"))
        return 1; // devfs is enabled
    else
        return 0; // devfs is disabled
}
