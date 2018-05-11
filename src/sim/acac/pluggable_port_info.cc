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

/* -*- C++ -*-
 * File: pluggable_port_info.cc
 * Author: 
 * Version: $Id: pluggable_port_info.cc,v 1.89 1999/02/26 19:39:39 mountcas Exp $
 * Purpose: ACAC
 * BUGS:
 */

#ifndef LINT
static char const _pluggable_port_info_cc_rcsid[] =
"$Id: pluggable_port_info.cc,v 1.89 1999/02/26 19:39:39 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include "ACACPolicy.h"
#include "ACAC_DIAG.h"
#include "pluggable_port_info.h"
#include <sim/acac/call_identifier.h>
#include <FW/basics/diag.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_ig/uplink_info_attr.h>
#include <codec/q93b_msg/setup.h>
#include <codec/q93b_msg/release.h>
#include <codec/uni_ie/PNNI_crankback.h>
#include <codec/uni_ie/UNI40_td.h>
#include <codec/uni_ie/UNI40_bbc.h>
#include <codec/uni_ie/addr.h>
#include <codec/uni_ie/cause.h>
#include <fsm/nodepeer/NPVisitors.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fstream.h>
#include <iostream.h>
#include <string.h>

/*
 * See the UNI 3.1 book, pages 185-187 for the justification of these values.
 *
 * BUG- The global call reference has a value of zero.
 * BUG- The dummy call reference has a value of 0x7FFFFF or 0xFFFFFF
 */
int pluggable_port_info::_min_call_reference = 0x000001;
int pluggable_port_info::_max_call_reference = 0x7FFFFF;


//
// What is the service category of this call?  Determine it by looking
// at the Broadband bearer class in the BBC, the ATM transfer
// capability (ATC) in the BBC, and the Best effort indicator in the
// ATM Traffic Descriptor.
//
// See also the chart on pages 103 and 104 of af-sig-0061.000, ATM UNI
// Signalling, Version 4.0.
//
// BUG- Doesn't recognize when the GCAC_CLP bit must be set.
u_int ServiceCategory(const ie_bbc * const bbc, 
		      const UNI40_traffic_desc * const td)
{
  u_int answer = RAIG_FLAG_UNSUPPORTED;
  ie_bbc::Bbc_class  bbc_class    = bbc->get_qos_class();
  ie_bbc::trans_caps atm_transfer_capability = bbc->get_ATC();
  bool best_effort_indicator     = td->get_BE();

  switch (bbc_class) {
    case ie_bbc::BCOB_A:
      switch (atm_transfer_capability) {
        case ie_bbc::ATC_Absent:
        case ie_bbc::CBR_CLR:
	  switch (best_effort_indicator) {
	    case true:
	      DIAG(SIM_ACAC, DIAG_DEBUG, 
		   cout << "Undefined service category BBC class=0x" 
		   << hex << (int) bbc_class
		   << dec 
		   << " ATC= " << atm_transfer_capability 
		   << " BEI= " << best_effort_indicator << endl);
	      break;
	    case false:
	      answer = RAIG_FLAG_CBR;
	      break;
	  }
	  break;
        default:
	  DIAG(SIM_ACAC, DIAG_DEBUG, 
	       cout << "Undefined service category BBC class=0x" 
	       << hex << (int) bbc_class
	       << dec 
	       << " ATC= " << atm_transfer_capability 
	       << " BEI= " << best_effort_indicator << endl);
	  break;
      }
      break;
      
    case ie_bbc::BCOB_C:
      switch (atm_transfer_capability) {
        case ie_bbc::ATC_Absent:
	  switch (best_effort_indicator) {
	    case true:
	      answer = RAIG_FLAG_UBR;
	      break;
	    case false:
	      answer = RAIG_FLAG_NRTVBR;
	      break;
	  }
	  break;
        case ie_bbc::NonRT_VBR_CLR:
	  switch (best_effort_indicator) {
	    case true:
	      DIAG(SIM_ACAC, DIAG_DEBUG, 
		   cout << "Undefined service category BBC class=0x" 
		   << hex << (int) bbc_class
		   << dec 
		   << " ATC= " << atm_transfer_capability 
		   << " BEI= " << best_effort_indicator << endl);
	      break;
	    case false:
	      answer = RAIG_FLAG_NRTVBR;
	      break;
	  }
	  break;
        case ie_bbc::ABR:
	  switch (best_effort_indicator) {
	    case true:
	      DIAG(SIM_ACAC, DIAG_DEBUG, 
		   cout << "Undefined service category BBC class=0x"
		   << hex << (int) bbc_class
		   << dec 
		   << " ATC= " << atm_transfer_capability 
		   << " BEI= " << best_effort_indicator << endl);
	      break;
	    case false:
	      answer = RAIG_FLAG_ABR;
	      break;
	  }
	  break;
        case ie_bbc::RT_VBR:
        case ie_bbc::RT_VBR_CLR:
	  switch (best_effort_indicator) {
	    case true:
	      DIAG(SIM_ACAC, DIAG_DEBUG, 
		   cout << "Undefined service category BBC class=0x" 
		   << hex << (int) bbc_class
		   << dec 
		   << " ATC= " << atm_transfer_capability 
		   << " BEI= " << best_effort_indicator << endl);
	      break;
	    case false:
	      answer = RAIG_FLAG_RTVBR;
	      break;
	  }
      }
      break;
      
    case ie_bbc::BCOB_X:
      switch (atm_transfer_capability) {
        case ie_bbc::ATC_Absent:
        case ie_bbc::NonRT_VBR_0:
        case ie_bbc::NonRT_VBR_2:
        case ie_bbc::NonRT_VBR_8:
        case ie_bbc::NonRT_VBR:
	  switch (best_effort_indicator) {
	    case true:
	      answer = RAIG_FLAG_UBR;
	      break;
	    case false:
	      answer = RAIG_FLAG_NRTVBR;
	      break;
	  }
	  break;
        case ie_bbc::NonRT_VBR_CLR:
	  switch (best_effort_indicator) {
	    case true:
	      DIAG(SIM_ACAC, DIAG_DEBUG, 
		   cout << "Undefined service category BBC class=0x" 
		   << hex << (int) bbc_class
		   << dec 
		   << " ATC= " << atm_transfer_capability 
		   << " BEI= " << best_effort_indicator << endl);
	      break;
	    case false:
	      answer = RAIG_FLAG_NRTVBR;
	      break;
	  }
	  break;
        case ie_bbc::ABR:
	  switch (best_effort_indicator) {
	    case true:
	      DIAG(SIM_ACAC, DIAG_DEBUG, 
		   cout << "Undefined service category BBC class=0x" 
		   << hex << (int) bbc_class
		   << dec 
		   << " ATC= " << atm_transfer_capability 
		   << " BEI= " << best_effort_indicator << endl);
	      break;
	    case false:
	      answer = RAIG_FLAG_ABR;
	      break;
	  }
	  break;
        case ie_bbc::RT_VBR_1:
        case ie_bbc::RT_VBR:
        case ie_bbc::RT_VBR_CLR:
	  switch (best_effort_indicator) {
	    case true:
	      DIAG(SIM_ACAC, DIAG_DEBUG, 
		   cout << "Undefined service category BBC class=0x" 
		   << hex << (int) bbc_class
		   << dec 
		   << " ATC= " << atm_transfer_capability 
		   << " BEI= " << best_effort_indicator << endl);
	      break;
	    case false:
	      answer = RAIG_FLAG_RTVBR;
	      break;
	  }
	  break;
        case ie_bbc::CBR_4:
        case ie_bbc::CBR:
        case ie_bbc::CBR_6:
        case ie_bbc::CBR_CLR:
	  switch (best_effort_indicator) {
	    case true:
	      DIAG(SIM_ACAC, DIAG_DEBUG, 
		   cout << "Undefined service category BBC class=0x" 
		   << hex << (int) bbc_class
		   << dec 
		   << " ATC= " << atm_transfer_capability 
		   << " BEI= " << best_effort_indicator << endl);
	      break;
	    case false:
	      answer = RAIG_FLAG_CBR;
	      break;
	  }
	  break;
        default:
	  DIAG(SIM_ACAC, DIAG_DEBUG, 
	       cout << "Undefined service category BBC class=0x" 
	       << hex << (int) bbc_class
	       << dec 
	       << " ATC= " << atm_transfer_capability 
	       << " BEI= " << best_effort_indicator << endl);
	  break;
      }
      break;
      
    case ie_bbc::BCOB_VP:
      switch (atm_transfer_capability) {
        case ie_bbc::ATC_Absent:
        case ie_bbc::NonRT_VBR:      
	  switch (best_effort_indicator) {
	    case true:
	      answer = RAIG_FLAG_UBR;
	      break;
	    case false:
	      answer = RAIG_FLAG_NRTVBR;
	      break;
	  }
	  break;
        case ie_bbc::NonRT_VBR_CLR:
	  switch (best_effort_indicator) {
	    case true:
	      DIAG(SIM_ACAC, DIAG_DEBUG, 
		   cout << "Undefined service category BBC class=0x" 
		   << hex << (int) bbc_class
		   << dec 
		   << " ATC= " << atm_transfer_capability 
		   << " BEI= " << best_effort_indicator << endl);
	      break;
	    case false:
	      answer = RAIG_FLAG_NRTVBR;
	      break;
	  }
	  break;
        case ie_bbc::ABR:
	  switch (best_effort_indicator) {
	    case true:
	      DIAG(SIM_ACAC, DIAG_DEBUG,
		   cout << "Undefined service category BBC class=0x" 
		   << hex << (int) bbc_class
		   << dec 
		   << " ATC= " << atm_transfer_capability 
		   << " BEI= " << best_effort_indicator << endl);
	      break;
	    case false:
	      answer = RAIG_FLAG_ABR;
	      break;
	  }
	  break;
        case ie_bbc::RT_VBR:
        case ie_bbc::RT_VBR_CLR:
	  switch (best_effort_indicator) {
	    case true:
	      DIAG(SIM_ACAC, DIAG_DEBUG, 
		   cout << "Undefined service category BBC class=0x" 
		   << hex << (int) bbc_class
		   << dec 
		   << " ATC= " << atm_transfer_capability 
		   << " BEI= " << best_effort_indicator << endl);
	      break;
	    case false:
	      answer = RAIG_FLAG_RTVBR;
	      break;
	  }
	  break;
        case ie_bbc::CBR:
        case ie_bbc::CBR_CLR:
	  switch (best_effort_indicator) {
	    case true:
	      DIAG(SIM_ACAC, DIAG_DEBUG, 
		   cout << "Undefined service category BBC class=0x" 
		   << hex << (int) bbc_class
		   << dec 
		   << " ATC= " << atm_transfer_capability 
		   << " BEI= " << best_effort_indicator << endl);
	      break;
	    case false:
	      answer = RAIG_FLAG_CBR;
	      break;
	  }
        default:
	  DIAG(SIM_ACAC, DIAG_DEBUG, 
	       cout << "Undefined service category BBC class=0x"
	       << hex << (int) bbc_class
	       << dec 
	       << " ATC= " << atm_transfer_capability 
	       << " BEI= " << best_effort_indicator << endl);
	  break;
      }
      break;
      
    default:
      DIAG(SIM_ACAC, DIAG_DEBUG, 
	   cout << "Unknown Broadband bearer class=0x" 
	   << hex << (int) bbc_class
	   << dec 
	   << " ATC= " << atm_transfer_capability 
	   << " BEI= " << best_effort_indicator << endl);
      break;
  }
  
  return answer;
}


