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

#ifndef LINT
static char const _event_cc_rcsid_[] =
"$Id: event.cc,v 1.2 1998/08/06 04:04:53 bilal Exp $";
#endif
#include <common/cprototypes.h>
  VisitorType * vt2 = (VisitorType *)QueryRegistry(HELLO_VISITOR_NAME);
  if (vt2 && vt1.Is_A(vt2)) {// hello visitor
    delete vt2; vt2 = 0;
    DIAG(FSM_HELLO, DIAG_DEBUG, cout << GetName() << ": received a " << v->GetType() << " (" << v << ").\n";);

    // Until we receive a LinkUpVisitor, all Hellos are ignored
    if (_ds->_PortIsUP == 0) {
      DIAG(FSM_HELLO, DIAG_DEBUG, cout << GetName() << ": Port is not UP -- suiciding a " 
	   << v->GetType() << " (" << v << ").\n";);
    } else {
      HelloVisitor * hv = (HelloVisitor *)v;
      diag(FSM_HELLO, DIAG_DEBUG, " (vpi, vci) is (%d,%d)\n", 
	   hv->GetOutVP(), hv->GetOutVC());

      diag(FSM_HELLO, DIAG_DEBUG, "%s:\n    NHL: %x\n"
	                    "    AGGR: %d\n"
	                    "    ULIA: %x\n", OwnerName(), hv->_NHL, hv->_AGGR, hv->_ULIA);
      
      if (hv->GetOutVP() != _ds->_vpi || hv->GetOutVC()  != _ds->_vci) {
	// must be a hello for a LGN
	DIAG(FSM_HELLO, DIAG_DEBUG, cout << GetName() << " pass through a " << v->GetType() 
	     << " (" << v << ")" << endl;);
	return passthru;
      }
      // HelloMisMatch has precedence so we consider it first
      if (_ds->_cver != hv->_cver) {
	diag(FSM_HELLO, DIAG_DEBUG, "%s: version does not match: ds->_cver = %d hv->_cver = %d\n",
	     OwnerName(),_ds->_cver, hv->_cver);
	return (HelloMisMatch);
      }
      if (_ds->_RemNodeID && (!_ds->_RemNodeID->equals(hv->_NID))) {
	diag(FSM_HELLO, DIAG_DEBUG, "%s: RemNodeID does not match.\n", OwnerName());
	return (HelloMisMatch);
      }
      if (_ds->_RemPortID && (_ds->_RemPortID != hv->_PID)) {
	diag(FSM_HELLO, DIAG_DEBUG, "%s: RemPortID does not match.\n", OwnerName());
	return (HelloMisMatch);
      }
      if (_ds->_RemPeerGroupID && (!_ds->_RemPeerGroupID->equals(hv->_PGID))) {
	diag(FSM_HELLO, DIAG_DEBUG, "%s: RemPeerGroupID does not match.\n", 
	     OwnerName());
	return (HelloMisMatch);
      }
      // OneWayInside & TwoWayInside
      if (_ds->_LocPeerGroupID->equals(hv->_PGID)) {
	diag(FSM_HELLO, DIAG_DEBUG, "%s: PeerGroupIDs match.\n", OwnerName());
	
	if ((hv->_RNID == 0) && (hv->_RPID == 0)) {
	  diag(FSM_HELLO, DIAG_DEBUG, "%s: We are in OneWayInside.\n", OwnerName());
	  return (WayInside1);
	}
      
	if ((_ds->_cver == hv->_cver) &&
	    (_ds->_LocNodeID->equals(hv->_RNID))  &&
	    (_ds->_LocPortID == hv->_RPID)) {
	  diag(FSM_HELLO, DIAG_DEBUG, "%s: We are in TwoWayInside.\n", OwnerName());
	  return (WayInside2);
	}
	diag(FSM_HELLO, DIAG_DEBUG, "%s: We have a Hello Mismatch.\n", OwnerName());
	return (HelloMisMatch);
      }
      
      diag(FSM_HELLO, DIAG_DEBUG, "%s: PeerGroupIDs do not match.\n", OwnerName());
      
      // OneWayOutside
      if (hv->_RNID == 0 && (hv->_RPID == 0)) {
	diag(FSM_HELLO, DIAG_DEBUG, "%s: RNID = 0 & RPID = 0 : We are in OneWayOutside.\n",
	     OwnerName());
	return (WayOutside1);
      }
      // TwoWayOutside
      if (_ds->_LocNodeID->equals(hv->_RNID) && (_ds->_LocPortID==hv->_RPID)) {
	diag(FSM_HELLO, DIAG_DEBUG, "%s: RNID & RPID match what we have\n",OwnerName());
	ig_nodal_hierarchy_list * myNHL = _ds->_DB->GetNHL(_ds->_LocNodeID);
	if (myNHL)
	  cout << "My NHL: " << endl << *myNHL << endl;
	if (hv->_NHL) {
	  cout << "hv->_NHL: " << endl << *(hv->_NHL) << endl;
	  if ((level = hv->_NHL->FindCommonPGID(myNHL)))
	    cpgid = level->GetPID();
	  if (cpgid) {
	    DIAG(FSM_HELLO, DIAG_DEBUG, cout << OwnerName() << ": CommonPGID: " 
		 << *cpgid << " (" << cpgid << ").\n";);
	  } else {
	    DIAG(FSM_HELLO, DIAG_DEBUG, cout << OwnerName() << ": CommonPGID: " 
		 << " (" << cpgid << ").\n";);
	  }
	}
	if (hv->_NHL && (hv->_AGGR != -1) && hv->_ULIA) {
	  if ((level = hv->_NHL->FindCommonPGID(myNHL))) {
	    hv->_CPGID = level->GetPID();
	    hv->_UpNodeID = level->GetNID();		
	    hv->_UpNodeAddr = level->GetATM();
	  }
	  // NHL contains a common peer group id (cpgid) AND the
	  // CPGID, the UpNodeID, and UpNodeATMAddr in the DS are zero
	  // OR match the hp ones
	  if (cpgid) {
	    if (((_ds->_CommonPeerGroupID == 0) &&
		 (_ds->_UpNodeATMaddr == 0)     &&
		 (_ds->_UpNodeATMaddr == 0))    ||
		(_ds->_CommonPeerGroupID->equals(cpgid)  &&
		 (_ds->_UpNodeATMaddr == hv->_UpNodeAddr) &&
		 (_ds->_UpNodeID->equals(hv->_UpNodeID)))) {
	      diag(FSM_HELLO, DIAG_DEBUG, "%s: We are in CommonOutside.\n", 
		   OwnerName());
	      return (CommHier);
	    }
	  }
	  // OR NHL contains either a seq num
	  // that matches the received one && the Common Peer Group ID
	  // is set in DS
	  if ((hv->_NHL->GetSequenceNum() == _ds->_RcvNHLSeqNum) && 
	      _ds->_CommonPeerGroupID != 0) {
	    diag(FSM_HELLO, DIAG_DEBUG, "%s: We are in CommonOutside.\n", OwnerName());
	    return (CommHier);
	  }
	}
	// if the NHL has No common peer group OR has one that does not
	// match what we have OR the UpnodeID/UpnodeAddr associated with
	// the common peer group does not match we return a Hierarchy
	// MisMatch
	if ((cpgid == 0) && (hv->_UpNodeID == 0) && (hv->_UpNodeAddr == 0)) {
	  if (hv->_NHL)
	    if (cpgid)
	      return (HelloMisMatch);
	  diag(FSM_HELLO, DIAG_DEBUG, "%s: We are in TwoWayOutside.\n", OwnerName());
	  return (WayOutside2);
	}
	if (hv->_NHL) {
	  if (!cpgid || !cpgid->equals(_ds->_CommonPeerGroupID) ||
	      (_ds->_UpNodeATMaddr != hv->_UpNodeAddr) || 
	      !_ds->_UpNodeID->equals(hv->_UpNodeID))
	    return (HierMisMatch);
	}
      }
      rval = HelloMisMatch;
    }
    return rval;
  }
  rval = passthru;
  // TREAT ALL OTHER CASES HERE
  delete vt2; vt2 = 0;
  vt2 = (VisitorType *)QueryRegistry(DB_INTRO_VISITOR_NAME);
  if (vt2 && vt1.Is_A(vt2))
    DIAG(FSM_HELLO, DIAG_DEBUG, cout << OwnerName() << " received a " << v->GetType() << " (" << v << ").\n";);
    // Shouldn't you copy the DBIntroVisitor's DB pointer?
  delete vt2; vt2=0;
  vt2 = (VisitorType *)QueryRegistry(LINK_UP_VISITOR_NAME);
  if (vt2 && vt1.Is_A(vt2)) {
    DIAG(FSM_HELLO, DIAG_DEBUG, cout << OwnerName() << " received a " << v->GetType() << " (" << v << ").\n";);
    // need to deal with multiple LinkUpVisitors
    if (_ds->_PortIsUP != 1) {
      _ds->_PortIsUP = 1;
      rval = LinkIsUp;
    }
  }
  delete vt2;vt2=0;
  vt2 = (VisitorType *)QueryRegistry(LINK_DOWN_VISITOR_NAME);
  if (vt2 && vt1.Is_A(vt2)) {
    DIAG(FSM_HELLO, DIAG_DEBUG, cout << 
	 OwnerName() << " received a " << v->GetType() << " (" << v << ").\n";);
    _ds->_PortIsUP = 0;
    rval = LinkIsDown;
  }
  delete vt2;vt2=0;
  vt2 = (VisitorType *)QueryRegistry(NPSTATEVISITOR_NAME);
  if (vt2 && vt1.Is_A(vt2)) {
    DIAG(FSM_HELLO, DIAG_DEBUG, cout << 
	 OwnerName() << " received a " << v->GetType() << " (" << v << ").\n";);
    rval = Invalid;
  }
  delete vt2; vt2 = 0;

  return rval;
