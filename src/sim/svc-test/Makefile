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
LIBSRCS = 
HEADERS  = ${LIBSRCS:%.cc=%.h}

#*** other targets, e.g. linktst.${THIS_DIR}_${OS}
EXECS = svc-test_${OS}${LIBTYPE}

#*** files to be compiled for non-library targets
EXECSRCS = 

OTHERFILES = Makefile ${EXECSRCS} SwitchTimers.cc SwitchTimers.h

include ../Make.local

#*** executables, and the library ${MODULELIB}
all:	${MODULELIB}
	@echo ${MODULE}/${THIS_DIR} is ready.

library: ${MODULELIB}
	@true

execs: ${EXECS}
	@true

#*** PLEASE test your module linkage before you push it into src
SwitchTimers_${OS}.${OTYPE}: SwitchTimers.cc \
	SwitchTimers.h
	${CCC} -D__${OS}__ ${CCOPTS} ${INCLUDES} \
	-c SwitchTimers.cc -o SwitchTimers_${OS}.${OTYPE}

svc-test_${OS}${LIBTYPE}:	svc-test.cc \
	SwitchTimers_${OS}.${OTYPE} \
	${MODULELIB}
	$(C++) -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		svc-test.cc ${LINKS} \
		SwitchTimers_${OS}.${OTYPE} \
		-o svc-test_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/svc-test_${OS}${LIBTYPE} is ready.

svc-test:	svc-test_${OS}${LIBTYPE}
	@echo Done

2DoubleSWPG-Linear_${OS}${LIBTYPE}:	2DoubleSWPG-Linear.cc ${MODULELIB}
	${CCC} -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		2DoubleSWPG-Linear.cc ${LINKS} -o 2DoubleSWPG-Linear_${OS}${LIBTYPE}
	@echo "2DoubleSWPG-Linear_${OS}${LIBTYPE} is ready."

2DoubleSWPG-Linear:	2DoubleSWPG-Linear_${OS}${LIBTYPE}
	@true

4SingleSWPG-Linear_${OS}${LIBTYPE}:	4SingleSWPG-Linear.cc ${MODULELIB}
	${CCC} -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		4SingleSWPG-Linear.cc ${LINKS} -o 4SingleSWPG-Linear_${OS}${LIBTYPE}
	@echo "4SingleSWPG-Linear_${OS}${LIBTYPE} is ready."

4SingleSWPG-Linear:	4SingleSWPG-Linear_${OS}${LIBTYPE}
	@true

3SingleSWPG-Linear_${OS}${LIBTYPE}:	3SingleSWPG-Linear.cc ${MODULELIB}
	${CCC} -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		3SingleSWPG-Linear.cc ${LINKS} -o 3SingleSWPG-Linear_${OS}${LIBTYPE}
	@echo "3SingleSWPG-Linear_${OS}${LIBTYPE} is ready."

3SingleSWPG-Linear:	3SingleSWPG-Linear_${OS}${LIBTYPE}
	@true

3SingleSWPG-Triangle_${OS}${LIBTYPE}:	3SingleSWPG-Triangle.cc ${MODULELIB}
	${CCC} -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		3SingleSWPG-Triangle.cc ${LINKS} -o 3SingleSWPG-Triangle_${OS}${LIBTYPE}
	@echo "3SingleSWPG-Triangle_${OS}${LIBTYPE} is ready."

3SingleSWPG-Triangle:	3SingleSWPG-Triangle_${OS}${LIBTYPE}
	@true

3TripleSWPG-Triangle_${OS}${LIBTYPE}:	3TripleSWPG-Triangle.cc ${MODULELIB}
	${CCC} -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		3TripleSWPG-Triangle.cc ${LINKS} -o 3TripleSWPG-Triangle_${OS}${LIBTYPE}
	@echo "3TripleSWPG-Triangle_${OS}${LIBTYPE} is ready."

3TripleSWPG-Triangle:	3TripleSWPG-Triangle_${OS}${LIBTYPE}
	@true

4TripleSWPG-Linear_${OS}${LIBTYPE}:	4TripleSWPG-Linear.cc ${MODULELIB}
	${CCC} -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		4TripleSWPG-Linear.cc ${LINKS} -o 4TripleSWPG-Linear_${OS}${LIBTYPE}
	@echo "4TripleSWPG-Linear_${OS}${LIBTYPE} is ready."

4TripleSWPG-Linear:	4TripleSWPG-Linear_${OS}${LIBTYPE}
	@true

3TripleSWPG-Linear_${OS}${LIBTYPE}:	3TripleSWPG-Linear.cc ${MODULELIB}
	${CCC} -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		3TripleSWPG-Linear.cc ${LINKS} -o 3TripleSWPG-Linear_${OS}${LIBTYPE}
	@echo "3TripleSWPG-Linear_${OS}${LIBTYPE} is ready."

3TripleSWPG-Linear:	3TripleSWPG-Linear_${OS}${LIBTYPE}
	@true

