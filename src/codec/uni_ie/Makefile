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
OTHER_MODULES = DS FW

#*** library sources and associated headers
LIBSRCS = PNNI_called_party_pvpc.cc ie_base.cc \
	PNNI_calling_party_pvpc.cc PNNI_crankback.cc \
	PNNI_designated_transit_list.cc UNI40_abr.cc UNI40_conn_scope.cc \
	UNI40_e2e_trans_delay.cc UNI40_generic.cc UNI40_leaf.cc \
	UNI40_notif.cc UNI40_td.cc UNI40_xqos.cc UNI40_bbc.cc aal.cc addr.cc \
	bhli.cc blli.cc broadband_repeat_ind.cc broadband_send_comp_ind.cc \
	cache.cc call_state.cc called_party_num.cc \
	called_party_subaddr.cc calling_party_num.cc calling_party_subaddr.cc \
	cause.cc conn_id.cc e2e_transit_delay.cc end_pt_ref.cc end_pt_state.cc\
	ie.cc qos_param.cc restart_ind.cc subaddr.cc td.cc \
	transit_net_sel.cc wildcard.cc \
#	broadband_bearer_cap.cc 

HEADERS  = ${LIBSRCS:%.cc=%.h}

#*** other targets, e.g. linktst.{THIS_DIR}_${OS}
EXECS = linktst_${OS}${LIBTYPE}

#*** files to be compiled for non-library targets
EXECSRCS = linktst.cc 

OTHERFILES = Makefile ${EXECSRCS}

include ../Make.local

CCFLAGS += -DUNI40

#*** executables, and the library ${MODULELIB}
all:	library
	@echo ${MODULE}/${THIS_DIR} is ready.

library:	${MODULELIB}
	@true

#*** PLEASE test your module linkage before you push it into src
linktst:	${EXECSRCS:%.cc=%.${OTYPE}} ${MODULELIB}
	$(C++) -D__${OS}__ $(CCFLAGS) ${EXECOPTS} ${INCLUDES} \
		${EXECSRCS:%.cc=%.${OTYPE}} ${LINKS} -o linktst_${OS}${LIBTYPE}
	@echo ${MODULE}/${THIS_DIR}/linktst_${OS}${LIBTYPE} is ready.
