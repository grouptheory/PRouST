/*
 * +++++++++++++++
 *   P R o u S T     ---  PNNI Routing Simulation Toolkit  ---
 * +++++++++++++++
 * Version: 1.0 (Fri Mar  5 14:35:33 EST 1999)
 * 
 * 			  Copyright (c) 1998
 * 		 Naval Research Laboratory (NRL/CCS)
 * 			       and the
 * 	  Defense Advanced Research Projects Agency (DARPA)
 * 
 * 			 All Rights Reserved.
 * 
 * Permission to use, copy, and modify this software and its
 * documentation is hereby granted, provided that both the copyright notice and
 * this permission notice appear in all copies of the software, derivative
 * works or modified versions, and any portions thereof, and that both notices
 * appear in supporting documentation.
 * 
 * NRL AND DARPA ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION AND
 * DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM
 * THE USE OF THIS SOFTWARE.
 * 
 * NRL and DARPA request users of this software to return modifications,
 * improvements or extensions that they make to:
 * 
 *                 proust-dev@cmf.nrl.navy.mil
 *                         -or-
 *                Naval Research Laboratory, Code 5590
 *                Center for Computation Science
 *                Washington, D.C.  20375
 * 
 * and grant NRL and DARPA the rights to redistribute these changes in
 * future upgrades.
 */

/*
 * read output from nm and converts mangled C++ named to readable format
 *   cd src/common
 *   set OS=`./get_OS.sh`
 *   gcc demangle.c -liberty -o demangle_$OS
 *   nm file.o | demangle_$OS
 */
#include <stdio.h>

#ifdef __IRIX64__
#include <dem.h>

#endif
/* from libiberty/cplus_dem.c */

char *cplus_demangle(char *mangle, int options);
int DMGL_OPTS = 3; /*  DMGL_PARAMS | DMGL_ANSI */
char *demangled=0;
char CCmangled[MAXDBUF];


main()
{
  char line[2048];
  
/* output from SunOS nm will look like following 
 *
00000500 t _size__Ct4list1Zi
         U _std_memory_mgr

0123456789ab...
 *        print the first 10 characters as is 
 * then demange the reset of the line, if the demadle fails
 * print as is
 */
  while (gets(line)) {
#ifdef __IRIX64__
    /* might be compiled with CC or g++ */
#if __Cc__
     demangled = CCmangled;
     demangle(&line[0], demangled); /* check for CC */
     if (strcmp(line,demangled) == 0 )
#endif
       demangled=cplus_demangle(&line[0], DMGL_OPTS);
    printf("%s\n",demangled);
#else
    /* this is made to work with output of SunOS 4 nm */
    line[10]=0;
    printf("%s ",line);
    demangled=cplus_demangle(&line[11], DMGL_OPTS);
    if (demangled) {
      printf("%s\n",demangled);
      free(demangled);
      demangled=0;
    } else {
      printf("%s\n", &line[11]);
    }
#endif
  }

}