//
// PNNI 1.0 section 2.5.2.3.4 p. 214 refers indirectly to UNI 4.0
// 8.1.2.1 pp. 60-61.  The alternative atm traffic descriptor may have
// the Best Effort Indicator if and only if the atm traffic descriptor
// has it.  Furthermore, if the bandwidth required by the alternative
// is greater than that of the atm traffic descriptor then the element
// may be treated as a non-manditory information element with content
// error.
//
// An Alternative ATM Traffic Descriptor is valid for CBR and VBR
// calls.
//
bool validate_traffic_descriptors(const u_int service_category,
				  const ig_resrc_avail_info * const theRAIG, 
				  const UNI40_traffic_desc * const td,
				  const UNI40_alt_traffic_desc * const alt_td)
{
  bool answer = false;
  bool td_has_best_effort_indicator = td->get_BE();
  bool alt_td_has_best_effort_indicator = alt_td->get_BE();

  if ((service_category == RAIG_FLAG_CBR) ||
      (service_category == RAIG_FLAG_RTVBR) ||
      (service_category == RAIG_FLAG_NRTVBR)) {
    //
    // alt td may have a best effort indicator only if the td has it.
    //
    if ((alt_td_has_best_effort_indicator) && !td_has_best_effort_indicator)
      answer = false;
    else {
      //
      // alt_td's bandwidth must be less than or equal to
      // td's bandwidth.
      //
      if (// Peak Cell Rate
	  (alt_td->get_FPCR_0() <= td->get_FPCR_0()) &&
	  (alt_td->get_BPCR_0() <= td->get_BPCR_0()) &&
	  (alt_td->get_FPCR_01() <= td->get_FPCR_01()) &&
	  (alt_td->get_BPCR_01() <= td->get_BPCR_01()) &&

	  // Sustained Cell Rate
	  (alt_td->get_FSCR_0() <= td->get_FSCR_0()) &&
	  (alt_td->get_BSCR_0() <= td->get_BSCR_0()) &&
	  (alt_td->get_FSCR_01() <= td->get_FSCR_01()) &&
	  (alt_td->get_BSCR_01() <= td->get_BSCR_01()) &&

	  // Maximum Burst Size
	  (alt_td->get_FMBS_0() <= td->get_FMBS_0()) &&
	  (alt_td->get_BMBS_0() <= td->get_BMBS_0()) &&
	  (alt_td->get_FMBS_01() <= td->get_FMBS_01()) &&
	  (alt_td->get_BMBS_01() <= td->get_BMBS_01()) &&

	  // ABR Minimum Cell Rate
	  (alt_td->get_FMCR_01() <= td->get_FMCR_01()) &&
	  (alt_td->get_BMCR_01() <= td->get_BMCR_01()) )

	answer = true;
    }
  }

  return answer;
}

