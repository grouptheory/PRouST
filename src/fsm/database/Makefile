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
OTHER_MODULES = codec FW DS

#*** library sources and associated headers
LIBSRCS = Database.cc DatabaseEvent.cc ExecutionerPTSE.cc \
	DatabaseInterfaces.cc
HEADERS  = ${LIBSRCS:%.cc=%.h}

#*** other targets, e.g. linktst.{THIS_DIR}_${OS}
EXECS = dbtest_${OS}${LIBTYPE}

#*** files to be compiled for non-library targets
EXECSRCS = dbtest.cc 

OTHERFILES = Makefile ${EXECSRCS}

include ../Make.local

CCOPTS += 
# -DDEBUG_DB

#*** executables, and the library ${MODULELIB}
all:	library
	@echo ${MODULE}/${THIS_DIR} is ready.

library: ${MODULELIB}
	@true

#*** PLEASE test your module linkage before you push it into src
dbtest: dbtest_${OS}${LIBTYPE}
	@echo ""

dbtest_${OS}${LIBTYPE}:	${EXECSRCS:.cc=.${OTYPE}} ${MODULELIB}
	$(C++) -D__${OS}__ $(CCFLAGS) ${EXECOPTS} ${INCLUDES} \
		${EXECSRCS:%.cc=%.${OTYPE}} ${LINKS} -o dbtest_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/dbtest_${OS}${LIBTYPE} is ready.

dbtest2: dbtest2.o
	$(C++) $(CCFLAGS) ${EXECOPTS} ${INCLUDES} dbtest2.o ${LINKS} -o dbtest2
	@echo ${MODULE}/${THIS_DIR}/dbtest2 is ready

uplinks: uplink.${OTYPE}
	$(C++) -D__${OS}__ $(CCFLAGS) ${EXECOPTS} ${INCLUDES} \
		uplink.${OTYPE} ${LINKS} -o uplink_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/uplink_${OS}${LIBTYPE} is ready.
