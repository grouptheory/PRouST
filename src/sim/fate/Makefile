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

# Makefile
# Version: $Id: Makefile,v 1.71 1999/02/26 18:25:16 talmage Exp $

#*** developer/module we are exposed to during development
OTHER_DEVS = 

LIBEXT = so.0.0

CWD := ${shell pwd}

#*** other libraries we need to link with
OTHER_MODULES = sim fsm codec FW DS

#*** library sources and associated headers
LIBSRCS =
HEADERS  = ${LIBSRCS:%.cc=%.h}

#*** other targets, e.g. linktst.${THIS_DIR}_${OS}
EXECS = FC_${OS}${LIBTYPE}

#*** files to be compiled for non-library targets
EXECSRCS = main.cc

OTHERFILES = Makefile ${EXECSRCS} syntax.yac syntax.lex examples compile.sh parse.sh libextract.awk examples/Scripts

LOCALSRCS = cronos.cc cronos_tmpl.cc

include ../Make.local

ifeq (${GRANDPARENT}, src)
	CRONOS_TARG = ${TOP}/lib/${OS}/libcronos_${OS}${LIBTYPE}.${LIBEXT}
	CRON_EXT =
else
	CRON_EXT = _${GRANDPARENT}
	CRONOS_TARG = ../libcronos_${OS}${LIBTYPE}${CRON_EXT}.$(LIBEXT)
	FAKE_LIB = fake-lib
	RLINKS := ${RLINKS}:${TOP}/dev/${GRANDPARENT}/${PARENT}
endif

_o.${OS}/%.${OTYPE}:  %.cc
	${CCC} -D__${OS}__ $(CCFLAGS) ${CCOPTS} ${EXECOPTS} ${INCLUDES} -c $< -o $@

_o.${OS}:
	@if [ ! -d _o.${OS} ]; then mkdir _o.${OS}; fi

#*** executables, and the library ${MODULELIB}
all:	library ${EXECS}
	@echo ${MODULE}/${THIS_DIR}/${EXECS} now ready.

library: _o.${OS} ${CRONOS_TARG}
	@true

#*** PLEASE test your module linkage before you push it into src
lex.yy.c:	syntax.lex
	$(LEX) $(LEX_ARGS) syntax.lex

syntax.yac.tab.c:	syntax.yac
	$(YACC) $(YACC_ARGS) syntax.yac


${CRONOS_TARG}:	_o.${OS}/cronos.${OTYPE} _o.${OS}/cronos_tmpl.${OTYPE}
	$(LD) $(LDSHOPTS) ${LDOPTS} _o.${OS}/cronos.${OTYPE} _o.${OS}/cronos_tmpl.${OTYPE} -o ${CRONOS_TARG}

fake-lib:
	@if [ -f ../libsim_${OS}_${DEVNAME}.so.0.0 \
	   -o -f ../libsim_${OS}_${DEVNAME}.a ]; then \
	  echo -n ""; \
	else \
	  ${AR} cr ../libsim_${OS}_${DEVNAME}.a ; \
	  ${RANLIB} ../libsim_${OS}_${DEVNAME}.a ; \
	fi

FC:	FC_${OS}${LIBTYPE}

FC_${OS}${LIBTYPE}: syntax.yac.tab.c lex.yy.c main.cc ${CRONOS_TARG} ${FAKE_LIB}
	${CCC} -D__${OS}__ ${CCOPTS} ${EXECOPTS} ${INCLUDES} \
	     -DYYDEBUG=1 -DYYERROR_VERBOSE ${EXECSRCS} \
	     ${LINKS} -lcronos_${OS}${LIBTYPE}${CRON_EXT} \
		-o FC_${OS}${LIBTYPE}
	@echo FC_${OS}${LIBTYPE} is now ready.

fate:	FC

depend::
	${DependSubDir}

clean::
	@rm -f *.o *.O *_${OS}* _o.${OS}/*.[oO] *.yac.tab.* \
	*.yac.output lex.yy.c

