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
#ifndef __CRANKBACK_H__
#define __CRANKBACK_H__

#ifndef LINT
static char const _PNNI_crankback_h_rcsid_[] =
"$Id: PNNI_crankback.h,v 1.10 1998/02/24 21:47:02 talmage Exp $";
#endif

#include <codec/uni_ie/ie_base.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/errmsg.h>

class NodeID;

/** Crankback indicates that crankback procedures have been initiated.
 * It also indicates the node or link where the call/connection or
 * party cannot be accepted, and the level of PNNI hierarchy at which
 * crankback is being carried out. p 203.  */

class PNNI_crankback : public InfoElem {
  friend ostream & operator << (ostream & os, PNNI_crankback & x);
public:

  enum BlockedTransitTypes {
    SucceedingEndOfInterface = 2,
    BlockedNode = 3,
    BlockedLink = 4
  };

  enum CrankbackCauses {
    TransitNetworkUnreachable                 = 2,
    DestinationUnreachable                    = 3,
    TooManyPendingAddPartyRequests            = 32,
    RequestVCINotAvailable                    = 35,
    UserCellRateNotAvailable                  = 37,
    NetworkOutOfOrder                         = 38,
    TemporaryFailure                          = 41,
    NoVCIAvailable                            = 45,
    ResourceUnavailableUnspecified            = 47,
    QualityOfServiceUnavailable               = 49,
    BearerCapabilityNotAuthorized             = 57,
    BearerCapabilityNotPresentlyAvailable     = 58,
    ServiceOrOptionNotAvailableUnSpecified    = 63,
    BearerServiceNotImplemented               = 65,
    UnsupportedCombinationOfTrafficParameters = 73,
    NextNodeUnreachable                       = 128,
    DTLTransitNotMyNodeID                     = 160
  };

  enum Dir {
    forward  = 0,
    backward = 1
  };


  PNNI_crankback(void);

  PNNI_crankback(u_char crankback_level, 
		 BlockedTransitTypes blocked_transit_type, 
		 CrankbackCauses cause,
		 const NodeID * blocked_node = 0,
		 u_int blocked_port = 0, 
		 const NodeID * blocked_end_node = 0 );


  PNNI_crankback(const PNNI_crankback & rhs);

  virtual ~PNNI_crankback( );

  PNNI_crankback * copy(void);

  InfoElem *copy(void) const;

  int                 encode(u_char *buffer);
  InfoElem::ie_status decode(u_char *buffer, int & id);

  int operator == (const PNNI_crankback & rhs) const;
  int equals(const InfoElem * him) const;
  int equals(const PNNI_crankback * himptr) const;

  void SetCauseDiagnostic(Dir direction, int port_id, int AvCR, 
			  int CRM, int VF);
  void SetCauseDiagnostic(int CTD, int CDV, int CLR, int QoS);

  BlockedTransitTypes GetBlockedTransitType(void) const;
  CrankbackCauses     GetCrankbackCause(void) const;
  const u_char      * GetBlockedTransitID(void) const;
  const u_char        GetBlockedLevel(void) const;

  /** Returns a new NodeID that must be deleted. */
  NodeID * GetBlockedNode(void);

  /** Returns a new NodeID that must be deleted. */
  NodeID * GetStartNode(void);

  /** Returns a new NodeID that must be deleted. */
  NodeID * GetEndNode(void);

  u_int GetBlockedPort(void);

  int Length();

protected:

  virtual char PrintSpecific(ostream & os);

private:

  u_char                _level;
  BlockedTransitTypes   _blocked_transit_type;
  u_char              * _blocked_transit_id;
  CrankbackCauses       _cause;

  // Optional Cause Diagnostic
  Dir     _direction;
  u_int   _port_id;
  u_int   _avcr;
  u_int   _crm;
  u_int   _vf;
};

#endif // __CALLED_PARTY_SOFT_PVPC_H__