//
// UNI 4.0 pp. 61-62
//
bool validate_traffic_descriptors(const u_int service_category,
				  const ig_resrc_avail_info * const theRAIG, 
				  const UNI40_traffic_desc * const td,

				  const UNI40_min_traffic_desc * const min_td)
{
  bool answer = false;

  if ((service_category == RAIG_FLAG_CBR) ||
      (service_category == RAIG_FLAG_RTVBR) ||
      (service_category == RAIG_FLAG_NRTVBR) ||
      (service_category == RAIG_FLAG_ABR) ||
      (service_category == RAIG_FLAG_UBR)) {

    if (service_category != RAIG_FLAG_UBR) {
      // rules for UBR from ATM Forum Traffic Management Version 4.0

      answer = true;
    } else {
      answer = true;

      // min_td can have a Peak Cell Rate parameter only if the
      // corresponding one is in the td.
      if (min_td->IsSet(UNI40_min_traffic_desc::FPCR_0)) {
	if (!(td->IsSet(PNNI_crankback::forward, 
		       UNI40_traffic_desc::PCR_0)) ||
	    (min_td->get_FPCR_0() > td->get_FPCR_0()))
	  answer = false;
      }

      if (min_td->IsSet(UNI40_min_traffic_desc::BPCR_0)) {
	if (!(td->IsSet(PNNI_crankback::backward, 
		       UNI40_traffic_desc::PCR_0)) ||
	    (min_td->get_BPCR_0() > td->get_BPCR_0()))
	  answer = false;
      }

      if (min_td->IsSet(UNI40_min_traffic_desc::FPCR_01)) {
	if (!(td->IsSet(PNNI_crankback::forward, 
		       UNI40_traffic_desc::PCR_01)) ||
	    (min_td->get_FPCR_01() > td->get_FPCR_01()))
	  answer = false;
      }

      if (min_td->IsSet(UNI40_min_traffic_desc::BPCR_01)) {
	if (!(td->IsSet(PNNI_crankback::backward, 
		       UNI40_traffic_desc::PCR_01)) ||
	    (min_td->get_BPCR_01() > td->get_BPCR_01()))
	  answer = false;
      }

    }
  }
  return answer;
}


pluggable_port_info::pluggable_port_info(ACACPolicy *sharedACACPolicy,
		     bool up, enum port_type type, 
		     NPStateVisitor::NPStateVisitorType npstate, 
		     int id, u_int localPort, u_int remotePort, 
		     NodeID *remoteNode, 
		     PeerID *commonPeerGroup, 
		     int aggregationToken) :
  _sharedACACPolicy(sharedACACPolicy),
  _up(up), _type(type), 
  _npstate(npstate), 
  _id(id), 
  _localNode(0), _localPort(localPort), 
  _remoteNode(0), _remotePort(remotePort), 
  _commonPeerGroup(0), 
  _aggregationToken(aggregationToken), 
  _dirty(true), _sequenceNumber(1), _remoteAddress(0),
  _call_reference(_min_call_reference - 1),
  _theULIA(0)
{
  if (commonPeerGroup != 0) {
    _commonPeerGroup = new PeerID(*commonPeerGroup);
  }

  if (remoteNode != 0) {
    _remoteNode = new NodeID(*remoteNode);
    _remoteAddress = _remoteNode->GetAddr();	// It's safe to delete this.
  }
}

pluggable_port_info::~pluggable_port_info(void)
{
  _sharedACACPolicy = 0;

  delete _commonPeerGroup;
  delete _localNode;
  delete _remoteNode;
  delete _remoteAddress;

  ClearCalls();
  ClearRAIGs();
}


void pluggable_port_info::ClearRAIGs(void)
{
  dic_item ditem = 0;
  ig_resrc_avail_info *oldRAIG =0;

  forall_items(ditem, _theRAIGs) {
    oldRAIG = _theRAIGs.inf(ditem);
    _theRAIGs.change_inf(ditem, 0);
    delete oldRAIG;
  }
  _theRAIGs.clear();
}


//
// Release the bandwidth reserved by all calls.
// Empty the _Calls dictionary.
//
#define  CREF_FLAG  0x800000
void pluggable_port_info::ClearCalls(void)
{
  iport_oport_td *thing = 0;
  int cref = 0;
  FastUNIVisitor *release = 0;
  dic_item ditem;

  forall_items(ditem, _Calls) {
    cref = _Calls.key(ditem);
    thing = _Calls.inf(ditem);

    FreeBandwidth(cref);

    // Why is this disabled?  Why can't I delete thing?
    //    delete thing;
  }
  _Calls.clear();
}


/* Get the type of the port */
void pluggable_port_info::PortType(enum port_type type)
{
  _type = type;
  _dirty = true;
}


/* Set the type of the port */
enum pluggable_port_info::port_type pluggable_port_info::PortType(void)
{
  return _type;
}


/* Get the port's ID.  There may or may not be a correspondence
 * between the port number and the port ID.  
 */
int pluggable_port_info::ID(void)
{
  return _id;
}

void pluggable_port_info::LocalNode(NodeID * localNode)
{
  if (_localNode != 0 && _localNode != localNode) {
    delete _localNode;
    _localNode = 0;
  }

  if (localNode != 0) {
    _localNode = new NodeID(*localNode);
  }

  _dirty = true;
}


/* Get the NodeID of the local host or switch. */
NodeID *pluggable_port_info::LocalNode(void) const
{
  return _localNode;
}


/*
 * What is the number of the port?  This is different from the port ID.
 */
u_int pluggable_port_info::LocalPort(void)
{
  return _localPort;
}


/* Set the number of the port on the remote switch or host. */
void pluggable_port_info::RemotePort(u_int remotePort)
{
  _remotePort = remotePort;
  _dirty = true;
}


/* Get the number of the remote port */
u_int pluggable_port_info::RemotePort(void)
{
  return _remotePort;
}


/* Se the ATM address of the remote host or switch */
void pluggable_port_info::RemoteAddress(Addr * remoteAddress)
{
  if (_remoteAddress != 0 && _remoteAddress != remoteAddress) {
    delete _remoteAddress;
    _remoteAddress = 0;
  }

  if (remoteAddress != 0)
    _remoteAddress = remoteAddress->copy();

  _dirty = true;
}


/* Get the ATM address of the remote host or switch */
Addr *pluggable_port_info::RemoteAddress(void) const
{
  return _remoteAddress;
}


/* Set the NodeID of the remote host or switch.
 * This also sets the ATM address of the remote host or switch.
 */
void pluggable_port_info::RemoteNode(const NodeID * remoteNode)
{
  if (_remoteNode != 0 && _remoteNode != remoteNode) {
    delete _remoteNode;
    _remoteNode = 0;

    delete _remoteAddress;
    _remoteAddress = 0;
  }

  if (remoteNode != 0) {
    _remoteNode = new NodeID(*remoteNode);
    _remoteAddress = _remoteNode->GetAddr();	// It's safe to delete this.
  }

  _dirty = true;
}


/* Get the NodeID of the remote host or switch. */
NodeID *pluggable_port_info::RemoteNode(void) const
{
  return _remoteNode;
}


/* Set the PeerID of that this node shares with the remote node.
 * This node and the remote node are border nodes.
 */
