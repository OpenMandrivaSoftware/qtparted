/***************************************************************************
                         runprog.cpp  -  description
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
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

#include "runprog.h"
#include "qp_options.h"

//------------------------------------------------
/*int doesFileExists2(char *szPath)
{
#ifdef HAVE_FSTAT64
  struct stat64 fStat;
#else
  struct stat fStat;
#endif
  int nFd;
  int nRes;

  errno = 0;
#ifdef HAVE_OPEN64
  nFd = open64(szPath, O_RDONLY | O_NOFOLLOW | O_LARGEFILE);
#else
  nFd = open(szPath, O_RDONLY | O_NOFOLLOW | O_LARGEFILE);
#endif
  //fStream = fopen(szPath, "rb");
  //if (!fStream) // error
  if (nFd == -1)
    {
      return (errno != ENOENT);
    }
  else // success
    {
#ifdef HAVE_FSTAT64
      nRes = fstat64(nFd, &fStat);
#else
      nRes = fstat(nFd, &fStat);
#endif

      if (nRes == -1)
	return true; // for large files
      //fclose(fStream);
      ::close(nFd);
      return (S_ISREG(fStat.st_mode));
    }
}

//------------------------------------------------
int locateProgram(char *szResultPath, int nPathLen, char *szBin)
{
   long i;
   char *szLocations[] = {"/sbin", "/usr/sbin", "/usr/local/sbin", "/bin", "/usr/bin", "/usr/local/bin", ""};
   char szTemp[2048];

   for (i=0; *(szLocations[i]); i++)
   {
      snprintf(szTemp, sizeof(szTemp), "%s/%s", szLocations[i], szBin);
      //pi_printf ("--> pgm=%s/%s\n", szLocations[i], szBin);
      if (doesFileExists2(szTemp))
      {
	 snprintf(szResultPath, nPathLen, "%s/%s", szLocations[i], szBin);
	 return 0; // okay
      }
   }

   return -1; // error
}
*/

//------------------------------------------------
int execProgram(char *szProgram, char *argv[], char *szOutput, int nMaxOutput)
{
   int nFd[2];
   int nRes;
   pid_t pid;

   // init
   memset(szOutput, 0, nMaxOutput);
   if (pipe(nFd) < 0)
      return -1;

   pid = fork();
   switch (pid)
   {
      case -1: // error
	 perror ("fork() in execProgram");
	 return -1;

      case 0: // child
 	 dup2(nFd[1], STDOUT_FILENO); // stdout --> parent process
 	 dup2(nFd[1], STDERR_FILENO); // stdout --> parent process
	 nRes = execve(szProgram, argv, NULL);
	 exit(0);
	 break;

      default: // parent
	 read(nFd[0], szOutput, nMaxOutput);
	 //pi_printf ("\n\n\n[[[[[[[%s]]]]]]]\n\n\n", szString);
	 waitpid(pid, NULL, 0);
	 sleep(1);
	 break;
   };

   return 0;
}

//------------------------------------------------
//---------------------- USAGE -------------------
//------------------------------------------------
/*
   nRes = locateProgram(szFullPath, sizeof(szFullPath), g_fs[nFs].szCmdMkfsName);
   if (nRes == -1)
      return CResult("Can't find program %s which is required <br>to format the partition.<br>"
		     "Please, install %s. <br>You can search on http://www.freshmeat.net.", 
		     g_fs[nFs].szCmdMkfsName, g_fs[nFs].szFsTools);

   if (szLabel && *szLabel) // not empty
      snprintf (szFullLabel, sizeof(szFullLabel), "%s", szLabel);
   else
      snprintf (szFullLabel, sizeof(szFullLabel), "none");

   argv[0] = g_fs[nFs].szCmdMkfsOpts;
   argv[1] = g_fs[nFs].szCmdMkfsLabelOpts;
   argv[2] = szFullLabel;
   argv[3] = g_fs[nFs].szCmdMkfsOpts;
   argv[4] = szDevice;
   argv[5] = NULL;

   // pi_printf debug
   pi_printf ("FORMATTING: path=[%s]\n", szFullPath);
   for (int j=0; j < 4; j++)
      pi_printf ("argv[%d]=%s\n", j, argv[j]);

   execProgram(szFullPath, argv, szOutput, sizeof(szOutput));
*/

