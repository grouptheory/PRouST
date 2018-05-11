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
OTHER_MODULES = 

#*** library sources and associated headers
LIBSRCS = 
HEADERS  = ${LIBSRCS:%.cc=%.h}

LOCALSRCS = GSynchronyInvestigator.cc TrafficInvestigator.cc \
	PGLInvestigator.cc ExperimentalInvestigator.cc  \
	ArrivalRateInvestigator.cc

LOCALOBJS = ${addprefix _o.${OS}/, ${LOCALSRCS:%.cc=%.${OTYPE}}}

#*** other targets, e.g. linktst.${THIS_DIR}_${OS}
EXECS = linktst.${THIS_DIR}_${OS}${LIBTYPE}

#*** files to be compiled for non-library targets
EXECSRCS = linktst.cc 

OTHERFILES = Makefile ${EXECSRCS}

include ../Make.local

ifeq (${GRANDPARENT}, src)
	LOCAL_LIB_EXT =
	LOCAL_LIB_TARG = ${TOP}/lib/${OS}/libInvestigators_${OS}${LIBTYPE}.${LIBEXT}
else
	LOCAL_LIB_EXT = _${GRANDPARENT}
	LOCAL_LIB_TARG = ../libInvestigators_${OS}${LIBTYPE}${CRON_EXT}.$(LIBEXT)
	RLINKS := ${RLINKS}:${TOP}/dev/${GRANDPARENT}/${PARENT}
endif

_o.${OS}/%.${OTYPE}:  %.cc
	${CCC} -D__${OS}__ $(CCFLAGS) ${CCOPTS} ${EXECOPTS} ${INCLUDES} -c $< -o $@

_o.${OS}:
	@if [ ! -d _o.${OS} ]; then mkdir _o.${OS}; fi

#*** executables, and the library ${MODULELIB}
all:	${MODULELIB}
	@echo ${MODULE}/${THIS_DIR} is ready.

library: ${MODULELIB} ${LOCAL_LIB_TARG}
	@true

${LOCAL_LIB_TARG}:	_o.${OS} ${LOCALOBJS}
	$(LD) $(LDSHOPTS) ${LDOPTS} ${LOCALOBJS} -o ${LOCAL_LIB_TARG}

clean::
	@rm -f *.o *.O *_${OS}* _o.${OS}/*.[oO] ../libInvestigators_*

#*** PLEASE test your module linkage before you push it into src
linktst.${THIS_DIR}_${OS}${LIBTYPE}:	${EXECSRCS:%.cc=%.${OTYPE}} ${MODULELIB}
	$(CCC) -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		${EXECSRCS:%.cc=%.${OTYPE}} ${LINKS} -o linktst.${THIS_DIR}_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/linktst.${THIS_DIR}_${OS}${LIBTYPE} is ready.