void pluggable_port_info::CommonPeerGroup(const PeerID *commonPeerGroup)
{
  if (_commonPeerGroup != 0 && _commonPeerGroup != commonPeerGroup) {
    delete _commonPeerGroup;
    _commonPeerGroup = 0;
  }

  if (commonPeerGroup != 0) {
    _commonPeerGroup = new PeerID(*commonPeerGroup);
  }

  _dirty = true;
}


/* Get the peer group ID.
 */
PeerID *pluggable_port_info::CommonPeerGroup(void) const
{
  return _commonPeerGroup;
}    


/* Set the aggregation token */
void pluggable_port_info::AggregationToken(int aggregationToken)
{
  _aggregationToken = aggregationToken;
  _dirty = true;
}


/* Get the aggregation token. */
int pluggable_port_info::AggregationToken(void)
{
  return _aggregationToken;
}


/*
 * Set the status of the NodePeer that controls this port.
 * CACRoutingFSM knows to originate PTSE's only when the port is
 * in FullState.
 */
void pluggable_port_info::NPState(NPStateVisitor::NPStateVisitorType npstate)
{
  _npstate = npstate;
  _dirty = true;
}


/*
 * Get the status of the NodePeer that controls this port.
 */
NPStateVisitor::NPStateVisitorType pluggable_port_info::NPState(void)
{
  return _npstate;
}


/*
 * Set the physical state of the port.  Use true for 'the port is up'. 
 * Use false for 'the port is down'.
 */
void pluggable_port_info::Up(bool up)
{
  _up = up;
  _dirty = true;

  //
  // When a port goes down, release all of its calls. 
  if (!up) {
    ClearCalls();
  }

}


/* Get the physical state of the port. */
bool pluggable_port_info::Up(void)
{
  return _up;
}


/* Tell the port whether or not its state has changed.
 * true means the state has changed.  false means it hasn't.
 */
void pluggable_port_info::Dirty(bool dirty)
{
  _dirty = dirty;
}


/* Ask the port whether its state has changed. */
bool pluggable_port_info::Dirty(void)
{
  return _dirty;
}


//
// Find the next available call reference value that is greater than
// or equal to _min_call_reference but less than or equal to
// _max_call_reference.  Return zero if there is no available call
// reference.
//
// When there are a lot of calls, NextCallReference() performs a
// linear search for the next available one.  This is a Bad Thing.
// Instead, it should allocate a range of call references all at once,
// giving them out as required.  It should remember which ones are
// returned and put them back into the free list after a suitable
// period of time.  By waiting before reusing the call references,
// NextCallReference() gives the network time to forget that it has
// already seen them.
//
// Another approach to call reference allocation is a CREFAllocator
// object that runs in another thread.  The CREFAllocator will know
// when it must generate another sequence of unused call reference
// values.  The crvs must be in the range 1..0x7fffffff, inclusive.
//
u_int pluggable_port_info::NextCallReference(void)
{
  dic_item ditem = 0;
  u_int upper_range = _call_reference + 1;
  u_int lower_range = _min_call_reference;
  u_int answer = 0;	// No call reference available.

  //
  // Do a linear search of the upper range, _call_reference + 1
  // through _max_call_reference, for an unused call reference value.
  // There isn't one in the range if _upper_range exceeds
  // _max_call_reference.
  //
  while ((upper_range <= _max_call_reference) &&
	 ((ditem = _Calls.lookup(upper_range)) != 0))
      upper_range++;

  //
  // Do a linear search the lower range, _min_call_reference to
  // _call_reference - 1, for an unused call reference value.  There
  // isn't one in the range if _lower_range equals _call_reference.
  //
  if (upper_range > _max_call_reference) {
    while (((ditem = _Calls.lookup(lower_range)) != 0) &&
	   (lower_range < _call_reference))
      lower_range++;

    if (lower_range < _call_reference)
      answer = _call_reference = lower_range;

  } else answer = _call_reference = upper_range;

  return answer;
}


//
// Make entries in _Calls for a new call.
// Don't do the bandwidth arithmetic if there are no RAIGs or
// no traffic descriptor.
//
pluggable_port_info::ReserveBandwidth(u_int inPort, u_int outPort,
				      u_int inCREF, u_int outCREF,
				      u_int inVPI, u_int inVCI,
				      u_int service_category,
				      const ie_bbc *bbc,
				      const UNI40_traffic_desc *td,
				      const NodeID *called_party,
				      ig_resrc_avail_info *theRAIG,
				      ACACPolicy::Direction direction)
{
  //
  // Yes, there are enough resources.
  //
  iport_oport_td *iot = 0;
  ie_bbc * bbc_copy_1 = 0;
  UNI40_traffic_desc *td_copy_1 = 0;
  NodeID *called_party_copy_1 = 0;
  int required_cell_rate = 0;
  int available_cell_rate = (theRAIG != 0) ? theRAIG->GetACR() : 0;

  called_party_copy_1 = new NodeID(*called_party);

  if (bbc != 0) bbc_copy_1 = (ie_bbc *)(bbc->copy());

  if (td != 0) {
    td_copy_1 = (UNI40_traffic_desc *)(td->copy());

  // Choose the Peak Cell Rate with CLP=0+1 for the right direction.
    switch (direction) {
    case ACACPolicy::Succeeding:
      required_cell_rate = td->get_FPCR_01();
      break;

    case ACACPolicy::Preceeding:
      required_cell_rate = td->get_BPCR_01();
      break;
    
    default:
      assert(0);
    }
  }

  if (theRAIG  != 0) theRAIG->SetACR(available_cell_rate - required_cell_rate);

  DIAG(SIM_ACAC_CALL_ADMISSION_RECORDS, DIAG_DEBUG,
       cout << SIM_ACAC_CALL_ADMISSION_RECORDS << " (Debug): "
       << *_localNode << ":"
       << " Port: " << _localPort
       << " inPort: " << inPort
       << " outPort: " << outPort
       << " inCREF: 0x" << hex << inCREF
       << " outCREF: 0x" << hex << outCREF
       << " ACR: " << dec << available_cell_rate - required_cell_rate
       << " Dir: ";
       if (direction == ACACPolicy::Succeeding) cout << "Forward";
       else cout << "Backward";
       cout << endl;)

  //
  // Remember the characteristics of this new call.
  //
  iot = new iport_oport_td(inPort, outPort, service_category, 
			   inCREF, outCREF, 
			   inVPI, inVCI, bbc_copy_1, td_copy_1,
			   called_party_copy_1);

  _Calls.insert(inCREF, iot);
  _Calls.insert(outCREF, iot);
}


//
// Reserve some bandwidth for a call on the preceeding end of the
// following link, assuming we're a physical node.  If there is enough
// bandwidth to support the call's category and there is a call
// reference available, allocate the bandwidth and return the call
// reference value.  Otherwise, return zero.
//
// If returning zero, also set the cause and crankback cause.
//
// AllocateBandwidth() makes   two  entries in _Calls.   The   pair of
// entries share a pointer  to an inport_outport_td.   Each of the two
// entries represents one  half of  the pair  of call  references that
// identify the call.  One is for the in-bound  call reference and the
// other is for the out-bound call reference.
//
ACACPolicy::CallStates
pluggable_port_info::AllocateBandwidth(q93b_setup_message * setup, 
				       u_int inPort, u_int outPort,
				       u_int inCREF, u_int &outCREF,
				       u_int inVPI, u_int inVCI,
				       pluggable_port_info *slavePort,
				       const u_char crankback_level, 
				       const NodeID * called_party, 
				       ie_cause *& cause,
				       PNNI_crankback *& crankback,
				       ACACPolicy::Direction failureDirection)
				       
