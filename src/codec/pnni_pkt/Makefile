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

#*** developer/module we are exposed to during development
OTHER_DEVS =

#*** other libraries we need to link with
OTHER_MODULES = DS FW

#*** library sources and associated headers
LIBSRCS = database_sum.cc ptse_ack.cc ptsp.cc \
	hello.cc pnni_pkt.cc ptse_req.cc

HEADERS  = ${LIBSRCS:%.cc=%.h}

#*** other targets, e.g. linktst.{THIS_DIR}_${OS}
EXECS = linktst_${OS}${LIBTYPE}

#*** files to be compiled for non-library targets
EXECSRCS = linktst.cc 

OTHERFILES = Makefile ${EXECSRCS}

include ../Make.local

#*** executables, and the library ${MODULELIB}
all:	library
	@echo ${MODULE}/${THIS_DIR} is ready.

library:	${MODULELIB}
	@true

#*** PLEASE test your module linkage before you push it into src
linktst_${OS}${LIBTYPE}:	${EXECSRCS:%.cc=%.${OTYPE}} ${MODULELIB}
	$(C++) -D__${OS}__ $(CCFLAGS) ${EXECOPTS} ${INCLUDES} \
		${EXECSRCS:%.cc=%.${OTYPE}} ${LINKS} -o linktst_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/linktst_${OS}${LIBTYPE} is ready.








