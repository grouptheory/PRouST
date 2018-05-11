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


#include <stdio.h>
#include <string.h>

int main(int argc, char** argv){
  
  FILE* fin, *fout;
  char line[2048];
  char prewrap[255];
  char postwrap[255];
  char *pos;
  int len;
  char libname[2048];

  fin = fopen(argv[1],"r");
  fout = fopen(argv[2],"w");
  strcpy(prewrap,argv[3]);
  strcpy(postwrap,argv[4]);

  while (fgets(line,2047,fin)){
    if (pos=strstr(line,".o:")){
      strcpy(libname,prewrap);
      len=pos-line;
      strncat(libname,line,len);
      libname[len+strlen(prewrap)]=0;
      strcat(libname,".o");
      strcat(libname,postwrap);
      strcat(libname,":");
      fprintf(fout,"%s",libname);
      fprintf(fout,"%s",pos+3);
    }
    else fprintf(fout,"%s",line);
  }

  fclose(fin);
  fclose(fout);
  exit(0);
}
