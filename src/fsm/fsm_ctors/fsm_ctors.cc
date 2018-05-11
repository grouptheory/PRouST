// -*- C++ -*-
// +++++++++++++++
//   P R o u S T     ---  PNNI Routing Simulation Toolkit  ---
// +++++++++++++++
// Version: 1.0 (Fri Mar  5 14:35:33 EST 1999)
// 
// 			  Copyright (c) 1998
// 		 Naval Research Laboratory (NRL/CCS)
// 			       and the
// 	  Defense Advanced Research Projects Agency (DARPA)
// 
// 			 All Rights Reserved.
// 
// Permission to use, copy, and modify this software and its
// documentation is hereby granted, provided that both the copyright notice and
// this permission notice appear in all copies of the software, derivative
// works or modified versions, and any portions thereof, and that both notices
// appear in supporting documentation.
// 
// NRL AND DARPA ALLOW FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION AND
// DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM
// THE USE OF THIS SOFTWARE.
// 
// NRL and DARPA request users of this software to return modifications,
// improvements or extensions that they make to:
// 
//                 proust-dev@cmf.nrl.navy.mil
//                         -or-
//                Naval Research Laboratory, Code 5590
//                Center for Computation Science
//                Washington, D.C.  20375
// 
// and grant NRL and DARPA the rights to redistribute these changes in
// future upgrades.
//

// -*- C++ -*-
#ifndef LINT
static char const _ctors_fsm_cc_rcsid_[] =
"$Id: fsm_ctors.cc,v 1.50 1998/12/03 16:35:14 mountcas Exp bilal $";
#endif

#include <common/cprototypes.h>

#include <fsm/visitors/PacketVisitor.h>
vistype PacketVisitor::_my_type(PACKET_VISITOR_NAME);

#include <fsm/election/ElectionVisitor.h>
vistype ElectionVisitor::_my_type(ELECTION_VISITOR_NAME);

#include <codec/pnni_pkt/pkt_incl.h>
#include <FW/basics/Visitor.h>
#include <FW/basics/VisitorType.h>
#include <fsm/hello/HelloVisitor.h>

vistype HelloVisitor::_my_type(HELLO_VISITOR_NAME);
vistype HorLinkVisitor::_my_type(HLINK_VISITOR_NAME);
vistype SVCCPortInfoVisitor::_my_type(SVCC_PORT_INFO_VISITOR);

// --------------------- PortVisitor ---------------------------
#include <fsm/visitors/PortVisitor.h>
vistype PortVisitor::_mytype(PORT_VISITOR_NAME);

// ----------------- Data Visitor ---------------
#include <fsm/visitors/DataVisitor.h>
vistype DataVisitor::_my_type(DATA_VISITOR_NAME);

// ----------------- SaveVisitor ---------------
#include <fsm/visitors/FileIOVisitors.h>
vistype SaveVisitor::_my_type(SAVE_VISITOR_NAME);

// -------------- StreamSaveVisitor -------------
vistype StreamSaveVisitor::_my_type(STREAM_SAVE_VISITOR_NAME);

// ----------------- ReadVisitor ---------------
vistype ReadVisitor::_my_type(READ_VISITOR_NAME);

// ----------------- ShowVisitor ---------------
vistype ShowVisitor::_my_type(SHOW_VISITOR_NAME);

// ----------------- LGNInstantiationVisitor -----------
#include <fsm/visitors/LGNVisitors.h>

vistype LGNInstantiationVisitor::_my_type(LGN_INSTANTIATION_VISITOR);
vistype LGNDestructionVisitor::_my_type(LGN_DESTRUCTION_VISITOR);

// ----------------- FastUNI Visitor ---------------
#include <fsm/visitors/FastUNIVisitor.h>
vistype FastUNIVisitor::_my_type(FAST_UNI_VISITOR_NAME);

// --------------------- LinkUpVisitor ---------------------------
#include <fsm/visitors/LinkUpVisitor.h>
vistype LinkUpVisitor::_mytype(LINK_UP_VISITOR_NAME);

