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

/** -*- C++ -*-
 * File: FastUniVisitor.h
 * @author marsh
 * @version $Id: FastUNIVisitor.h,v 1.75 1999/02/09 17:26:00 mountcas Exp $
 *
 *
 * BUGS:
 */
#ifndef __FASTUNIVISITOR_H__
#define __FASTUNIVISITOR_H__

#ifndef LINT
static char const _FastUNIVisitor_h_rcsid_[] =
"$Id: FastUNIVisitor.h,v 1.75 1999/02/09 17:26:00 mountcas Exp $";
#endif

#include <fsm/visitors/VPVCVisitor.h>
#include <DS/containers/list.h>

class NodeID;
class PNNI_crankback;
class generic_q93b_msg;
class q93b_setup_message;
class q93b_release_message;
class ie_cause;
class ig_uplinks;
class Q93bVisitor;
class DTLContainer;

#define FAST_UNI_VISITOR_NAME "FastUNIVisitor"

class FastUNIVisitor : public VPVCVisitor {
  friend ostream & operator << (ostream & os, const FastUNIVisitor & fuv);
  friend class Translator;
public:

  static const u_long DefaultBandwidthReq = 47000;

  enum uni_message {
    Unknown                = 0,
    FastUNITunneling,
    FastUNISetup,
    FastUNICallProceeding,
    FastUNIRelease,
    FastUNIConnect,
    FastUNILHI,            // LgnHelloInstantiator
    FastUNILHIRebinder,    // see above
    FastUNILHD,            // LgnHelloDestroyer

    FastUNIUplinkResponse  = 128,
    FastUNISetupFailure,

    RemotePGLChanged       = 255,
    PGLChanged,
    NewUplink,
    UplinkDown,
    UpGrade
  };

  /**
   * Constructor
   *
   * Values for dtl_filename: 
   * 	0       ==> don't print the DTL on death
   * 	""      ==> print the DTL to cout on death
   *   filename ==> print the DTL to filename on death
   */
  FastUNIVisitor(const NodeID * from, const NodeID * to,
		 int port = -1, int vpvc = -1, int agg = -1,
		 int cref = 0, uni_message type = FastUNISetup, 
		 char * dtl_filename = 0, 
		 u_long bandwidth_req = DefaultBandwidthReq);

  FastUNIVisitor(const FastUNIVisitor & rhs);

  NodeID * GetBorder(void) const;
  State  * GetState(void) const;  
  int      GetBorderPort(void) const;

  void     SetBorder(NodeID * nid);
  void     SetBorderPort(int port);
  void     SetState(State * state);

  void         SetMSGType(uni_message msg);
  uni_message  GetMSGType(void) const;
  const char * PrintMSGType(void) const;

  ig_uplinks * GetULIG(void) const;
  void         SetULIG(ig_uplinks * ulig);

  virtual const VisitorType GetType(void) const;

  const DTLContainer * Peek(void) const;
        DTLContainer * Pop(void);
  void                 Push(const NodeID * nid, int port = 0);
  void                 Append(const NodeID * nid, int port = 0);
  void                 Push(const list<NodeID *> * lptr);
  void                 Push(const list<DTLContainer *> * lptr);

  q93b_setup_message * GetSetup(void) const;
  void                 SetSetup(q93b_setup_message * msg);
  void                 SetSetup(u_long bandwidth, int flags = 0);

  q93b_release_message * GetRelease(void) const;
  void                   SetRelease(q93b_release_message * msg);
  void                   SetRelease(const ie_cause * c, const PNNI_crankback * crank = 0, int flags = 0);

  generic_q93b_msg * TakeMessage(void);
  bool               SetMessage(generic_q93b_msg *& msg);
  const generic_q93b_msg * GetSharedMessage(void) const;

  PNNI_crankback * GetCrankback(void) const;
  ie_cause       * GetCause(void) const;

  //
  // FastUNIVisitor thinks it owns crank.
  // Don't delete crank after calling SetCrankback().
  //
  void SetCrankback(const PNNI_crankback * crank);

  //
  // FastUNIVisitor thinks it owns cause.
  // Don't delete cause after calling SetCause().
  //
  void SetCause(const ie_cause * cause);

  // For RouteControl and Logos only
  list<PNNI_crankback *> * GetCrankbackList(void);
  void                     SetCrankbackList(const list<PNNI_crankback *> * lst);

  FastUNIVisitor & operator = (const FastUNIVisitor & rhs);

  void ClearDTL(void);

  // redefines VPVCVisitor's CREF
  void   SetCREF(u_long cref);
  u_long GetCREF(void) const;

  bool IsForSVCC(void) const;
  void SetForSVCC(bool s);

  void SetLogicalPort(int lp);
  int  GetLogicalPort(void) const;

protected:

  virtual Visitor * dup(void) const;

  virtual ~FastUNIVisitor();
  virtual void on_death(void) const;

  uni_message    _message_type;
  // What are these three for?
  NodeID       * _border;
  int            _border_port;
  State        * _state;
  // What are the above for?
  ig_uplinks   * _uplink;
  u_long         _bandwidth;
  char         * _dtl_filename;

  // For DTL
  list<DTLContainer *> _dtl;
  // Holds ID's of visited nodes as we traverse the DTL
  list<DTLContainer *> _hops;

  // For RouteControl and Logos only
  list<PNNI_crankback *> * _lst;
  bool                     _is_for_svcc;
  int                      _lp;

  bool SetTunnelingPayload(Q93bVisitor *& v);
  Q93bVisitor * TakeTunnelingPayload(void);

  void PrintDTL(ostream & os) const;

private:

  void BuildTheQ93bMessage(uni_message type, u_long cref);

  generic_q93b_msg * _message;

  Q93bVisitor * _tunneling_payload;

  virtual const vistype & GetClassType(void) const { return _my_type; }

  void Fill_Subtype_Name(char * buf) const;

  static vistype _my_type;
};

#endif // __FASTUNIVISITOR_H__

