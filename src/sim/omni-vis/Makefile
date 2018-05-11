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
EXECS = OmniVis_${OS}${LIBTYPE}

#*** files to be compiled for non-library targets
EXECSRCS = ov_main.cc ov_main.moc.cc \
	WidgetView.cc WidgetView.moc.cc \
	canvas3D.cc ov_tmpl.cc \
	ov_event.cc ov_parser.cc ov_data.cc \
	Event.cc OVNode.cc \
	eventTypeRegistry.cc levelRegistry.cc \
	connComp.cc levelSubgraph.cc cHull.cc \
	TexFont.cc ov_ec.cc id.cc

OTHERFILES = Makefile ${EXECSRCS} pixmaps FONTS MESA_config QT_config .needs_depend_local

include ../Make.local
include ./QT_config
include ./MESA_config

CCOPTS += -DCONV_TEST

%.moc.cc: %.h
	$(QT_MOC) -o $@ $<

.PRECIOUS: %.moc.cc

_o.${OS}/%.${OTYPE}:  %.cc
	${CCC} -D__${OS}__ $(CCFLAGS) ${CCOPTS} ${EXECOPTS} ${INCLUDES} -c $< -o $@

_o.${OS}:
	@if [ ! -d _o.${OS} ]; then mkdir _o.${OS}; fi

#*** executables, and the library ${MODULELIB}
ifeq (${QT_ROOT},)
  all:
	@echo QT_ROOT not defined edit QT_config in ${GRANDPARENT}/${MODULE}/${THIS_DIR}
else
  all:	_o.${OS} ${EXECS}
	@echo ${MODULE}/${THIS_DIR} is ready.
endif

INCLUDES += -I$(QT_INC) -I$(GL_INC)
LINKS = -L${GL_LIB} ${GL_LIBS} -L${QT_LIB} -l${QT_libname} -L${TOP}/lib/${OS} -ljiggle_${OS}${LIBTYPE} \
	-lDS_${OS}${LIBTYPE} -lctorsDS_${OS}${LIBTYPE} -lDS_${OS}${LIBTYPE} \
	-L${X11_LIB} -lX11 -lXi -lXmu -lm -lc

ifeq (${OS},NetBSD)
   RLINKS = -R.:..:../..:${QT_LIB}:${GL_LIB}:${TOP}/lib/${OS}
   LINKS += ${RLINKS}
endif

ifeq (${OS},SunOS5)
	LINKS += -R..:${GL_LIB}:${QT_LIB}:${TOP}/lib/${OS}:${X11_LIB}
endif

ifeq (${OS},IRIX64)
	LINKS += -rpath ..:${GL_LIB}:${QT_LIB}:${TOP}/lib/${OS}:${X11_LIB}
endif

ifeq (${OS},Linux)
	LINKS += -Xlinker -rpath -Xlinker ..:${QT_LIB}:${GL_LIB}:${TOP}/lib/${OS}:${X11_LIB}
endif

## Uncomment the below to add more functionality
## CCOPTS += -DMOVE_AFFECTS_CHILDREN -DHYPNOTIC

OmniVis_${OS}${LIBTYPE}:	${EXECSRCS:%.cc=_o.${OS}/%.${OTYPE}}
	${CCC} -D__${OS}__ $(CCOPTS) ${EXECOPTS} ${INCLUDES} \
		${EXECSRCS:%.cc=_o.${OS}/%.${OTYPE}} ${LINKS} -o OmniVis_${OS}${LIBTYPE}

OmniVis:	OmniVis_${OS}${LIBTYPE}
	@true

depend_local:
	@echo making ${THIS_DIR} local dependencies
	@${CCC} -M ${EXECSRCS} ${INCLUDES} |\
	sed -e 's/\(.*\)\.o/_o.${OS}\/\1.o/' > .depend_${OS}

# clean only cleans for the current OS
clean::
	@rm -rf _o.${OS}
	@rm -f *_${OS}*	 *.moc.cc 

# distclean tries to remove anything created since untarring
distclean::
	for O in  _o.*; do \
	  if [ "x$$O" != "x_o.*" ]; then \
	    Os=`echo $$O | sed 's/_o\.//'`; \
	    rm -rf $$O *_$$Os*;\
	  fi \
	done
	@rm -f *.moc.cc .depend*
