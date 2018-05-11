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

#
# this file is similar to common/Make.module but doesnot and cannot
# be a link to it because of the REAL_RELINK section of that file
#
include Make.local

all:	library execs

library:	
	@ MAKE=${MAKE} MFLAGS=${MFLAGS} OPT="${OPT}" \
	  ${TOP}/src/common/forall-nonexempt-subdirs library library

clean::	checkauth checkowner
	@MAKE=${MAKE} ${COMMON}/forall-nonexempt-subdirs clean clean
	rm -f ${MODULELIB}

distclean::
	@MAKE=${MAKE} ${COMMON}/forall-nonexempt-subdirs clean distclean
	rm -rf */_o.*
	rm -rf ../lib/*
	rm -f common/dep_*
	@O=`find . -name "*.o" -print`; if [ "x$$O" != "x" ]; then \
		echo $$O | xargs rm -f; \
	fi
	@O=`find . -name "_o.*" -print`; if [ "x$$O" != "x" ]; then \
		echo $$O | xargs rm -rf; \
	fi

depend::	
	@MAKE=${MAKE} ${COMMON}/forall-nonexempt-subdirs depend depend

execs:	
	@MAKE=${MAKE} OPT="${OPT}" ${COMMON}/forall-nonexempt-subdirs execs execs

