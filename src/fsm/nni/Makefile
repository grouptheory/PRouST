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
LIBSRCS = CREFAccessor.cc Q93bCreator.cc \
	Translator.cc NNIExpander.cc \
	Q93bCall.cc Q93bState.cc Q93bTimers.cc\
	Q93bVisitors.cc Q93bParty.cc \
	SSCFVisitor.cc Coordinator.cc 

HEADERS  = ${LIBSRCS:%.cc=%.h}

#*** other targets, e.g. linktst.${THIS_DIR}_${OS}
EXECS = nnitest_${OS}${LIBTYPE} linktst_${OS}${LIBTYPE}

#*** files to be compiled for non-library targets
EXECSRCS = nni-main.cc NNITester.cc

OTHERFILES = Makefile ${EXECSRCS}

include ../Make.local

CCOPTS +=

# -D_BILAL_TESTING_

#*** executables, and the library ${MODULELIB}
all:	library
	@echo ${MODULE}/${THIS_DIR} is ready.

library:	${MODULELIB}
	@true

other:	${EXECS}

#*** PLEASE test your module linkage before you push it into src
nnitest_${OS}${LIBTYPE}:	${EXECSRCS:.cc=.o} ${MODULELIB}
	$(C++) -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		${EXECSRCS} ${LINKS} -o nnitest_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/nnitest_${OS}${LIBTYPE} is ready.

linktst_${OS}${LIBTYPE}: linktst.cc NNITester.cc ${MODULELIB}
	$(C++) -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		linktst.cc NNITester.cc ${LINKS} -o linktst_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/linktst_${OS}${LIBTYPE} is ready.
