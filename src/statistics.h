/***************************************************************************
                          statistics.h  -  description
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

#include "qp_libparted.h"

int isMounted(QP_PartInfo *, char *szMountPoint);
unsigned long getFsUsedKiloBytes(QP_PartInfo *);
int my_mount(QP_PartInfo *, char *szMountPoint);
PedSector space_stats(QP_PartInfo *);

