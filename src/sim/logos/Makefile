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
OTHER_MODULES = fsm codec FW DS

#*** library sources and associated headers
LIBSRCS = Logos.cc atoms.cc BaseLogos.cc DynamicLogos.cc \
	LogosGraph.cc transit.cc LogosInterfaces.cc
#	Logos_tmpl.cc
HEADERS  = ${LIBSRCS:%.cc=%.h}

#*** other targets, e.g. linktst.{THIS_DIR}_${OS}
EXECS =
#EXECS = linktst_${OS}${LIBTYPE}
#EXECS = linktst_${OS}${LIBTYPE} rtest_${OS}${LIBTYPE} igtest_${OS}${LIBTYPE}

#*** files to be compiled for non-library targets
EXECSRCS = linktst.cc 

OTHERFILES = Makefile ${EXECSRCS}

include ../Make.local

## CCOPTS += -DDEBUG_LOGOS

#*** executables, and the library ${MODULELIB}
all:	library ${EXECS}
	@echo ${MODULE}/${THIS_DIR} is ready.

library: ${MODULELIB}
	@true

linktst: linktst_${OS}${LIBTYPE}
	@true

#*** PLEASE test your module linkage before you push it into src
linktst_${OS}${LIBTYPE}:   linktst.cc ${MODULELIB}
	$(C++) -D__${OS}__ $(CCFLAGS) ${EXECOPTS} ${INCLUDES} \
	  linktst.cc ${LINKS} -o linktst_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/linktst_${OS}${LIBTYPE} is ready.

# easy to type targets
rtest: rtest_${OS}${LIBTYPE}
	@echo rtest ok

igtest: igtest_${OS}${LIBTYPE}
	@true


# real target
rtest_${OS}${LIBTYPE}:	rtest.cc ${MODULELIB}
	$(C++) -D__${OS}__ $(CCFLAGS) ${EXECOPTS} ${INCLUDES} \
		rtest.cc ${LINKS} -o rtest_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/rtest_${OS}${LIBTYPE} is ready.

igtest_${OS}${LIBTYPE}:	igtest.cc ${MODULELIB}
	$(C++) -D__${OS}__ $(CCFLAGS) ${EXECOPTS} ${INCLUDES} \
		igtest.cc ${LINKS} -o igtest_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/igtest_${OS}${LIBTYPE} is ready.


