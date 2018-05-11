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
static char const _NNITester_cc_rcsid_[] =
"$Id: NNITester.cc,v 1.25 1998/10/15 19:08:43 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "NNITester.h"
#include <iostream.h>
#include <FW/basics/Visitor.h>
#include <codec/q93b_msg/q93b_msg.h>
#include <codec/uni_ie/ie.h>
#include <fsm/forwarder/VCAllocator.h>

//*****************************************
//           IMPORTANT MACROS
//*****************************************
#ifdef _BILAL_TESTING_
#define MAKE_VISITOR(TYPE) { int tmpvci; sscanf(line,"%d",&tmpvci); \
	     v = new VPVCVisitor(0,0,tmpvci); }
#else
//#define MAKE_VISITOR(TYPE) { msg = parse_ ## TYPE ## (line); \
//	     v = new TYPE ## Visitor(msg); }
#define MAKE_VISITOR(TYPE, SUBTYPE) { \
	msg = parse_ ## TYPE ## _ ## SUBTYPE ## (line); \
	v = new TYPE ## Visitor(msg, Q93bVisitor::##TYPE##_##SUBTYPE); }

#define MAKE_VISITOR1(TYPE) { \
	msg = parse_ ## TYPE ## (line); \
	v = new TYPE ## Visitor(msg, Q93bVisitor::##TYPE); }
#endif
//*******************************************
//*******************************************

void NNITester::Absorb(Visitor* v)
{
  cout << OwnerName() << " received visitor (" << v << ") "
       << endl << "   which took the path " << endl;
  const VisitorType *vt2;
  VisitorType vt1 = v->GetType();
  vt2 = QueryRegistry(Q93B_VISITOR_NAME);
  if (vt2 && vt1.Is_A(vt2))
    {
      Q93bVisitor *qv = (Q93bVisitor *)v;
      generic_q93b_msg *msg = qv->get_msg();
      assert(msg);
      InfoElem **ies = msg->get_ie_array();
      int i = 0;
      switch(qv->GetVT())
	{
	case Q93bVisitor::setup_ind :
	  _icrv = qv->get_crv();
	  _ivci = qv->get_vci();
	  cout << OwnerName() << " received setup_ind visitor" << endl;
	  for(i=0; i < num_ie; i++)
	    _iei[i] = ies[i];
	  break;

	case Q93bVisitor::call_proc_ind :
	  cout << OwnerName() << " received call_proc_ind  visitor" << endl;
	  break;

	case Q93bVisitor::connect_ind :
	  cout << OwnerName() << " received connect_ind visitor" << endl;
	  break;

	case Q93bVisitor::release_ind :
	  cout << OwnerName() << " received release_ind visitor" << endl;
	  break;

	case Q93bVisitor::connect_ack_ind :
	  cout << OwnerName() << " received connect_ack_ind visitor" << endl;
	  break;

	case Q93bVisitor::release_comp_ind :
	  cout << OwnerName() << " received release_comp_ind visitor" << endl;
	  break;

	case Q93bVisitor::status_enq_ind :
	  cout << OwnerName() << " received status_enq_ind visitor" << endl;
	  break;

	case Q93bVisitor::status_ind :
	  cout << OwnerName() << " received status_ind visitor" << endl;
	  break;

	case Q93bVisitor::add_party_ind :
	  cout << OwnerName() << " received add_party_ind visitor" << endl;
	  break;

	case Q93bVisitor::add_party_comp_ind :
	  cout << OwnerName() << " received add_party_comp_ind visitor" << endl;
	  break;

	case Q93bVisitor::add_party_ack_ind :
	  cout << OwnerName() << " received add_party_ack_ind visitor" << endl;
	  break;

	case Q93bVisitor::add_party_rej_ind :
	  cout << OwnerName() << " received add_party_rej_ind visitor" << endl;
	  break;

	case Q93bVisitor::drop_party_ind :
	  cout << OwnerName() << " received drop_party_ind visitor" << endl;
	  break;

	case Q93bVisitor::drop_party_comp_ind :
	  cout << OwnerName() << " received drop_party_comp_ind visitor" << endl;
	  break;

	case Q93bVisitor::drop_party_ack_ind :
	  cout << OwnerName() << " received drop_party_ack_ind visitor" << endl;
	  break;

	case Q93bVisitor::leaf_setup_ind :
	  cout << OwnerName() << " received leaf_setup_ind visitor" << endl;
	  break;

	case Q93bVisitor::restart_ind :
	  cout << OwnerName() << " received restart_ind visitor" << endl;
	  break;

	case Q93bVisitor::restart_comp_ind :
	  cout << OwnerName() << " received restart_comp_ind visitor" << endl;
	  break;

	case Q93bVisitor::restart_ack_ind :
	  cout << OwnerName() << " received restart_comp_ind visitor" << endl;
	  break;
	}
    }
  v->Suicide();
}


