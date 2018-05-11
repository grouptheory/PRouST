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
#ifndef __NNITESTER_H__
#define __NNITESTER_H__
#ifndef LINT
static char const _NNITester_h_rcsid_[] =
"$Id: NNITester.h,v 1.22 1998/07/01 14:54:51 mountcas Exp $";
#endif

#include <FW/actors/Terminal.h>
#include <DS/containers/list.h>

class InfoElem;
class Addr;

#include <codec/uni_ie/ie_base.h>

class NNITester;
class Q93bTester;
class generic_q93b_msg;
class VCAllocator ;

//*****************************************
//           IMPORTANT MACROS
//*****************************************
#ifdef _BILAL_TESTING_

#include <fsm/visitors/VPVCVisitor.h>
#define InjectionVisitor VPVCVisitor

#else

#include <fsm/nni/Q93bVisitors.h>
#define InjectionVisitor Q93bVisitor

#endif
//*******************************************
//*******************************************


//----------------------------------------------
#include <FW/kernel/Handlers.h>

class NNIReplayer : public TimerHandler {
public:
  
  NNIReplayer(double time, InjectionVisitor * v, char * name, NNITester* owner);
  virtual ~NNIReplayer();

  void Callback(void);

private:

  InjectionVisitor * _v;
  NNITester   *_owner;
  char         _vname[255];
};

//------------------------------------------
class NNITester;

//----------------------------------------------


class NNITester: public Terminal {
private:

  int tokenize(char* op);

  enum code {
    unknown=0,
    setup_code,
    setup_comp_code,
    call_proc_code,
    connect_code,
    connect_ack_code,
    release_code,
    release_comp_code,
    status_enq_code,
    status_resp_code,
    add_party_code,
    add_party_comp_code,
    add_party_ack_code,
    add_party_rej_code,
    drop_party_code,
    drop_party_comp_code,
    drop_party_ack_code,
    leaf_setup_code,
    restart_code,
    restart_resp_code,
    restart_ack_code,
    passive_mode
  };

  generic_q93b_msg* parse_setup_req(char* line);
  generic_q93b_msg* parse_setup_comp_req(char* line);
  generic_q93b_msg* parse_call_proc_req(char* line);
  generic_q93b_msg* parse_connect_req(char* line);
  generic_q93b_msg* parse_connect_ack_req(char* line);
  generic_q93b_msg* parse_release_req(char* line);
  generic_q93b_msg* parse_release_comp_req(char* line);
  generic_q93b_msg* parse_status_enq_req(char* line);
  generic_q93b_msg* parse_status_resp_req(char* line);
  generic_q93b_msg* parse_add_party_req(char* line);
  generic_q93b_msg* parse_add_party_comp_req(char* line);
  generic_q93b_msg* parse_add_party_ack_req(char* line);
  generic_q93b_msg* parse_add_party_rej_req(char* line);
  generic_q93b_msg* parse_drop_party_req(char* line);
  generic_q93b_msg* parse_drop_party_comp_req(char* line);
  generic_q93b_msg* parse_drop_party_ack_req(char* line);
  generic_q93b_msg* parse_leaf_setup_req(char* line);
  generic_q93b_msg* parse_restart_req(char* line);
  generic_q93b_msg* parse_restart_resp(char* line);
  generic_q93b_msg* parse_restart_ack_req(char* line);

public:

  NNITester(char * fname, VCAllocator * vcpool, bool master);
  virtual ~NNITester();

  void Absorb(Visitor* v);
  void Interrupt(class SimEvent *) { }
  int GetCRV();

private:

  list<NNIReplayer*> _timerpile;
  VCAllocator *_vcpool;
  int _ovci;
  int _ocrv;
  int _ivci;
  int _icrv;
  bool _master;
  Addr *_addr;
  InfoElem* _ieo[num_ie];
  InfoElem* _iei[num_ie];
};

#endif
