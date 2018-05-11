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

# -*- Makefile -*-
#*** developer/module we are exposed to during development
OTHER_DEVS =

#*** other libraries we need to link with
OTHER_MODULES = fsm codec FW DS

#*** library sources and associated headers
LIBSRCS = 
HEADERS  = ${LIBSRCS:%.cc=%.h}

#*** other targets, e.g. linktst.${THIS_DIR}_${OS}
EXECS = chain 3by3grid 2by2grid

#*** files to be compiled for non-library targets
EXECSRCS = 

OTHERFILES = Makefile ${EXECSRCS}

include ../Make.local

#*** executables, and the library ${MODULELIB}
all:	${MODULELIB} ${EXECS}
	@echo ${MODULE}/${THIS_DIR} is ready.

library: ${MODULELIB}
	@true

#*** PLEASE test your module linkage before you push it into src
chain_${OS}${LIBTYPE}:	chain.cc ${MODULELIB}
	$(CCC) -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		chain.cc ${LINKS} -o chain_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/chain_${OS}${LIBTYPE} is ready.

chain:	chain_${OS}${LIBTYPE}
	@true

3by3grid_${OS}${LIBTYPE}:	3by3grid.cc ${MODULELIB}
	$(CCC) -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		3by3grid.cc ${LINKS} -o 3by3grid_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/3by3grid_${OS}${LIBTYPE} is ready.

3by3grid:	3by3grid_${OS}${LIBTYPE}
	@true

2by2grid_${OS}${LIBTYPE}:	2by2grid.cc ${MODULELIB}
	$(CCC) -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		2by2grid.cc ${LINKS} -o 2by2grid_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/2by2grid_${OS}${LIBTYPE} is ready.

2by2grid:	2by2grid_${OS}${LIBTYPE}
	@true

3linear_${OS}${LIBTYPE}:	3linear.cc ${MODULELIB}
	$(CCC) -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		3linear.cc ${LINKS} -o 3linear_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/3linear_${OS}${LIBTYPE} is ready.

3linear:	3linear_${OS}${LIBTYPE}
	@true


linktst.${THIS_DIR}_${OS}${LIBTYPE}:	${EXECSRCS:%.cc=%.${OTYPE}} ${MODULELIB}
	$(CCC) -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		${EXECSRCS:%.cc=%.${OTYPE}} ${LINKS} -o linktst.${THIS_DIR}_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/linktst.${THIS_DIR}_${OS}${LIBTYPE} is ready.
