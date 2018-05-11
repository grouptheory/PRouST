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
#ifndef _Q93BVISITORS_H_
#define _Q93BVISITORS_H_

#ifndef LINT
static char const _Q93bVisitors_h_rcsid_[] =
"$Id: Q93bVisitors.h,v 1.58 1999/01/28 15:52:54 mountcas Exp battou $";
#endif

#include <FW/kernel/Kernel.h>
#include <FW/actors/State.h>
#include <FW/basics/Visitor.h>

#include <codec/q93b_msg/header_parser.h>
#include <fsm/visitors/PacketVisitor.h>

#define Q93B_VISITOR_NAME           "Q93bVisitor"
#define SETUP_VISITOR_NAME          "setupVisitor"
#define SETUP_COMP_VISITOR_NAME     "setup_compVisitor"
#define CALL_PROC_VISITOR_NAME      "call_procVisitor"
#define CONNECT_VISITOR_NAME        "connectVisitor"
#define CONNECT_ACK_VISITOR_NAME    "connect_ackVisitor"
#define RELEASE_VISITOR_NAME        "releaseVisitor"
#define RELEASE_COMP_VISITOR_NAME   "release_compVisitor"
#define STATUS_ENQ_VISITOR_NAME     "status_enqVisitor"
#define STATUS_VISITOR_NAME         "status_respVisitor"
#define ADD_PARTY_VISITOR_NAME      "add_partyVisitor"
#define ADD_PARTY_COMP_VISITOR_NAME "add_party_compVisitor"
#define ADD_PARTY_ACK_VISITOR_NAME  "add_party_ackVisitor"
#define ADD_PARTY_REJ_VISITOR_NAME  "add_party_rejVisitor"
#define DROP_PARTY_VISITOR_NAME     "drop_partyVisitor"
#define DROP_PARTY_COMP_VISITOR_NAME     "drop_party_compVisitor"
#define DROP_PARTY_ACK_VISITOR_NAME "drop_party_ackVisitor"
#define LEAF_SETUP_VISITOR_NAME     "leaf_setupVisitor"
#define RESTART_VISITOR_NAME        "restartVisitor"
#define RESTART_RESP_VISITOR_NAME   "restart_respVisitor"
#define RESTART_ACK_VISITOR_NAME    "restart_ackVisitor"

class generic_q93b_msg;
class Call;
class NodeID;
class Bomb;

class Q93bVisitor : public Visitor, public PacketVisitor {
  friend ostream & operator << (ostream & os, const Q93bVisitor & rhs);
  friend class Translator;
public:

  enum Q93bVisitorType {
    setup_req =  1,
    setup_ind = -1,

    call_proc_req = 2,
    call_proc_ind = -2,

    connect_req = 3,
    connect_ind = -3,

    release_req =  4,
    release_ind = -4,

    connect_ack_req = 5,
    connect_ack_ind = -5,

    setup_comp_req = 6,
    setup_comp_ind = -6,

    release_comp_req = 7,
    release_comp_ind = -7,

    status_enq_req = 8,
    status_enq_ind = -8,

    status_req = 9,
    status_ind = -9,

    add_party_req = 10,
    add_party_ind = -10,

    add_party_comp_req = 11,
    add_party_comp_ind = -11,

    add_party_ack_req = 12,
    add_party_ack_ind = 12,

    add_party_rej_req = 13,
    add_party_rej_ind = -13,

    drop_party_req = 14,
    drop_party_ind = -14,

    drop_party_comp_req = 15,
    drop_party_comp_ind = -15,

    drop_party_ack_req = 16,
    drop_party_ack_ind = -16,

    leaf_setup_req = 17,
    leaf_setup_ind = -17,

    restart_req = 18,
    restart_ind = -18,

    restart_resp = 19,
    restart_comp_ind = -19,

    restart_ack_req = 20,
    restart_ack_ind = -20
  };

  Q93bVisitor(generic_q93b_msg* msg, Q93bVisitorType vt);
  Q93bVisitor(Q93bVisitor & him, int msg_type);
  Q93bVisitor(const Q93bVisitor & him);

  virtual ~Q93bVisitor();
  virtual bool CallProtocol(Call *c) = 0;

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

  Q93bVisitorType GetVT();

  generic_q93b_msg* get_msg(void);
  generic_q93b_msg* take_msg(void);
  generic_q93b_msg* share_msg(void);
  void set_msg(generic_q93b_msg* msg);

  char * label(void);

  void set_crv(int crv);
  int  get_crv(void);

  void set_pid(int epr);
  int  get_pid(void);

  void set_port(int port);
  int  get_port(void);

  void set_msg_type(header_parser::msg_type mt);
  header_parser::msg_type get_msg_type(void);

