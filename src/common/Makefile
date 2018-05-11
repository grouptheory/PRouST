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

OS:=${shell ./get_OS.sh}

IRIX = ${findstring IRIX, ${OS}}
ifeq (${IRIX},IRIX)
  CC=cc
else
  CC=gcc
endif

library all: LIBDIR dep_${OS} ident_report_${OS} demangle_${OS}
	@true

dep_${OS}: DEP/dep.c
	cd DEP; ${CC} dep.c -o ../dep_${OS}

ident_report_${OS}:
	@if [ -d dev ]; then \
	  cd dev; \
	  ${CC} ident_report.c -o ../ident_report_${OS}; \
	fi

demangle_${OS}: demangle.c
	@OS=`./get_OS.sh`; echo $$OS; \
	if [ $$OS = "SunOS" ]; then \
	   ${CC} demangle.c -o demangle_${OS} -liberty || true; \
	elif [ $$OS = "IRIX64" ]; then \
	   ${CC} demangle.c -o demangle_${OS} -liberty -D__IRIX64__ \
		|| true; \
	fi

LIBDIR: ../../lib/${OS}
	@true

../../lib/${OS}: ../../lib
	@mkdir ../../lib/${OS}

../../lib:
	@mkdir ../../lib/

depend:
	@echo done

clean:
	@rm -f dep_${OS} ident_report_${OS} demangle_${OS} core *~