// --------------------- LinkDownVisitor ---------------------------
#include <fsm/visitors/LinkDownVisitor.h>
vistype LinkDownVisitor::_mytype(LINK_DOWN_VISITOR_NAME);


// --------------------- Border Up/Down Visitor ----------------
#include <fsm/visitors/BorderVisitor.h>
vistype BorderUpVisitor::_my_type(BORDER_UP_VISITOR_NAME);
vistype BorderDownVisitor::_my_type(BORDER_DOWN_VISITOR_NAME);


// ------------------- DBIntroVisitor ---------------------
#include <fsm/visitors/DBVisitors.h>
vistype DBIntroVisitor::_my_type(DB_INTRO_VISITOR_NAME);


// --------------------- LinkVisitor ---------------------------
#include <fsm/visitors/LinkVisitor.h>
vistype LinkVisitor::_mytype(LINK_VISITOR_NAME);


// ------------------ DB NPFloodVisitor -------------------
#include <fsm/visitors/NPFloodVisitor.h>
vistype NPFloodVisitor::_mytype(NPFLOOD_VISITOR_NAME);


// --------------------- PNNIVisitor ---------------------------
#include <fsm/visitors/PNNIVisitor.h>
vistype PNNIVisitor::_mytype(PNNI_VISITOR_NAME);


// --------------------- PortDownVisitor ---------------------------
#include <fsm/visitors/PortDownVisitor.h>
vistype PortDownVisitor::_mytype(PORT_DOWN_VISITOR_NAME);


// --------------------- PortUpVisitor ---------------------------
#include <fsm/visitors/PortUpVisitor.h>
vistype PortUpVisitor::_mytype(PORT_UP_VISITOR_NAME);

// --------------------- NodeIDVisitor ---------------------------
#include <fsm/visitors/NodeIDVisitor.h>
vistype NodeIDVisitor::_myType(NODEID_VISITOR_NAME);

// --------------------- VPVCVisitor ---------------------------
#include <fsm/visitors/VPVCVisitor.h>
vistype VPVCVisitor::_mytype(VPVC_VISITOR_NAME);

#include <fsm/visitors/VPVCRebindingVisitor.h>
vistype VPVCRebindingVisitor::_mytype(VPVC_REBINDING_VISITOR_NAME);

//---------------------- Q93bVisitor ------------------
#include <fsm/nni/Q93bVisitors.h>
vistype Q93bVisitor::_my_type(Q93B_VISITOR_NAME);
vistype setupVisitor::_my_type(SETUP_VISITOR_NAME);
vistype setup_compVisitor::_my_type(SETUP_COMP_VISITOR_NAME);
vistype call_procVisitor::_my_type(CALL_PROC_VISITOR_NAME);
vistype connectVisitor::_my_type(CONNECT_VISITOR_NAME);
vistype connect_ackVisitor::_my_type(CONNECT_ACK_VISITOR_NAME);
vistype releaseVisitor::_my_type(RELEASE_VISITOR_NAME);
vistype release_compVisitor::_my_type(RELEASE_COMP_VISITOR_NAME);
vistype status_enqVisitor::_my_type(STATUS_ENQ_VISITOR_NAME);
vistype statusVisitor::_my_type(STATUS_VISITOR_NAME);
vistype add_partyVisitor::_my_type(ADD_PARTY_VISITOR_NAME);
vistype add_party_compVisitor::_my_type(ADD_PARTY_COMP_VISITOR_NAME);
vistype add_party_ackVisitor::_my_type(ADD_PARTY_ACK_VISITOR_NAME);
vistype add_party_rejVisitor::_my_type(ADD_PARTY_REJ_VISITOR_NAME);
vistype drop_partyVisitor::_my_type(DROP_PARTY_VISITOR_NAME);
vistype drop_party_compVisitor::_my_type(DROP_PARTY_VISITOR_NAME);
vistype drop_party_ackVisitor::_my_type(DROP_PARTY_ACK_VISITOR_NAME);
vistype leaf_setupVisitor::_my_type(LEAF_SETUP_VISITOR_NAME);
vistype restartVisitor::_my_type(RESTART_VISITOR_NAME);
vistype restart_respVisitor::_my_type(RESTART_RESP_VISITOR_NAME);
vistype restart_ackVisitor::_my_type(RESTART_ACK_VISITOR_NAME);