{
  ACACPolicy::CallStates answer = ACACPolicy::Rejected;
  ig_resrc_avail_info * theSucceedingRAIG = 0;
  ig_resrc_avail_info * thePreceedingRAIG = 0;
  int required_cell_rate = 0;
  int available_cell_rate = 0;

  if (setup != 0) {
    //
    // Extract the traffic descriptor(s)
    //
    const UNI40_traffic_desc * td = 
      (UNI40_traffic_desc *)setup->ie(InfoElem::ie_traffic_desc_ix);

    const UNI40_alt_traffic_desc * alt_td = 
      (UNI40_alt_traffic_desc *)setup->ie(InfoElem::UNI40_alt_traff_desc_ix);

    const UNI40_min_traffic_desc * min_td = 
      (UNI40_min_traffic_desc *)setup->ie(InfoElem::UNI40_min_traff_desc_ix);

    const ie_bbc * bbc = 
      (ie_bbc *)setup->ie(InfoElem::ie_broadband_bearer_cap_ix);

    // Since these two are required for all setups ...
    assert(td && bbc);

    //
    // Can't have both an alt_td and a min_td.
    // Can have one or the other or neither.
    //
    if (!((alt_td != 0) && (min_td != 0))) {
      //
      // Determine the PNNI service category.
      //
      // If there is a RAIG for the service category of the call?
      //
      u_int service_category = ServiceCategory(bbc, td);
      if (service_category != RAIG_FLAG_UNSUPPORTED) {
	
	theSucceedingRAIG = RAIG(service_category);
	thePreceedingRAIG = slavePort->RAIG(service_category);

	//
	// There may be theSucceedingRAIG.  It will only be zero at
	// the called party.
	//
	// There may be thePreceedingRAIG.  It will only be zero at
	// the calling party.
	//
	if ((theSucceedingRAIG != 0) || (thePreceedingRAIG != 0)) {
	  //
	  // There are RAIGs.  Are there enough resources left to
	  // support the call?  There might not be enough in the
	  // succeeding direction.  There had better be enough in
	  // the preceeding direction because the preceeding node
	  // shouldn't have sent us the SETUP unless there is.
	  //

	  bool traffic_descriptors_are_in_spec = true;
	  UNI40_traffic_desc * new_td = 0;
	  UNI40_min_traffic_desc * new_min_td = 0;

	  if (alt_td != 0) {
	    traffic_descriptors_are_in_spec =
	      validate_traffic_descriptors(service_category,
					   theSucceedingRAIG, td, alt_td);
	    if (thePreceedingRAIG != 0)
	      traffic_descriptors_are_in_spec &=
		validate_traffic_descriptors(service_category,
					     thePreceedingRAIG, td, alt_td);
	  }
	  else if (min_td != 0) {
	    traffic_descriptors_are_in_spec =
	      validate_traffic_descriptors(service_category,
					   theSucceedingRAIG, td, min_td);
	    if (thePreceedingRAIG != 0)
	      traffic_descriptors_are_in_spec &=
		validate_traffic_descriptors(service_category,
					     thePreceedingRAIG, td, alt_td);
	  }

	  if (traffic_descriptors_are_in_spec) {
	    q93b_setup_message * setupCopy = 
	      (q93b_setup_message *)setup->copy();
	    answer = _sharedACACPolicy->AdmitCall(service_category,
						  thePreceedingRAIG,
						  theSucceedingRAIG,
						  setupCopy,
						  failureDirection);
	    delete setupCopy;

	    if ((answer != ACACPolicy::Rejected) &&
		((outCREF = NextCallReference()) != 0)) {

	      // XXX 
	      // XXX See if there is a new_td or new_min_td.
	      // XXX If there is, adjust setup appropriately
	      // XXX

	      //
	      //
	      //
	      if (theSucceedingRAIG != 0)
		ReserveBandwidth(outPort, inPort, outCREF, inCREF,
				 0, 0, service_category, 
				 bbc, td, called_party,
				 theSucceedingRAIG, 
				 ACACPolicy::Succeeding);
	      else 
		ReserveBandwidth(outPort, inPort, outCREF, inCREF,
				 0, 0, service_category, 
				 0, 0, called_party,
				 0, 
				 ACACPolicy::Succeeding);

	      if (thePreceedingRAIG != 0)
		slavePort->ReserveBandwidth(inPort, outPort, inCREF, outCREF,
					    inVPI, inVCI, service_category, 
					    bbc, td, called_party,
					    thePreceedingRAIG, 
					    ACACPolicy::Preceeding);

	      else
		slavePort->ReserveBandwidth(inPort, outPort, inCREF, outCREF,
					    inVPI, inVCI, service_category, 
					    0, 0, called_party,
					    0, 
					    ACACPolicy::Preceeding);

	      // Mark the change to this port as "signficant" if the
	      // plugin says it is.
	      if (answer == ACACPolicy::AcceptedSignificantChange) {
		Dirty(true);
		slavePort->Dirty(true);
	      }

	    } else {
	      cause = new ie_cause(ie_cause::user_cell_rate_unavailable, 
				   ie_cause::local_private_network);
	      crankback = 
		new PNNI_crankback(crankback_level, 
				   PNNI_crankback::BlockedLink, 
				   PNNI_crankback::UserCellRateNotAvailable,
				   _localNode, _localPort, 
				   failureDirection == ACACPolicy::Succeeding 
				   ? _remoteNode : slavePort->RemoteNode() );
      
      
	      outCREF = 0;
	      answer = ACACPolicy::Rejected;
	    }
	  } else {
	    // XXX FIX ME
	    // handle as non-manditory ie's with errors.
	    //
	    DIAG(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG,
		 cout << "pluggable_port_info::AllocateBandwidth(): "
		 "One of the min_td or alt_td is bogus" << endl; );

	    outCREF = 0;
	    answer = ACACPolicy::Rejected;
	  }
	} else {
	    cause = new 
	      ie_cause(ie_cause::unsupported_combination_of_traffic_parameters,
		       ie_cause::local_private_network);
	    crankback =  new 
	      PNNI_crankback(crankback_level, 
			     PNNI_crankback::BlockedLink,
			     PNNI_crankback::UnsupportedCombinationOfTrafficParameters, 
			     _localNode, _localPort, 
			     failureDirection == ACACPolicy::Succeeding 
			     ? _remoteNode : slavePort->RemoteNode() );
	  

	    DIAG(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG, 
		 cout << "pluggable_port_info::AllocateBandwidth(): "
		 "unsupported_combination_of_traffic_parameters" << endl; );
	  
	    outCREF = 0;
	    answer = ACACPolicy::Rejected;
	  }
      } else {
	cause = new ie_cause(ie_cause::bearer_capability_not_implemented, 
			      ie_cause::local_private_network);
	crankback = 
	  new PNNI_crankback(crankback_level, 
			     PNNI_crankback::BlockedLink, 
			     PNNI_crankback::BearerServiceNotImplemented, 
			     _localNode, _localPort, 
			     failureDirection == ACACPolicy::Succeeding 
			     ? _remoteNode : slavePort->RemoteNode() );


	DIAG(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG, 
	     cout << "pluggable_port_info::AllocateBandwidth(): "
	     "bearer_capability_not_implemented" << endl;);

	outCREF = 0;
	answer = ACACPolicy::Rejected;
      }
    } else {
      // can't have both alternative traffic descriptor and
      // minimum traffic descriptor.
      //
      // cause 73 unsupported combination of traffic parameters.
      // no crankback
      cause = 
	new ie_cause(ie_cause::unsupported_combination_of_traffic_parameters,
		     ie_cause::local_private_network);
      crankback = 0;
    }
  } else {
    cause = new ie_cause(ie_cause::normal_unspecified,
			  ie_cause::local_private_network);

    crankback = 0;

    DIAG(SIM_ACAC_CALL_ADMISSION, DIAG_DEBUG, 
	 cout << "pluggable_port_info::AllocateBandwidth(): "
	 "No q93b_setup_message.  SHOULDN'T HAPPEN!" << endl;);

    outCREF = 0;
    answer = ACACPolicy::Rejected;
  }

  return answer;
}


