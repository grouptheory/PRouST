# +++++++++++++++
#   P R o u S T     ---  PNNI Routing Simulation Toolkit  ---
# +++++++++++++++
# Version: 1.0 (Fri Mar  5 14:35:33 EST 1999)
# 
# 			  Copyright (c) 1998
# 		 Naval Research Laboratory (NRL/CCS)
# 			       and the
# 	  Defense Advanced Research Projects Agency (DARPA)
# 
# 			 All Rights Reserved.
# 
# Permission to use, copy, and modify this software and its
# documentation is hereby granted, provided that both the copyright notice and
# this permission notice appear in all copies of the software, derivative
# works or modified versions, and any portions thereof, and that both notices
# appear in supporting documentation.
# 
# NRL AND DARPA ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION AND
# DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM
# THE USE OF THIS SOFTWARE.
# 
# NRL and DARPA request users of this software to return modifications,
# improvements or extensions that they make to:
# 
#                 proust-dev@cmf.nrl.navy.mil
#                         -or-
#                Naval Research Laboratory, Code 5590
#                Center for Computation Science
#                Washington, D.C.  20375
# 
# and grant NRL and DARPA the rights to redistribute these changes in
# future upgrades.

# extracts library names and paths from a file with one
# record per line, generates -L and -l line
{
  libdir[ NR ] = $0;
  gsub( /\/[^/]*$/, "", libdir[ NR ] );
  where = match( libdir[ NR ], ".so" );
  # There was no path specified ...
  if ( where != 0 ) {
    libdir[ NR ] = "";
  }

  libname[ NR ] = $0;
  gsub( /^.*lib/, "-l", libname[ NR ] );
  gsub( /\..*$/, "", libname[ NR ] );
}
END {
  list = "";
  for ( i = 1; i <= NR; i++ ) {
    found = 0;
    for ( j = 1; j < i; j++) {
       if ( libdir[ i ] == libdir[ j ] )
         found = 1;
    }
    if ( found != 0 ) {
      # list = list ":" libdir[ i ];
      printf( "-L%s ", libdir[ i ] );
    }
  }
  # printf( "%s ", list );
  for ( i = 1; i <= NR; i++ )
    printf( "%s ", libname[ i ] );
}


