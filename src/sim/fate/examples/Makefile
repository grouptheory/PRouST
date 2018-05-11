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

 
#*** other libraries we need to link with
OTHER_MODULES = sim fsm codec FW

# LD_LOAD_PATH : separated list
LD_LOAD_PATH=

#*** library sources and associated headers
LIBSRCS =
HEADERS = ${LIBSRCS:%.cc=%.h}
 
#*** other targets
EXECS =
 
#*** files to be compiled for non-library targets
EXECSRCS =
 
include ../../Make.local

ifndef PLUGIN_LIBRARY
PLUGIN_LIBRARY = ${TOP}/sim/plugin/libPluginTest_${OS}${LIBTYPE}.${LIBEXT_BASE}
endif

INCLUDES += -I${TOP}/src -I../.. -I../../..

ifeq (${GREATGRANDPARENT}, src)
        CRON_EXT = 
else
        CRON_EXT = _${GREATGRANDPARENT}
	PARENTDIR=${shell cd ../..; pwd}
endif 

%: %_${OS}${LIBTYPE}
	@echo $@_${OS}${LIBTYPE} is ready

# Use the word following the "program" reserved word as part of the
# name of the output file and the executable.  Assume that "program"
# begins (possibly preceeded by spaces and tabs) one and only one
# line in the input file.  Grab the word following "program" and
# delete from any ; to the end of the line
# This should yield the name of the program.
# It must match the basename of the file.

.tmp/tmp_%.cc: %.fc
	 @echo Parsing $<; \
	 program=`grep "^[ 	]*program[ 	]" $<| 	awk '{print $$2}' | sed 's/;.*//' | head -1`;\
	 root=$*;\
	 if [ "x$$program" != "x$$root" ]; then \
	   echo program name - \"$$program\" - in $< must match file name - \"$$root\";\
	   exit 1; \
	 fi; \
	 if [ ! -d .tmp ]; then mkdir .tmp; fi; \
	 LD_LIBRARY_PATH=`(cd ../..; pwd)`:${LD_LIBRARY_PATH};\
	 export LD_LIBRARY_PATH; \
	 cd .tmp; ../../FC_${OS} < ../$<; \
	 if [ $$? != 0 ]; then \
	    echo FC parse error; \
	    rm tmp_$$root.cc; \
	    exit 1; \
	 fi

%_${OS}${LIBTYPE}: .tmp/tmp_%.cc
	 @root=`basename $< | sed -e 's/tmp_//;s/\..*$$//'`;\
	 echo ${CCC} -D__${OS}__ \
	    -D PLUGIN_LIBRARY=\"${PLUGIN_LIBRARY}\" \
	    ${CCOPTS} ${EXECOPTS} ${INCLUDES} \
	    $< -L../.. -lcronos_${OS}${CRON_EXT} \
	    ${LINKS} -o $$root\_${OS}${LIBTYPE}; \
	${CCC} -D__${OS}__ \
	    -D PLUGIN_LIBRARY=\"${PLUGIN_LIBRARY}\" \
	    ${CCOPTS} ${EXECOPTS} ${INCLUDES} \
	    $< -L../.. -lcronos_${OS}${CRON_EXT} \
	    ${LINKS} -o $$root\_${OS}${LIBTYPE}

# without this intermediate files (including our real target)
# are removed after make
.PRECIOUS: %_${OS} %_${OS}${LIBTYPE} .tmp/tmp_%.cc

all:
	@echo "";
	${MAKE} -f .Makefile.${USER}
#	@echo "Nothing to be made, try 'make -f .Makefile.${USER}'";

test:
	@${COMMON}/test.sh
	@echo "Done."
 
clean::
	@rm -f .tmp/*.o .tmp/*.O .tmp/*.cc *~ .tmp/*~ core *_${OS} *.OMNI gsync.* discord.*;
	@echo ".tmp has been cleaned.";