  int get_vpi(void);
  int get_vci(void);

  void set_vpi(int vpi);
  void set_vci(int vci);

  // This is more explicit that something like ToggleVT()
  void TurnIntoReq(void);
  void TurnIntoInd(void);

  bool IsReq(void);
  bool IsInd(void);

  // WARNING: Explosives
  Bomb * SetExplosives(void);

  bool encode(      unsigned char *& buffer, int & len) const;
  bool decode(const unsigned char *& buffer, int & len);

protected:

  Q93bVisitor(vistype & child_type, generic_q93b_msg * msg, Q93bVisitorType vt);

private:

  static vistype      _my_type;
  Q93bVisitorType     _vt;
  generic_q93b_msg  * _msg;
  char                _label[80];

  int                 _vpi;
  int                 _vci;
  // these are needed at CallControl
  int                 _port; 
  int                 _epr;

  //----------------------------------------------
};

class setupVisitor : public Q93bVisitor {
public:

  setupVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  setupVisitor(const setupVisitor & rhs);
  virtual ~setupVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class setup_compVisitor : public Q93bVisitor {
public:

  setup_compVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  setup_compVisitor(const setup_compVisitor & rhs);
  virtual ~setup_compVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class call_procVisitor : public Q93bVisitor {
public:

  call_procVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  call_procVisitor(const call_procVisitor & rhs);
  virtual ~call_procVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class connectVisitor : public Q93bVisitor {
public:

  connectVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  connectVisitor(const connectVisitor & rhs);
  virtual ~connectVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class connect_ackVisitor : public Q93bVisitor {
public:

  connect_ackVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  connect_ackVisitor(const connect_ackVisitor & rhs);
  virtual ~connect_ackVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class releaseVisitor : public Q93bVisitor {
public:

  releaseVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  releaseVisitor(const releaseVisitor & rhs);
  virtual ~releaseVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class release_compVisitor : public Q93bVisitor {
public:

  release_compVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  release_compVisitor(const release_compVisitor & rhs);
  virtual ~release_compVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class status_enqVisitor : public Q93bVisitor {
public:

  status_enqVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  status_enqVisitor(const status_enqVisitor & rhs);
  virtual ~status_enqVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class statusVisitor : public Q93bVisitor {
public:

  statusVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  statusVisitor(const statusVisitor & rhs);
  virtual ~statusVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class add_partyVisitor : public Q93bVisitor {
public:

  add_partyVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  add_partyVisitor(const add_partyVisitor & rhs);
  virtual ~add_partyVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class add_party_compVisitor : public Q93bVisitor {
public:

  add_party_compVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  add_party_compVisitor(const add_party_compVisitor & rhs);
  virtual ~add_party_compVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class add_party_ackVisitor : public Q93bVisitor {
public:

  add_party_ackVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  add_party_ackVisitor(const add_party_ackVisitor & rhs);
  virtual ~add_party_ackVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class add_party_rejVisitor : public Q93bVisitor {
public:

  add_party_rejVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  add_party_rejVisitor(const add_party_rejVisitor & rhs);
  virtual ~add_party_rejVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class drop_partyVisitor : public Q93bVisitor {
public:

  drop_partyVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  drop_partyVisitor(const drop_partyVisitor & rhs);
  virtual ~drop_partyVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class drop_party_compVisitor : public Q93bVisitor {
public:

  drop_party_compVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  drop_party_compVisitor(const drop_party_compVisitor & rhs);
  virtual ~drop_party_compVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class drop_party_ackVisitor : public Q93bVisitor {
public:

  drop_party_ackVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  drop_party_ackVisitor(const drop_party_ackVisitor & rhs);
  virtual ~drop_party_ackVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class leaf_setupVisitor : public Q93bVisitor {
public:

  leaf_setupVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  leaf_setupVisitor(const leaf_setupVisitor & rhs);
  virtual ~leaf_setupVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class restartVisitor : public Q93bVisitor {
public:

  restartVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  restartVisitor(const restartVisitor & rhs);
  virtual ~restartVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class restart_respVisitor : public Q93bVisitor {
public:

  restart_respVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  restart_respVisitor(const restart_respVisitor & rhs);
  virtual ~restart_respVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

class restart_ackVisitor : public Q93bVisitor {
public:

  restart_ackVisitor(generic_q93b_msg *msg, Q93bVisitorType vt);
  restart_ackVisitor(const restart_ackVisitor & rhs);
  virtual ~restart_ackVisitor();
  bool CallProtocol(Call *c);

  virtual const VisitorType GetType(void) const;
  virtual const vistype & GetClassType(void) const;

protected:

  virtual Visitor * dup(void) const;

private:

  static vistype _my_type;
};

#endif // __Q93bVISITORS_H__