//
// Set the vpi and vci for some call reference.
//
void pluggable_port_info::SetVPVC(int cref, u_int vpvc)
{
  dic_item ditem = _Calls.lookup(cref);

  if (ditem != 0) {
    iport_oport_td *call = _Calls.inf(ditem);

    if (call != 0) call->VPVC(vpvc);
    else {
      diag(SIM_ACAC, DIAG_ERROR, 
	   "SetVPVC(): Port %d has no call reference 0x%x\n",
	   _localPort, cref);
    }
  }
}


//
// Set the vpi and vci for some call reference.
//
void pluggable_port_info::SetVPVC(int cref, u_int vpi, u_int vci)
{
  dic_item ditem = _Calls.lookup(cref);

  if (ditem != 0) {
    iport_oport_td *call = _Calls.inf(ditem);

    if (call != 0) call->VPVC(vpi, vci);
    else {
      diag(SIM_ACAC, DIAG_ERROR, 
	   "SetVPVC(): Port %d has no call reference 0x%x\n",
	   _localPort, cref);
    }
  }
}


//
// Set the vpi and vci for some call reference.
//
u_int pluggable_port_info::GetVPVC(int cref)
{
  u_int answer = 0;
  dic_item ditem = _Calls.lookup(cref);

  if (ditem != 0) {
    iport_oport_td *call = _Calls.inf(ditem);

    if (call != 0) answer = call->VPVC();
    else {
      diag(SIM_ACAC, DIAG_ERROR, 
	   "GetVPVC(): Port %d has no call reference 0x%x\n",
	   _localPort, cref);
    }
  }

  return answer;
}


//
// Set the vpi and vci for some call reference.
//
void pluggable_port_info::GetVPVC(int cref, u_int &vpi, u_int &vci)
{
  dic_item ditem = _Calls.lookup(cref);

  if (ditem != 0) {
    iport_oport_td *call = _Calls.inf(ditem);

    if (call != 0) {
     u_int vpvc = call->VPVC();
     vpi = MAKE_VPI(vpvc);
     vci = MAKE_VCI(vpvc);
    }
    else {
      diag(SIM_ACAC, DIAG_ERROR, 
	   "SetVPVC(): Port %d has no call reference 0x%x\n",
	   _localPort, cref);
    }
  }
}

//
// Return some bandwidth to the port.  Increments the available
// bandwidth, decrements the used bandwidth.
//
void pluggable_port_info::FreeBandwidth(u_int call_reference)
{
  ig_resrc_avail_info *theRAIG = 0;
  dic_item ditem = 0;
  int required_cell_rate = 0;
  u_int service_category = 0;
  ie_bbc * bbc;
  UNI40_traffic_desc *td;
  iport_oport_td *iot = 0;

  ditem = _Calls.lookup(call_reference);

  if (ditem != 0) {

    iot = _Calls.inf(ditem);
    service_category = iot->ServiceCategory();
    td = iot->TD();

    _Calls.change_inf(ditem, 0);
    _Calls.del_item(ditem);
  
    theRAIG = RAIG(service_category);

    DIAG(SIM_ACAC_CALL_RELEASE_RECORDS, DIAG_DEBUG, 
	 cout << "Port " << _localPort
	 << " releasing bandwidth for CREF 0x" << hex << call_reference << dec
	 << " service category 0x" << hex << service_category << dec
	 << endl; );

    if (theRAIG != 0) {
      //
      // There is a RAIG.  Are there enough resources left to
      // support the call?
      //

      //
      // Add a fixed amount of bandwidth from this RAIG.
      // Implement a real, subtractive call admission policy later.
      // 
      if (td != 0) required_cell_rate = td->get_BPCR_01();
      const int available_cell_rate = theRAIG->GetACR();

      DIAG(SIM_ACAC_CALL_RELEASE_RECORDS, DIAG_DEBUG,
	   cout << "Port " << _localPort
	   << " CREF= " << _call_reference << ": Before release, RAIG= " 
	   << *theRAIG << endl;);

      theRAIG->SetACR(available_cell_rate + required_cell_rate);

      DIAG(SIM_ACAC_CALL_RELEASE_RECORDS, DIAG_DEBUG,
	   cout << "Port " << _localPort
	   << " CREF= " << _call_reference << ": After release, RAIG= " 
	   << *theRAIG << endl;);

      _dirty = true;


    } else {
      if (_localPort != 0) /* It is not an error for port zero to have
			    * no RAIGs.
			    */
	diag(SIM_ACAC_CALL_RELEASE_RECORDS, DIAG_ERROR, 
	     "Port %d can't find service category 0x%x\n", 
	     _localPort, service_category);
    }


    //
    // Remove the other reference to iot from _Calls.
    //
    if (call_reference == iot->InputCREF())
      ditem = _Calls.lookup(iot->OutputCREF());
    else
      ditem = _Calls.lookup(iot->InputCREF());

    _Calls.change_inf(ditem, 0);
    _Calls.del_item(ditem);

    delete iot;
  
  } else {
    diag(SIM_ACAC_CALL_RELEASE, DIAG_ERROR, 
	 "Port %d can't find call 0x%x.\n", 
	 _localPort, call_reference);
  }
}


  /**
   * GetDest returns the out-bound port number and call reference
   * that correspond to a call reference for a call on this port.
   * 
   * @param inCREF
   * @param outPort
   * @param outCREF
  */
void pluggable_port_info::GetDest(int inCREF, u_int &outPort, int &outCREF)
{
  dic_item ditem = _Calls.lookup(inCREF);

  outPort = outCREF = 0;

  if (ditem != 0) {
    iport_oport_td *iot = _Calls.inf(ditem);
    if (iot != 0) {
      outPort = iot->OutputPort();
      outCREF = iot->OutputCREF();
    }
    else {
      diag(SIM_ACAC, DIAG_ERROR, 
	   "Port %d has no fabric for call reference 0x%x \
in pluggable_port_info 0x%x\n",
	   _localPort, inCREF, ID()); // this);
    }
  } else {
    diag(SIM_ACAC, DIAG_ERROR, 
	 "Port %d has no call reference 0x%x in pluggable_port_info 0x%x\n",
	 _localPort, inCREF, ID()); // this);
  }
}