//--------------- SSCFLinkStatusVisitor ---------------
#include <fsm/nni/SSCFVisitor.h>
vistype SSCFLinkStatusVisitor::_my_type(SSCF_LINKSTATUS_VISITOR_NAME);

// ----------------- Queue Types -----------------------
#include <fsm/visitors/QueueVisitor.h>
vistype _queue_kill_me_type(QUEUE_KILL_ME_NAME);
vistype _queue_overrider_type(QUEUE_OVERRIDER_NAME);

// -------------------- NpeerVisitors --------
#include <fsm/nodepeer/NPVisitors.h>
vistype DSVisitor::_my_type(DS_VISITOR_NAME);
vistype ReqVisitor::_my_type(REQ_VISITOR_NAME);
vistype AckVisitor::_my_type(ACK_VISITOR_NAME);
vistype PTSPVisitor::_my_type(PTSP_VISITOR_NAME);
vistype NPStateVisitor::_my_type(NPSTATE_VISITOR_NAME);

// -------------------------------------------------------------
extern bool FW_ctors_inited;
extern bool init_ctors_FW(void);
extern bool init_ctors_fsm(void);

bool fsm_ctors_inited = init_ctors_fsm();

bool init_ctors_fsm(void) 
{
  if (!FW_ctors_inited) 
    FW_ctors_inited = init_ctors_FW();

  if (fsm_ctors_inited)
    return fsm_ctors_inited;

#ifdef MAKE_VISITORS_IN_CTORS
  ElectionVisitor * global_ElectionVisitor = new ElectionVisitor(0,0,0);
  HelloVisitor    * global_HelloVisitor    =
    new HelloVisitor(HelloVisitor::PhyHello,0,0,0,0);
  HorLinkVisitor  * global_HorLinkVisitor  =
    new HorLinkVisitor(HLVisitorTypes::HelloLGN,0,0);

  BorderUpVisitor* global_BorderUpVisitor =
    new BorderUpVisitor(0,0,0,0,0,0,0,0,0,0,0);
  BorderDownVisitor* global_BorderDownVisitor =
    new BorderDownVisitor(0,0,0,0,0,0,0,0,0,0);
  DBIntroVisitor* global_DBIntroVisitor = new DBIntroVisitor(0);

  DataVisitor *global_DataVisitor = new DataVisitor((void *)0);
  SaveVisitor *global_DataVisitor = new SaveVisitor("test.save");
  ReadVisitor *global_DataVisitor = new ReadVisitor("test.read");
  ShowVisitor *global_DataVisitor = new ShowVisitor("test.show");

  LinkVisitor* global_LinkVisitor = new LinkVisitor(0,0,0,0,0,0,0);
  NPFloodVisitor* global_NPFloodVisitor = new NPFloodVisitor(0);
  PNNIVisitor* global_PNNIVisitor = new PNNIVisitor(0,0,0,0);
  PortUpVisitor* global_PortUpVisitor = new PortUpVisitor(0,0,0,0,0,0,0);
  PortDownVisitor* global_PortDownVisitor = new PortDownVisitor(0,0,0,0,0,0);
  PortVisitor* global_PortVisitor = new PortVisitor(0);
  VPVCVisitor* global_VPVCVisitor = new VPVCVisitor(0,0,0,0,0);
#endif

  return true;
}

#include "fsm_atexit.h"
#include "fsm_atexit.cc"

#ifdef __ATEXIT_CLEANUP_H__
fsm_atexitCleanup fsm_clean_now;
#endif