NNITester::~NNITester(){
  list_item li;
  forall_items(li,_timerpile){
    _timerpile.del_item(li);
  };
  _timerpile.clear();
}


NNITester::NNITester(char* fname, VCAllocator *vcpool, bool master) : Terminal() {
  FILE *fp = fopen(fname,"r");
  if (!fp) {
    cout << "Unable to open file: " << fname << endl;
    exit(1);
  }
  _master = master;
  _vcpool = vcpool;
  _ocrv = 7;
  char op[255];
  double time;
  char vname[255];

  // read the time and opcode, then dispatch for rest of line

  int done=0;
  while((!done)&&(fscanf(fp,"%lf %s %s",&time,op,vname)!=EOF)){
    int opcode = tokenize(op);
    generic_q93b_msg * msg = 0;
    NNIReplayer* rep;
    InjectionVisitor * v;

    char line[512];
    strcpy(line,"");
    fgets(line,500,fp);

    switch (opcode){
    case unknown:
      cout << "NNITester: unknown op" << endl;
      break;
    case setup_code:
      cout << "NNITester: setup op ";
      //msg = parse_setup_req(line);
      //v = new setupVisitor(msg, Q93bVisitor::setup_req);

      MAKE_VISITOR(setup, req);

      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case setup_comp_code:
      cout << "NNITester: setup_comp op ";
      MAKE_VISITOR(setup_comp, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case call_proc_code:
      cout << "NNITester: call_proc op ";
      MAKE_VISITOR(call_proc, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case connect_code:
      cout << "NNITester: connect op ";
      MAKE_VISITOR(connect, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case connect_ack_code:
      cout << "NNITester: connect_ack op ";
      MAKE_VISITOR(connect_ack, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case release_code:
      cout << "NNITester: release op ";
      MAKE_VISITOR(release, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case release_comp_code:
      cout << "NNITester: release_comp op ";
      MAKE_VISITOR(release_comp, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case status_enq_code:
      cout << "NNITester: status_enq op ";
      MAKE_VISITOR(status_enq, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case status_resp_code:
      cout << "NNITester: status_resp op ";
#if 0
      MAKE_VISITOR(status_resp, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
#else
      cout << " I don't know how to make one of those." << endl;
#endif
      break;
    case add_party_code:
      cout << "NNITester: add_party op ";
      MAKE_VISITOR(add_party, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case add_party_comp_code:
      cout << "NNITester: add_party_comp op ";
      MAKE_VISITOR(add_party_comp, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case add_party_ack_code:
      cout << "NNITester: add_party_ack op ";
      MAKE_VISITOR(add_party_ack, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case add_party_rej_code:
      cout << "NNITester: add_party_rej op ";
      MAKE_VISITOR(add_party_rej, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case drop_party_code:
      cout << "NNITester: drop_party op ";
      MAKE_VISITOR(drop_party, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case drop_party_comp_code:
      cout << "NNITester: drop_party_comp op ";
      MAKE_VISITOR(drop_party_comp, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case drop_party_ack_code:
      cout << "NNITester: drop_party_ack op ";
      MAKE_VISITOR(drop_party_ack, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case leaf_setup_code:
      cout << "NNITester: leaf_setup op ";
      MAKE_VISITOR(leaf_setup, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case restart_code:
      cout << "NNITester: restart op ";
      MAKE_VISITOR(restart, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case restart_resp_code:
      cout << "NNITester: restart_resp op ";
      MAKE_VISITOR1(restart_resp);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case restart_ack_code:
      cout << "NNITester: restart_ack op ";
      MAKE_VISITOR(restart_ack, req);
      rep = new NNIReplayer(time,v,vname,this);
      Register(rep);
      _timerpile.append(rep);
      cout << "registered for time " << time << endl;
      break;
    case passive_mode:
      done = 1; // get out of the loop
      break;
    default:
      break;
    }

    int vci,cref;
    sscanf(line,"%d %d",&vci,&cref);
    cout << "Hammering vci " << vci << " and cref " << cref << " into visitor prior to launch\n";
    v->set_vci(vci);
    v->set_crv(cref);
  };

  fclose(fp);
}

//----------------------------------------------------------
int NNITester::GetCRV()
{
  _ocrv++;
  return(_ocrv);
}

//----------------------------------------------------------

int NNITester::tokenize(char* op){
  if (strcmp(op,"setup")==0) return (int)(NNITester::setup_code);
  else if (strcmp(op,"setup_comp")==0) 
    return (int)(NNITester::setup_comp_code);
  else if (strcmp(op,"call_proc")==0) 
    return (int)(NNITester::call_proc_code);
  else if (strcmp(op,"connect")==0) 
    return (int)(NNITester::connect_code);
  else if (strcmp(op,"connect_ack")==0) 
    return (int)(NNITester::connect_ack_code);
  else if (strcmp(op,"release")==0) 
    return (int)(NNITester::release_code);
  else if (strcmp(op,"release_comp")==0) 
    return (int)(NNITester::release_comp_code);
  else if (strcmp(op,"status_enq")==0) 
    return (int)(NNITester::status_enq_code);
  else if (strcmp(op,"status_resp")==0) 
    return (int)(NNITester::status_resp_code);
  else if (strcmp(op,"add_party")==0) 
    return (int)(NNITester::add_party_code);
  else if (strcmp(op,"add_party_comp")==0) 
    return (int)(NNITester::add_party_comp_code);
  else if (strcmp(op,"add_party_ack")==0) 
    return (int)(NNITester::add_party_ack_code);
  else if (strcmp(op,"add_party")==0) 
    return (int)(NNITester::add_party_code);
  else if (strcmp(op,"drop_party")==0) 
    return (int)(NNITester::drop_party_code);
  else if (strcmp(op,"drop_party_comp")==0) 
    return (int)(NNITester::drop_party_comp_code);
  else if (strcmp(op,"drop_party_ack")==0) 
    return (int)(NNITester::drop_party_ack_code);
  else if (strcmp(op,"leaf_setup")==0) 
    return (int)(NNITester::leaf_setup_code);
  else if (strcmp(op,"restart")==0) 
    return (int)(NNITester::restart_code);
  else if (strcmp(op,"restart_resp")==0) 
    return (int)(NNITester::restart_resp_code);
  else if (strcmp(op,"restart_ack")==0) 
    return (int)(NNITester::restart_ack_code);
  else if (strcmp(op,"end")==0) 
    return (int)(NNITester::passive_mode);
  else return (int)(NNITester::unknown);
}

//----------------------------------------------------------

generic_q93b_msg* NNITester::parse_setup_req(char *line)
{
  sscanf(line,"%d %d",&_ovci,&_ocrv);
  _vcpool->RequestVCI(_ovci);
  if(_master)
    {
      // Since I'am the master I allocate the VCI
      _ieo[InfoElem::ie_conn_identifier_ix] = new ie_conn_id(_ovci,0);
    }
  ie_traffic_desc* td=  new ie_traffic_desc();
  td->set_TP1(ie_traffic_desc::fw,100,100);
  td->set_TP1(ie_traffic_desc::bw,100,100);
  _ieo[InfoElem::ie_traffic_desc_ix]= td;
  _ieo[InfoElem::ie_broadband_bearer_cap_ix]=
    new C_Bbc(ie_broadband_bearer_cap::not_clipped,ie_broadband_bearer_cap::p2p);
  _addr = new NSAP_DCC_ICD_addr("0x47.0005.80.ffde00.0000.0000.0104.0020480620eb.02");
  _ieo[InfoElem::ie_called_party_num_ix] = new ie_called_party_num(_addr);;
  _ieo[InfoElem::ie_qos_param_ix] = new ie_qos_param(ie_qos_param::qos0,ie_qos_param::qos0);
  q93b_setup_message *m = new q93b_setup_message(_ieo,_ocrv,1);
  return m;
}
generic_q93b_msg* NNITester::parse_setup_comp_req(char* line){
  return 0;
}

generic_q93b_msg* NNITester::parse_call_proc_req(char* line){
  sscanf(line,"%d %d",&_ivci,&_icrv);

  _iei[InfoElem::ie_conn_identifier_ix] = new ie_conn_id(_ivci,0);

  q93b_call_proceeding_message *m = new q93b_call_proceeding_message(_iei,_icrv,1);
  return m;
}

generic_q93b_msg* NNITester::parse_connect_req(char* line){
  sscanf(line,"%d %d",&_ivci,&_icrv);

  q93b_connect_message *m = new  q93b_connect_message(_iei,_icrv,1);
  return m;
}

generic_q93b_msg* NNITester::parse_connect_ack_req(char* line){
  return 0;
}

generic_q93b_msg* NNITester::parse_release_req(char* line){
  sscanf(line,"%d %d",&_ovci,&_ocrv);

  _ieo[InfoElem::ie_cause_ix] = new ie_cause();
  q93b_release_message *m = new q93b_release_message(_ieo,_ocrv,1);
  return m;
}

generic_q93b_msg* NNITester::parse_release_comp_req(char* line){
  
  return 0;
}

generic_q93b_msg* NNITester::parse_status_enq_req(char* line){
  return 0;
}

generic_q93b_msg* NNITester::parse_status_resp_req(char* line){
  return 0;
}

generic_q93b_msg* NNITester::parse_add_party_req(char* line){
  return 0;
}

generic_q93b_msg* NNITester::parse_add_party_comp_req(char* line){
  return 0;
}

generic_q93b_msg* NNITester::parse_add_party_ack_req(char* line){
  return 0;
}

generic_q93b_msg* NNITester::parse_add_party_rej_req(char* line){
  return 0;
}

generic_q93b_msg* NNITester::parse_drop_party_req(char* line){
  return 0;
}

generic_q93b_msg* NNITester::parse_drop_party_comp_req(char* line){
  return 0;
}

generic_q93b_msg* NNITester::parse_drop_party_ack_req(char* line){
  return 0;
}

generic_q93b_msg* NNITester::parse_leaf_setup_req(char* line){
  return 0;
}

generic_q93b_msg* NNITester::parse_restart_req(char* line){
  return 0;
}

generic_q93b_msg* NNITester::parse_restart_resp(char* line){
  return 0;
}

generic_q93b_msg* NNITester::parse_restart_ack_req(char* line){
  return 0;
}

//----------------------------------------------------------

NNIReplayer::NNIReplayer(double time, InjectionVisitor * v, 
			 char* vname, NNITester* owner)
  : TimerHandler(owner,time) {
    _v = v;
    _owner = owner;
    strcpy(_vname,vname);
}

NNIReplayer::~NNIReplayer() {
}

void NNIReplayer::Callback(void) {
  cout << "NNIReplayer injecting " << _vname << " (" << _v << ") " 
       << " at time " << (double) GetExpirationTime() << endl;
  _v->SetLoggingOn();
  _owner->Inject(_v);
  _owner=0;
  _v=0;
}