//
// Return a pointer to the NodeID of the called party.
//
NodeID * pluggable_port_info::CalledParty(int CREF) const
{
  NodeID * answer = 0;
  dic_item di = ((sortseq<u_int, iport_oport_td *>)_Calls).lookup(CREF);

  if (di != 0) {
    iport_oport_td * iot = ((sortseq <u_int, iport_oport_td *>)_Calls).inf(di);

    answer = iot->CalledParty();
  }

  return answer;
}


//
// Returns _theRAIGs.
// This is probably a bad idea.
//
dictionary<u_int, ig_resrc_avail_info *> &
pluggable_port_info::ShareAllRAIGs(void)
{
  return _theRAIGs;
}


//
// Return a pointer to the RAIG that corresponds to raig_flag.
// Assumes that raig_flag is for one and only one service category.
//
// RAIG() returns non-zero if and only if there is a RAIG for the
// service category raig_flag
//
// Performs a linear search for raig_flag through _theRAIGs.  This is
// necessary because the keys to _theRAIGs can represent more than one
// service category.  I.e. they can be the OR of several different
// flags.
//
// BUG: Each search takes O(n) time where n is the number of
// service categories.
//
// BUG: Ignores the GCAC_CLP bit.  If there are two RAIGs whose
// flags differ only in the GCAC_CLP bit, the RAIG() returns the
// first one that it finds.  There is no telling which one that will
// be.
//
ig_resrc_avail_info *pluggable_port_info::RAIG(u_int raig_flag)
{
  bool gcac_clp = raig_flag & RAIG_FLAG_GCAC_CLP;
  ig_resrc_avail_info *answer = 0;
  dic_item ditem;

  if (gcac_clp)
    raig_flag ^= RAIG_FLAG_GCAC_CLP;

  forall_items(ditem, _theRAIGs) {
    u_int flags = _theRAIGs.key(ditem);

    if (flags & raig_flag) {

#if 0
      // We matched the flags without the GCAC_CLP bit

      // Now see that gcac_clp is properly included in the flags.
      // This means that either
      //
      // 1. flags' GCAC_CLP bit is set AND gcac_clp is TRUE
      // or
      // 2. flags' GCAC_CLP bit is not set AND gcac_clp is FALSE
      //
      if ((gcac_clp && (flags & RAIG_FLAG_GCAC_CLP)) ||
	  (!gcac_clp && !(flags & RAIG_FLAG_GCAC_CLP))) {
#endif
	answer = _theRAIGs.inf(ditem);
	break;
#if 0
      }
#endif
    }
  }

  return answer;
}


//
// Insert a new RAIG into _theRAIGs.  
//
// Abort if there is already a RAIG for a service category in the new
// RAIG.
//
// BUG: Permits the insertion of two RAIGs whose flags are identical
// but for the GCAC_CLP bit.  ig_resrc_avail_info * RAIG() returns the
// first of them that it finds but there is no telling which one that
// will be.
//
void pluggable_port_info::RAIG(ig_resrc_avail_info *raig)
{
  u_int flags = raig->GetFlags();

  // Since we must not have two RAIGs for the same service category,
  // we search _theRAIGs one service category at a time.  We must
  // preserve the GCAC_CLP flag in each search because otherwise we
  // might detect a "false positive".  E.g. there might already be a
  // RAIG for CBR but not for CBR + GCAC_CLP, so the search for just
  // CBR would give us a false positive.
  //
  // gcac_clp_flag is RAIG_FLAG_GCAC_CLP if that flag is present in
  // flags.  It is zero if RAIG_FLAG_GCAC_CLP is not present in flags.
  u_int gcac_clp_flag = 
    ((flags & RAIG_FLAG_GCAC_CLP) ? RAIG_FLAG_GCAC_CLP : 0);

  ig_resrc_avail_info *oldRAIG = 0;

  diag(SIM_ACAC, DIAG_DEBUG, 
       "Port %d adding RAIGs for service category 0x%x\n",
       _localPort, flags);

  //
  // Search for each service category that is in raig.  If any of them
  // are already represented in _theRAIGs, then abort the program
  // because we don't know what to do with the duplicates.
  //
  if (flags & RAIG_FLAG_CBR) {
    oldRAIG = RAIG(RAIG_FLAG_CBR | gcac_clp_flag);

    if (oldRAIG != 0) {
      diag(SIM_ACAC, DIAG_FATAL,
	   "Duplicate RAIG for service category CBR\n");
    }
  }

  if (flags & RAIG_FLAG_RTVBR) {
    oldRAIG = RAIG(RAIG_FLAG_RTVBR | gcac_clp_flag);

    if (oldRAIG != 0) {
      diag(SIM_ACAC, DIAG_FATAL,
	   "Duplicate RAIG for service category RTVBR\n");
    }
  }

  if (flags & RAIG_FLAG_NRTVBR) {
    oldRAIG = RAIG(RAIG_FLAG_NRTVBR | gcac_clp_flag);

    if (oldRAIG != 0) {
      diag(SIM_ACAC, DIAG_FATAL,
	   "Duplicate RAIG for service category NRTVBR\n");
    }
  }

  if (flags & RAIG_FLAG_ABR) {
    oldRAIG = RAIG(RAIG_FLAG_ABR | gcac_clp_flag);

    if (oldRAIG != 0) {
      diag(SIM_ACAC, DIAG_FATAL,
	   "Duplicate RAIG for service category ABR\n");
    }
  }

  if (flags & RAIG_FLAG_UBR) {
    oldRAIG = RAIG(RAIG_FLAG_UBR | gcac_clp_flag);

    if (oldRAIG != 0) {
      diag(SIM_ACAC, DIAG_FATAL,
	   "Duplicate RAIG for service category UBR\n");
    }
  }

  _theRAIGs.insert(flags, (ig_resrc_avail_info *)(raig->copy()));
  _dirty = true;
}


//
// What ULIA does this uplink carry?
//
ig_uplink_info_attr * pluggable_port_info::ULIA(void)
{
  return _theULIA;
}


//
// Set the ULIA for this uplink.
//
void pluggable_port_info::ULIA(const ig_uplink_info_attr *aULIA)
{
  if (_theULIA != 0 && _theULIA != aULIA) {
    delete _theULIA;
    _theULIA = 0;
  }

  if (aULIA != 0)
    _theULIA = new ig_uplink_info_attr(*((ig_uplink_info_attr *)aULIA));

  _dirty = true;
}


//
// Add increment to _sequenceNumber.
// Doesn't care if increment is negative.
//
void pluggable_port_info::NextSequenceNumber(int increment)
{
  _sequenceNumber += increment;
}


/* Get the sequence number */
int pluggable_port_info::SequenceNumber(void)
{
  return _sequenceNumber;
}


/* Print the state of the port.  If filename is zero or a string of
 * length zero, print to cout.  Otherwise, print to the filename.  
 */
