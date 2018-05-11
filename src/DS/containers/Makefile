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

#*** the .cc files that will go into the library
#*** and the .h files associated with the library
LIBSRCS = list.cc dictionary.cc h_array.cc p_queue.cc \
	sortseq.cc graph.cc queue.cc set.cc stack.cc \
	rb_tree.cc
HEADERS = ${LIBSRCS:%.cc=%.h}

#*** the other (non-library) files that need to be 
OTHERFILES = Makefile

include ../Make.local

#*** any executables go here
EXECS = 

#*** to make the library, include ${MODULELIB}
#*** in the dependencies of 'all'
all:	library ${EXECS}
	@echo ${MODULE}/${THIS_DIR} is ready.

library:	${MODULELIB}
	@true

#*** rules for other targets
slasher_${OS}${LIBTYPE}:	slasher.${OTYPE} ${MODULELIB}
	$(C++) $(CCFLAGS) ${CCOPTS} ${INCLUDES} slasher.${OTYPE} ${LINKS} -o slasher_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/slasher_${OS}${LIBTYPE} is ready.

slasher:	slasher_${OS}${LIBTYPE}
	@true

test_${OS}${LIBTYPE}:	test.cc ${MODULELIB}
	${C++} $(CCFLAGS) ${CCOPTS} ${INCLUDES} test.cc ${LINKS} -o test_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/test_${OS}${LIBTYPE} is ready.

test:	test_${OS}${LIBTYPE}
