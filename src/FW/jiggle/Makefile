# -*- Makefile -*-
#*** developer/module we are exposed to during development
OTHER_DEVS = 

#*** other libraries we need to link with
OTHER_MODULES = 

#*** library sources and associated headers
LIBSRCS = 
HEADERS = ${LIBSRCS:%.cc=%.h}

LOCAL_LIBSRCS = BinaryTree.cc Model.cc SpringChargeModel.cc \
	CGwithRestarts.cc My.cc SquareMesh.cc Coords.cc \
	Node.cc SteepestDescent.cc DynamicArray.cc Object.cc \
	Vector.cc Edge.cc Optimizer.cc Graph.cc QuadTree.cc \
	Hypercube.cc RandomGraph.cc

LOCALSRCS = ${LOCAL_LIBSRCS}

include ../Make.local

ifeq (${GRANDPARENT}, src)
	LOCAL_LIB = ${TOP}/lib/${OS}/libjiggle_${OS}${LIBTYPE}.${LIBEXT}
else
	LOCAL_LIB = ../libjiggle_${OS}${LIBTYPE}.${LIBEXT}
endif

#*** other targets, e.g. linktst.${THIS_DIR}_${OS}
EXECS = jiggle_${OS}${LIBTYPE}

#*** files to be compiled for non-library targets
EXECSRCS = 

OTHERFILES = Makefile ${EXECSRCS}

_o.${OS}/%.${OTYPE}:  %.cc
	${CCC} -D__${OS}__ $(CCFLAGS) ${CCOPTS} ${EXECOPTS} ${INCLUDES} -c $< -o $@

#*** executables, and the library ${MODULELIB}
all:	${MODULELIB} ${LOCAL_LIB}
	@echo ${MODULE}/${THIS_DIR} is ready.

library: ${MODULELIB} ${LOCAL_LIB}
	@true

LINKS += -L.. -ljiggle_${OS}

_o.${OS}:
	mkdir _o.${OS}

#*** PLEASE test your module linkage before you push it into src
${LOCAL_LIB}:	_o.${OS} ${LOCAL_LIBSRCS:%.cc=_o.${OS}/%.${OTYPE}}
	${LD} ${LDSHOPTS} ${LDOPTS} ${LOCAL_LIBSRCS:%.cc=_o.${OS}/%.${OTYPE}} -o ${LOCAL_LIB}

jiggle_${OS}${LIBTYPE}:	${EXECSRCS:%.cc=_o.${OS}/%.${OTYPE}} ${LOCAL_LIB}
	${CCC} -D__${OS}__ ${CCOPTS} ${EXECOPTS} ${INCLUDES} \
		${EXECSRCS:%.cc=_o.${OS}/%.${OTYPE}} ${LINKS} -o jiggle_${OS}${LIBTYPE}
	@echo jiggle_${OS}${LIBTYPE} is ready.

clean::
	rm -f ./_o.${OS}/*.[oO]

distclean::
	rm -rf _o.* ${LOCAL_LIB}

depend::
	${DependSubDir}