void pluggable_port_info::Show(const char * filename)
{
  if ((filename == 0) || (strlen(filename) == 0))
    cout << *this;
  else {
    ofstream of(filename);
    of << *this;
  }
}


/** Release all of the calls that go through this port.  Returns a
 * list of FastUNIVisitors of type FastUNIRelease.  The caller must
 * deliver them.  Also returns a list of pairs of <port, call
 * reference> that represent the other half of each call through
 * this port.  The caller must release those calls individually.
 *
 * @param *releases a list of FastUNIVisitors of type FastUNIRelease.
 *
 * @param *call_ids a list of call identifier structures. */
void pluggable_port_info::Release(list <FastUNIVisitor *> **releases,
			  list <struct call_identifier *> **call_ids)
{
  *releases = new list <FastUNIVisitor *>;
  *call_ids = new list <struct call_identifier *>;

  //
  // Release every call in _Calls one at a time.
  // Stop when _Calls is empty.
  //
  while (_Calls.size() > 0) {
    FastUNIVisitor *release = 0;
    struct call_identifier *call_id = 0;

    seq_item min = _Calls.min();

    //
    // Release() reduces the size of _Calls by one.
    //
    Release(_Calls.key(min), &release, &call_id);

    (*releases)->append(release);

    if (call_id != 0)
      (*call_ids)->append(call_id);	
  }

  if ((*call_ids)->size() == 0) {
    delete *call_ids;
    *call_ids = 0;
  }
}

/** Release one call identified by its call reference.  Returns a
 * FastUNIVisitor of type FastUNIRelease for the caller to deliver.
 * Also returns a call_identifier structure that names the other
 * half of this call.  The caller must release that call too. 
 *
 * @param cref the call reference
 *
 * @param *release the FastUNIVisitor of type FastUNIRelease
 *
 * @param *call_id the call_identifier of the other half of this call.
 */
void pluggable_port_info::Release(int cref, 
				  FastUNIVisitor **release,
				  struct call_identifier **call_id)
{
  seq_item call = _Calls.lookup(cref);
  *release = 0;
  *call_id = 0;

  if (call != 0) {
    ie_cause *cause =  new ie_cause(ie_cause::normal_unspecified,
				    ie_cause::local_private_network);

    iport_oport_td *iot = _Calls.inf(call);
    *release = new FastUNIVisitor(_localNode, 
				  iot->CalledParty(), 
				  iot->InputPort(), // out port
				  -1 ,	// vpvc
				  -1, 	// aggregation token
				  cref,
				  FastUNIVisitor::FastUNIRelease);

    //    q93b_release_message *release_message = new q93b_release_message();
    (*release)->SetRelease(cause, 0, 0);

    (*release)->SetInPort(0);
    (*release)->SetOutVP(iot->VPI());
    (*release)->SetOutVC(iot->VCI());

    *call_id = new call_identifier;
    (*call_id)->_cref = iot->OutputCREF();
    (*call_id)->_port = iot->OutputPort();

    FreeBandwidth(cref);
  } 
}


/* Output the state of the port to an ostream. */
ostream & operator << (ostream & os, const pluggable_port_info & port)
{
  dic_item ditem = 0;

  os << "_dirty: ";
  if (port._dirty) os << "true ";
  else os << "false ";
  
  os << " _npstate: ";
  if (port._npstate == NPStateVisitor::FullState) os << "FullState ";
  else os << "NonFullState ";

  os << " _up: ";
  if (port._up) os << "true ";
  else os << "false ";

  os << " _type: ";
  switch (port._type) {
  case pluggable_port_info::unspecified:
    os << "unspecified ";
    break;

  case pluggable_port_info::horizontal_link:
    os << "HLINK ";
    break;

  case pluggable_port_info::uplink:
    os << "UPLINK ";
    break;

  default:
    os << "ERROR! ";
    break;
  }
  os << " _aggregationToken: " << port._aggregationToken;
  os << " _id: " << port._id;
  os << " _sequenceNumber: " << port._sequenceNumber;
  os << " _localPort: " << port._localPort;
  os << " _remotePort: " << port._remotePort;
  os << endl;

  os << " _remoteNode: ";
  if (port._remoteNode != 0) os << *port._remoteNode;
  else os << "0";

  os << " _remoteAddress: ";
  if (port._remoteAddress != 0) os << *port._remoteAddress;
  else os << "0";

  os << endl;

  os << "RAIGs:" << endl;
  forall_items(ditem, port._theRAIGs) {
    u_int flags = port._theRAIGs.key(ditem);
    ig_resrc_avail_info *aRAIG = port._theRAIGs.inf(ditem);

    if (flags & RAIG_FLAG_CBR)
      os << "CBR | ";

    if (flags & RAIG_FLAG_RTVBR)
      os << "RTVBR | ";

    if (flags & RAIG_FLAG_NRTVBR)
      os << "NRTVBR | ";

    if (flags & RAIG_FLAG_ABR)
      os << "ABR | ";

    if (flags & RAIG_FLAG_UBR)
      os << "UBR | ";

    if (flags & RAIG_FLAG_GCAC_CLP)
      os << "GCAC_CLP";

    os << *aRAIG << endl;
  }
  
  os << "ULIA: ";
  if (port._theULIA != 0) os << *(port._theULIA) << endl;
  else os << endl;

  os << "Calls:" << endl;
  forall_items(ditem, port._Calls) {
    pluggable_port_info::iport_oport_td *call_stuff = port._Calls.inf(ditem);
    os << "  Call 0x" << hex << (int) port._Calls.key(ditem) << dec;
    if (call_stuff)
      os << " iport " << call_stuff->_input_port
	 << " oport " << call_stuff->_output_port
	 << " category 0x" << hex << call_stuff->_service_category
	 << dec << endl;
    else os << endl;
  }
  os << "--------------------------------------------------" << endl;
  os << endl;
  return os;
}


// -------------------------------------------------------------------
pluggable_port_info::iport_oport_td::iport_oport_td(u_int iport, u_int oport, 
						    u_int scat,
						    u_int input_cref, 
						    u_int output_cref, 
						    u_int vpi, u_int vci,
						    ie_bbc * bbc, 
						    UNI40_traffic_desc * td,
						    NodeID * called_party)
  : _input_port(iport), _output_port(oport), _service_category(scat),
    _input_cref(input_cref), _output_cref(output_cref), 
    _vpi(vpi), _vci(vci), _bbc(bbc), _td(td),
    _called_party(called_party) { }

pluggable_port_info::iport_oport_td::~iport_oport_td() 
{ 
  delete _bbc; delete _td; delete _called_party; 
}

void pluggable_port_info::iport_oport_td::BBC(ie_bbc * bbc) 
{
  if (_bbc != 0 && _bbc != bbc) delete _bbc;
  _bbc = bbc; 
}

void pluggable_port_info::iport_oport_td::TD(UNI40_traffic_desc *td) 
{
  if (_td != 0 && _td != td) delete _td;
  _td = td; 
}

void pluggable_port_info::iport_oport_td::CalledParty(NodeID *called_party) 
{
  if (_called_party != 0 && _called_party != called_party) 
    delete _called_party;
  
  _called_party = called_party; 
}
