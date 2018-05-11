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
static char const _NodePeerState_cc_rcsid_[] =
"$Id: NodePeerState.cc,v 1.237 1999/02/10 18:48:59 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/kernel/Kernel.h>
#include <FW/actors/State.h>
#include <FW/basics/diag.h>
#include <FW/basics/Log.h>

#include <fsm/nodepeer/NodePeerState.h>
#include <fsm/nodepeer/NodePeerTimers.h>
#include <fsm/nodepeer/RXEntry.h>
#include <fsm/nodepeer/NPVisitors.h>
#include <fsm/database/DatabaseInterfaces.h>
#include <fsm/hello/HelloVisitor.h>
#include <fsm/visitors/PortUpVisitor.h>
#include <fsm/visitors/PortDownVisitor.h>
#include <fsm/visitors/DBVisitors.h>
#include <fsm/visitors/LinkUpVisitor.h>
#include <fsm/visitors/VPVCVisitor.h>
#include <fsm/visitors/NPFloodVisitor.h>
#include <fsm/visitors/FastUNIVisitor.h>
#include <fsm/hello/HelloVisitor.h>
#include <fsm/netstats/NetStatsCollector.h>

#include <codec/pnni_pkt/ptsp.h>
#include <codec/pnni_pkt/ptse_req.h>
#include <codec/pnni_pkt/ptse_ack.h>
#include <codec/pnni_pkt/database_sum.h>
#include <codec/pnni_ig/DBKey.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/nodal_ptse_ack.h>
#include <codec/pnni_ig/req_ptse_header.h>

#define CONSTANT_DSRxInterval      20
#define CONSTANT_PTSEReqRxInterval 20
#define CONSTANT_PTSEAckInterval   5
#define CONSTANT_PTSPRxInterval    20

const VisitorType * NodePeerState::_port_up_type   = 0;
const VisitorType * NodePeerState::_port_down_type = 0;
const VisitorType * NodePeerState::_ds_type        = 0;
const VisitorType * NodePeerState::_req_type       = 0;
const VisitorType * NodePeerState::_ptsp_type      = 0;
const VisitorType * NodePeerState::_ack_type       = 0;
const VisitorType * NodePeerState::_npflood_type   = 0;
const VisitorType * NodePeerState::_link_up_type   = 0;
const VisitorType * NodePeerState::_svcc_type      = 0;
const VisitorType * NodePeerState::_hlink_type     = 0;
const VisitorType * NodePeerState::_fast_uni_type  = 0;

// --------------------------------------------------------------------------------
NodePeerState::~NodePeerState() 
{ 
  if (_AckTimer)         delete _AckTimer;
  if (_PTSEReqRxpkt)     delete _PTSEReqRxpkt;
  if (_DSPktOutStanding) delete _DSPktOutStanding;
  if (_DSRxTimer)        delete _DSRxTimer;
  if (_ReqRxTimer)       delete _ReqRxTimer;
  if (_LocNodeID)        delete _LocNodeID;
  if (_RemNodeID)        delete _RemNodeID;
}

NodePeerState::NodePeerState(u_char * nid, u_char *rnid)
  : _SendAck(false), _level(0),
    _DSRxInterval(CONSTANT_DSRxInterval), 
    _PTSEReqRxInterval(CONSTANT_PTSEReqRxInterval), 
    _PTSEAckInterval(CONSTANT_PTSEAckInterval),
    _PTSPRxInterval(CONSTANT_PTSPRxInterval),
    _State(NpDown), _vpi(-1), _vci(-1), _cref(0xffffffff),
    _DSSequenceNum(0), _PTSEReqRxpkt(0L), _PTSEAckpkt(0L),
    _DSPktOutStanding(0L), _LocNodeID(0), _RemNodeID(0),
    _Mode(Unknown)
{
  assert(nid && rnid);

  if (theKernel().GetSpeed() == Kernel::REAL_TIME) {
    double time = theKernel().CurrentTime();
    _DSSequenceNum = ((int)time) & 0xFFFFFF;
  }

  // Allocate the VisitorTypes
  if (!_port_up_type)
    _port_up_type = QueryRegistry(PORT_UP_VISITOR_NAME);
  if (!_port_down_type)
    _port_down_type = QueryRegistry(PORT_DOWN_VISITOR_NAME);
  if (!_ds_type)
    _ds_type = QueryRegistry(DS_VISITOR_NAME);
  if (!_req_type)
    _req_type = QueryRegistry(REQ_VISITOR_NAME);
  if (!_ptsp_type)
    _ptsp_type = QueryRegistry(PTSP_VISITOR_NAME);
  if (!_ack_type)
    _ack_type = QueryRegistry(ACK_VISITOR_NAME);
  if (!_npflood_type)
    _npflood_type = QueryRegistry(NPFLOOD_VISITOR_NAME);
  if (!_link_up_type)
    _link_up_type = QueryRegistry(LINK_UP_VISITOR_NAME);
  if (!_svcc_type)
    _svcc_type = QueryRegistry(SVCC_PORT_INFO_VISITOR);
  if (!_hlink_type)
    _hlink_type = QueryRegistry(HLINK_VISITOR_NAME);
  if (!_fast_uni_type)
    _fast_uni_type = QueryRegistry(FAST_UNI_VISITOR_NAME);
  // Done alloc'ing the vistypes

  _LocNodeID = new NodeID(nid);
  _level = _LocNodeID->GetLevel();
  _RemNodeID = new NodeID(rnid);

  assert(_DSRxTimer = new DSRxTimer(this, _DSRxInterval));
  assert(_ReqRxTimer = new ReqRxTimer(this, _PTSEReqRxInterval));
  assert(_AckTimer = new AckTimer(this, _PTSEAckInterval));

  if (_LocNodeID->GetChildLevel() == 160)
    _NodeType = PhysicalNode;
  else
    _NodeType = LogicalNode;

  DIAG("fsm.nodepeer", DIAG_DEBUG, cout << 
       (_NodeType == PhysicalNode ? "Physical NodePeer " : 
	"Logical NodePeer ") <<
       *(_LocNodeID) << " NodePeerState is NPDOWN." << endl);
}


NodePeerState::NodePeerState(const NodeID * nid, const NodeID * rnid, 
			     int vp, int vc, u_long cref)
  : _SendAck(false), _State(NpDown),
    _DSSequenceNum(0), _PTSEReqRxpkt(0), _PTSEAckpkt(0),
    _DSPktOutStanding(0), _vpi(vp), _vci(vc), _cref(cref),
    _DSRxInterval(CONSTANT_DSRxInterval), 
    _PTSEReqRxInterval(CONSTANT_PTSEReqRxInterval), 
    _PTSEAckInterval(CONSTANT_PTSEAckInterval),
    _PTSPRxInterval(CONSTANT_PTSPRxInterval)
{
  assert(nid && rnid);

  if (theKernel().GetSpeed() == Kernel::REAL_TIME) {
    double time = theKernel().CurrentTime();
    _DSSequenceNum = ((int)time) & 0xFFFFFF;
  }
  // Allocate the VisitorTypes
  if (!_port_up_type)
    _port_up_type = QueryRegistry(PORT_UP_VISITOR_NAME);
  if (!_port_down_type)
    _port_down_type = QueryRegistry(PORT_DOWN_VISITOR_NAME);
  if (!_ds_type)
    _ds_type = QueryRegistry(DS_VISITOR_NAME);
  if (!_req_type)
    _req_type = QueryRegistry(REQ_VISITOR_NAME);
  if (!_ptsp_type)
    _ptsp_type = QueryRegistry(PTSP_VISITOR_NAME);
  if (!_ack_type)
    _ack_type = QueryRegistry(ACK_VISITOR_NAME);
  if (!_npflood_type)
    _npflood_type = QueryRegistry(NPFLOOD_VISITOR_NAME);
  if (!_link_up_type)
    _link_up_type = QueryRegistry(LINK_UP_VISITOR_NAME);
  if (!_svcc_type)
    _svcc_type = QueryRegistry(SVCC_PORT_INFO_VISITOR);
  if (!_hlink_type)
    _hlink_type = QueryRegistry(HLINK_VISITOR_NAME);
  if (!_fast_uni_type)
    _fast_uni_type = QueryRegistry(FAST_UNI_VISITOR_NAME);
  // Done alloc'ing the vistypes

  _LocNodeID = new NodeID(*nid);
  _level = _LocNodeID->GetLevel();
  _RemNodeID = new NodeID(*rnid); 
  
  assert(_DSRxTimer = new DSRxTimer(this, _DSRxInterval));
  assert(_ReqRxTimer = new ReqRxTimer(this, _PTSEReqRxInterval));
  assert(_AckTimer = new AckTimer(this, _PTSEAckInterval));

  if (_LocNodeID->GetChildLevel() == 160)
    _NodeType = PhysicalNode;
  else
    _NodeType = LogicalNode;

  DIAG("fsm.nodepeer", DIAG_DEBUG, cout << 
       (_NodeType == PhysicalNode ? "Physical NodePeer " : 
	"Logical NodePeer ") <<
       *(_LocNodeID) << " NodePeerState is NPDOWN." << endl);
}


void NodePeerState::Interrupt(SimEvent * e) 
{
  diag("fsm.nodepeer", DIAG_DEBUG, 
       "NodePeerState (%x) was interrupted by %x!\n", this, e);
}

//  NodePeerState::Handle(Visitor)
//  Function: Looks mainly for Database Summary Packets,
//            ptsp request packets, ptsp acknowledgement
//            packets, ptsp packets, Link up and down events, 
//            NPFloodVisitors from database.
//            Calls appropriate  mathods to process these 
//            visitors. Just passes through the other packets.
State * NodePeerState::Handle(Visitor * v) 
{
  assert(v);

  DIAG("fsm.nodepeer", DIAG_DEBUG,
       cout << OwnerName() << " is in " << StateName() << endl
            << (_NodeType == PhysicalNode ? "Physical NodePeer " : 
		"Logical NodePeer ");
       if (_LocNodeID) cout << *(_LocNodeID);
       cout << " received a " << v->GetType() << endl;
       );

  ThisVisitor type = GetVisitorType(v);
  
  switch (type) {
    case DSPkt:
      RcvDSPkt((DSVisitor *)v);
      break;
    case ReqPkt:
      RcvPTSEReqPkt((ReqVisitor *)v);
      break;
    case PtspPkt:
      RcvPTSPPkt((PTSPVisitor *)v);
      break;
    case AckPkt:
      RcvPTSEAckPkt((AckVisitor *)v);
      break;
    case AddPort:
      diag("fsm.nodepeer", DIAG_DEBUG, 
	   "%s: Received the PortUpVisitor\n", OwnerName());
      RcvAddPort((PortUpVisitor *)v);
      PassThru(v);
      v = 0;
      break;
    case SvccPortInfo:
      RcvSvccPortInfo((SVCCPortInfoVisitor *)v);
      PassThru(v);
      v = 0;
      break;
    case DropPort:
      RcvDropPort((PortDownVisitor *)v);
      PassThru(v);
      v = 0;
      break;
    case Flood :
      RcvFloodPkt((NPFloodVisitor *)v);
      break;
    case Invalid:
      // Handled by the if block at end of Handle
      break;
    default:
      PassThru(v);
      v = 0;
      break;
  }
  if (v) v->Suicide();

  return this;
}

// NodePeerState::GetVisitorType(Visitor)
// Function : Determines wheather a visitor is
//            of database summary type(has database
//            summary pkt), or request type, Flood type(contains
//            a ptsp which needs to be flooded), ptsp type,
//            Acknowledgement type, or is indicating a 
//            Link up event or a Link down event
NodePeerState::ThisVisitor NodePeerState::GetVisitorType(Visitor * v)
{
  assert(v);
  ThisVisitor rval = Invalid;
  VisitorType vt = v->GetType();

  const NodeID * destNID = ((VPVCVisitor *)v)->GetDestNID();

  if (_NodeType == PhysicalNode) {
    if ((rval == Invalid) && vt.Is_A(_port_up_type))
      rval = AddPort;
    if ((rval == Invalid) && vt.Is_A(_port_down_type))
      rval = DropPort;
  }

  if ((rval == Invalid) && vt.Is_A(_ds_type)) {
    // Temporary Addition to check level testing in 
    // general DestNodeID mux does this
    if (_LocNodeID->equals(destNID))
      rval = DSPkt;
    else
      rval = passthru;
  }
  
  if ((rval == Invalid) && vt.Is_A(_req_type)) {
    if (_LocNodeID->equals(destNID))
      rval = ReqPkt;
    else
      rval = passthru;
  }

  if ((rval == Invalid) && vt.Is_A(_ptsp_type)) { 
    if (_LocNodeID->equals(destNID))
      rval = PtspPkt;
    else
      rval = passthru;
  }
  
  if ((rval == Invalid) && vt.Is_A(_ack_type)) {
    if (_LocNodeID->equals(destNID))
      rval = AckPkt;
    else
      rval = passthru;
  }

  if ((rval == Invalid) && vt.Is_A(_npflood_type)) {
    // Introducing a temporary condition to Check
    // if the NodeId is same as this one
    // Check must be made at the SourceId mux.  
    // Introduced to check different level NP's 
    const NodeID * nid = ((VPVCVisitor *)v)->GetSourceNID();
    if (nid && (*nid == *_LocNodeID))
      rval = Flood;
  }
  
  if ((rval == Invalid) && vt.Is_A(_link_up_type))
    rval = passthru;

  if (_NodeType == LogicalNode) {
    if ((rval == Invalid) && vt.Is_A(_svcc_type))
      rval = SvccPortInfo;

    if ((rval == Invalid) && vt.Is_A(_hlink_type))
      rval = passthru;
  }
  // handle FUVs properly
  if ((rval == Invalid) && vt.Is_A(_fast_uni_type))
    rval = passthru;
  
  return rval;
}

// RcvDSPkt(DSVisitor)
// Function: To process a Database Summary packet as in
//           5.7.6 page 93, specs to summarise,
//           1 Get to Negotiating done when the master slave relationship
//             is defined
//           2. Exchange database summary packets 
//           3. Request for database packets if some ptsp pkt
//              which he advertises is required. 
//           4. reach Exchanging state when complete exchanging the 
//              summary 
//           5. Handle mismatch conditions like breach in master, slave
//              version
int NodePeerState::RcvDSPkt(DSVisitor * v)
{
  assert(v);
  int i, rval = NodePeerState::fsm_ok;
  const DatabaseSumPkt * rDpkt = v->GetDSPktptr(); 
  if (rDpkt)
    {
      DIAG("fsm.nodepeer", DIAG_DEBUG, 
	   cout << OwnerName() << ": RcvDSPkt" << endl;
	   cout << OwnerName() << *rDpkt << endl;
	   if (_LocNodeID)
	   cout << "LocNodeID: " << *_LocNodeID << endl;
	   if (_RemNodeID)
	   cout << "RemNodeID: " << *_RemNodeID << endl;
	   cout << "init_bit   = " << rDpkt->IsSet(DatabaseSumPkt::init_bit) 
	   << endl;
	   cout << "more_bit   = " << rDpkt->IsSet(DatabaseSumPkt::more_bit) 
	   << endl;
	   cout << "master_bit = " << rDpkt->IsSet(DatabaseSumPkt::master_bit) 
	   << endl;
	   if ((rDpkt->GetNodalSummaries()).empty())
	   cout << "Empty Contents." << endl;
	   else
	   cout << "NOT Empty Contents." << endl;
	  if(*_RemNodeID > *_LocNodeID)
	  cout << "Remote Node ID > Local Node ID" << endl;
	  else
	  cout << "Remote Node ID < Local Node ID" << endl;
	   );

      switch (_State)
	{
	case NodePeerState::NpDown :
	  return NodePeerState::fsm_ok;

	case NodePeerState::Negotiating :
	  // page 93 of the spec
	  if ( rDpkt->IsSet(DatabaseSumPkt::init_bit)   &&
	       rDpkt->IsSet(DatabaseSumPkt::more_bit)   &&
	       rDpkt->IsSet(DatabaseSumPkt::master_bit) &&
	       (rDpkt->GetNodalSummaries()).empty()     &&
	       (*_RemNodeID > *_LocNodeID) )
	    {
	      diag("fsm.nodepeer", DIAG_DEBUG, 
		   "%s: State Negotiating -- SLAVE\n", OwnerName());
	      Cancel(_DSRxTimer);
	      _Mode = NodePeerState::Slave;
	      if (_DSPktOutStanding)
		{
		  delete _DSPktOutStanding;
		  _DSPktOutStanding = 0L;
		}

	      DBNodePeerInterface * dbi = (DBNodePeerInterface *)QueryInterface( "Database" );
	      assert( dbi != 0 && dbi->good() );
	      dbi->Reference();
	      SetDSPkt( dbi->GetDatabaseSummary( _level ) );
	      dbi->Reference();
	      if (!_DSPktOutStanding)
		{
		  // Case where nothing in our database. May be possible
		  _DSPktOutStanding = new DatabaseSumPkt(rDpkt->GetSequenceNum());
		}
	      // Set the DS Sequence Number to that specified by the Master
	      SetDSSequenceNum(rDpkt->GetSequenceNum());
	      _DSPktOutStanding->SetSequenceNum(_DSSequenceNum);
	      _DSPktOutStanding->RemBit(DatabaseSumPkt::init_bit);
	      _DSPktOutStanding->RemBit(DatabaseSumPkt::master_bit);
	      _DSPktOutStanding->RemBit(DatabaseSumPkt::more_bit); // ??

	      diag("fsm.nodepeer", DIAG_DEBUG,"%s: going to Exchanging @line = %d\n", OwnerName(),__LINE__);
	      ChangeState( NodePeerState::Exchanging );
	      do_ds(v, 2);
	      return NodePeerState::fsm_ok;
	    }
	  
	  if ( !(rDpkt->IsSet(DatabaseSumPkt::init_bit))   &&
	       !(rDpkt->IsSet(DatabaseSumPkt::master_bit)) &&
	       (_DSSequenceNum == rDpkt->GetSequenceNum()) &&
	       (*_RemNodeID < *_LocNodeID) ) 
	    {
	      diag("fsm.nodepeer", DIAG_DEBUG, 
		   "%s: State Negotiating -- MASTER\n", OwnerName());
	      Cancel(_DSRxTimer);
	      _Mode = NodePeerState::Master;
	      // Processing the Database summary & preparing the request pkt
	      DBNodePeerInterface * dbi = (DBNodePeerInterface *)QueryInterface( "Database" );
	      assert( dbi != 0 && dbi->good() );
	      dbi->Reference();
	      SetReqPkt( dbi->ReqDiffDB((DatabaseSumPkt *)rDpkt, _RemNodeID) );
	      if (_DSPktOutStanding)
		{  
		  delete _DSPktOutStanding;
		  _DSPktOutStanding = 0L;
		}
	      SetDSPkt( dbi->GetDatabaseSummary(_level) );
	      dbi->Unreference();
	      // Increment the DS Sequence Number
	      SetDSSequenceNum(_DSSequenceNum+1);
	      if (!_DSPktOutStanding)
		{
		  // Case where nothingin our database. May be possible
		  _DSPktOutStanding = new DatabaseSumPkt(_DSSequenceNum);
		}
	      _DSPktOutStanding->SetSequenceNum(_DSSequenceNum);
	      _DSPktOutStanding->RemBit(DatabaseSumPkt::init_bit);
	      _DSPktOutStanding->SetBit(DatabaseSumPkt::master_bit);
	      _DSPktOutStanding->SetBit(DatabaseSumPkt::more_bit);
	  
	      diag("fsm.nodepeer", DIAG_DEBUG,"%s: going to Exchanging @line = %d\n", OwnerName(),__LINE__);
	      ChangeState( NodePeerState::Exchanging );

	      if (_PTSEReqRxpkt) 
		SendReqPkt();
	      do_ds(v, 2);
	      return NodePeerState::fsm_ok;
	    }
	  diag("fsm.nodepeer", DIAG_DEBUG, 
	       "%s: State Negotiating -- DISCARD DS Summary packet\n", OwnerName());
	  return NodePeerState::fsm_ok;

	case NodePeerState::Exchanging :
	  Cancel(_DSRxTimer);
	  if ((rDpkt->IsSet(DatabaseSumPkt::master_bit) && 
	       (_Mode == NodePeerState::Master)) ||
	      (!(rDpkt->IsSet(DatabaseSumPkt::master_bit)) && 
	       (_Mode == NodePeerState::Slave)) ||
	      (rDpkt->IsSet(DatabaseSumPkt::init_bit)))
	    {
	      DIAG("fsm.nodepeer", DIAG_DEBUG, cout <<
		   OwnerName() << ": Got an error!" << endl;
		   if ((rDpkt->IsSet(DatabaseSumPkt::master_bit) && 
			(_Mode == NodePeerState::Master)))
		   cout << "Master bit is set on incoming packet, but I'm the master!" << endl;
		   else if ((!(rDpkt->IsSet(DatabaseSumPkt::master_bit)) && 
			     (_Mode == NodePeerState::Slave)))
		   cout << "Master bit is not set on incoming packet and I'm the slave!" << endl;
		   else if (rDpkt->IsSet(DatabaseSumPkt::init_bit))
		   cout << "The Init bit is set on incoming packet, and I'm in Exchanging!" << endl;
		   else
		   cout << "The sky is falling!  The sky is falling!" << endl;
		   );
	      SetDSSequenceNum(rDpkt->GetSequenceNum()+1);
	      do_ds(v, 5);
	      return NodePeerState::fsm_error;
	    }
	  if (_Mode == NodePeerState::Master)
	    MasterProcessDS((DatabaseSumPkt *)rDpkt);
	  else
	    SlaveProcessDS((DatabaseSumPkt *)rDpkt);
	  return NodePeerState::fsm_ok;


      case NodePeerState::Loading: 
      case NodePeerState::Full :
	Cancel(_DSRxTimer);
	if ((rDpkt->IsSet(DatabaseSumPkt::master_bit) && (_Mode == ( NodePeerState::Master)))  ||
	    ((!(rDpkt->IsSet(DatabaseSumPkt::master_bit))) && (_Mode == (NodePeerState::Slave))) ||
	    (_DSSequenceNum != rDpkt->GetSequenceNum()) || 
	    (rDpkt->IsSet(DatabaseSumPkt::init_bit)))
	  {
	    SetDSSequenceNum(rDpkt->GetSequenceNum()+1);
	    do_ds(v, 5);
	    return NodePeerState::fsm_ok;
	  }
	if (_Mode == NodePeerState::Slave)
	  SendDSPkt();
	return NodePeerState::fsm_ok;
	
	default:
	  diag("fsm.nodepeer", DIAG_DEBUG, 
	       "%s: Unknown State\n", OwnerName());
	  return NodePeerState::fsm_error;
	}
    }
  else
    {
      diag("fsm.nodepeer", DIAG_ERROR, 
	   "%s: DSPkt with Null pointer received.\n", OwnerName());
    }
  return NodePeerState::fsm_error;
}
  

// RcvPTSEReqPkt(ReqVisitor)
//   Function: When a request pkt comes, processing in diff states
//             NpDown, Negotiating: Ignore.
//             Exchanging, Loading, Full: Call FloodDiffDB method  of
//             the database which returns a list of the PTSP
//             pkts requested.
//             Forall the PTSP's requested send the PTSPPkt one by
//             one. Send the same instances of ptsp pkts as
//             returned by the database as they are not going to
//             be retransmitted
int NodePeerState::RcvPTSEReqPkt(ReqVisitor * v)
{
  assert(v);
  const PTSEReqPkt * rRpkt = v->GetReqPktptr();

  // assert(rRpkt);
  if (rRpkt) {
    diag("fsm.nodepeer", DIAG_DEBUG, "%s: RcvReqPkt (%x).\n", OwnerName(), 
	 rRpkt);
    list_item li;
    list<PTSPPkt *> * listPtsp = 0;
    switch (_State) {
      case NodePeerState::NpDown :  
      case NodePeerState::Negotiating :
	diag("fsm.nodepeer", DIAG_DEBUG,
	     "%s: Ignoring request in less than Exchanging State.\n", 
	     OwnerName());
	break;
      case NodePeerState::Exchanging :
      case NodePeerState::Loading    :
      case NodePeerState::Full       :
	{
	  DBNodePeerInterface * dbi = (DBNodePeerInterface *)QueryInterface( "Database" );
	  assert( dbi != 0 && dbi->good() );
	  dbi->Reference();
	  listPtsp = dbi->FloodDiffDB((PTSEReqPkt *)rRpkt);
	  dbi->Unreference();
	  if (!listPtsp) {
	    SetDSSequenceNum(_DSSequenceNum+1);
	    diag("fsm.nodepeer", DIAG_FATAL, "Received PTSE Req w/o list of PTSPs!"
		 "NodePeerState.cc: line %d", __LINE__ );
	    do_ds(v, 5);
	    return NodePeerState::fsm_error;
	  }
	  forall_items(li, *listPtsp) {
	    PTSPPkt * lPtsp = listPtsp->inf(li);
	    SendPTSPPkt(lPtsp);
	  }
	  listPtsp->clear();
	  delete listPtsp;
	}
        break;
      default:
	return NodePeerState::fsm_error;
    }
  } else {
    diag("fsm.nodepeer", DIAG_ERROR,
	 "%s: ReqPkt will Null pointer received!\n", OwnerName());
  }
  return NodePeerState::fsm_ok;
}

//   RcvPTSPPkt(PTSPVisitor)
//   Function:  Precisely as in 5.8.3.3 page 108
int NodePeerState::RcvPTSPPkt(PTSPVisitor *v)
{
  assert(v);
  bool      yes = false;
  PTSPPkt * tmp = v->GetPTSPPktptr(); 

  diag("fsm.nodepeer", DIAG_DEBUG, "%s: Received PTSPPkt (%x).\n", 
       OwnerName(), tmp);

  if (!tmp) {
    diag("fsm.nodepeer", DIAG_ERROR, "%s: PTSPVisitor with null PTSP!\n", 
	 OwnerName());
    return NodePeerState::fsm_error;
  }

  switch (_State) {
    case  NodePeerState::NpDown      :
    case  NodePeerState::Negotiating :
      return NodePeerState::fsm_ok;
    case  NodePeerState::Exchanging  :
    case  NodePeerState::Loading     :
    case  NodePeerState::Full        :
      // Added below so that the in PTSP visitor, it is
      // the PTSP is deleted hereonwards. -Sandeep
      PTSPPkt * rPtsp = (PTSPPkt *)(tmp->copy());
      // Added above -Sandeep
      PTSPPkt * sPtsp = new PTSPPkt(rPtsp->GetOID(), rPtsp->GetPGID());
      list<ig_ptse *> _rem;
      list_item li;
      const list<ig_ptse *> & ptse_list = rPtsp->GetElements();

      if (!(ptse_list.empty())) {
	forall_items(li, ptse_list) {
	  ig_ptse * rPtse = ptse_list.inf(li);
	  
	  if (!rPtse) {
	    diag("fsm.nodepeer", DIAG_FATAL, 
		 "%s: PTSE null pointer in received PTSP!\n", OwnerName());
	  } else
	    diag("fsm.nodepeer", DIAG_DEBUG, 
		 "%s: received PTSE of type %d.\n", 
		 OwnerName(), rPtse->GetType());
	  // 1
	  // if ((rPtse->GetTTL()) != DBKey::ExpiredAge)
	  // Need to be written, a checksum fn
	  // if (!rPtse->CheckSum(rPtsp->GetOID(), rPtsp->GetPGID())) {
	  //   rPtsp->RemPTSE(rPtse);
	  //   continue;
	  // }
	  // In ptse, CheckSum(u_char * onid, u_char * pgid)
	  
	  // 2
	  if (_State == NodePeerState::Exchanging) 
	    yes = true; 
	  else if (_State == NodePeerState::Loading) 
	    yes = true;
	  if (yes) {
	    if (IsBadRequest(rPtsp->GetOID(), rPtse)) {
	      diag("fsm.nodepeer", DIAG_DEBUG, 
		   "%s: State Exchanging or Loading and Bad PTSE request!\n", 
		   OwnerName());
	      if (sPtsp) 
		delete sPtsp;
	      if (rPtsp) 
		delete rPtsp;
	      do_ds(v, 5);
	      return NodePeerState::fsm_error;
	    }
	  }
	  NodeID    nid(rPtsp->GetOID());
	  ig_ptse * lPtse = 0;

	  DBNodePeerInterface * dbi = (DBNodePeerInterface *)QueryInterface( "Database" );
	  assert( dbi != 0 && dbi->good() );
	  dbi->Reference();
	  lPtse = dbi->ReqPTSE(&nid, rPtse);
	  dbi->Unreference();

	  // 3 & 4
	  // Replacing to check if 
	  // rptse is ever zero
	  // if (lPtse && rPtse) 
	  if (lPtse) {
	    if (*lPtse > *rPtse) {
	      CheckPTSPRxlistForCase3(rPtse, lPtse, sPtsp);
              _rem.append(rPtse); 
	      continue;
	    } else if (*lPtse == *rPtse) {
	      CheckPTSPRxlistForCase4(lPtse, rPtse, rPtsp);
	      _rem.append(rPtse);
	      continue;
	    }
	  }
	  // 5 b
	  else if (_State == (NodePeerState::Full) && 
		   (rPtse->GetTTL() == DBKey::ExpiredAge)) {
	    diag("fsm.nodepeer", DIAG_DEBUG, 
		 "%s: State Full and PTSP received is expired.  \
Acknowledging.\n", 
		 OwnerName());
	    if (!_PTSEAckpkt) {  
	      _PTSEAckpkt = new PTSEAckPkt();
	      Register(_AckTimer);
	      diag("fsm.nodepeer", DIAG_DEBUG, "%s: AckTimer Started.\n", 
		   OwnerName());
	    }
	    assert(rPtsp->GetOID()); // added Abdella 01/07/99
	    _PTSEAckpkt->AddNodalPTSEAck(rPtsp->GetOID(), rPtse); 
	    _SendAck = true;
	    _rem.append(rPtse);
	    continue;
	  }
	  
	  // No Acks for 6 a
	  if (memcmp((void *)rPtsp->GetOID(), 
		     (void *)_LocNodeID->GetNID(), 22) == 0) {
	    if (!(!(lPtse) && (rPtse->GetTTL() == DBKey::ExpiredAge)))
	      continue;
	  }

	  // Ack for all pkts to be processed in Dbase  5 a & 6 b
	  // If floding is done to this node also then ok.
	  // If not we need to ensure for this fsm we require to
	  // delete PTSPRxlist & Ack list if we have any older
	  // copy which need to be deleted.
	  if (!_PTSEAckpkt) {
	    _PTSEAckpkt = new PTSEAckPkt();
	    Register(_AckTimer);
	    diag("fsm.nodepeer", DIAG_DEBUG, "%s: AckTimer Started.\n", 
		 OwnerName());
	  }
	  _PTSEAckpkt->AddNodalPTSEAck(rPtsp->GetOID(), rPtse);
	}	
      }

      // Added Aug 25
      if (!_rem.empty()) {
        // Removing less recent ptse's from the
        // PTSP pkt
        int tes = _rem.size();
        list_item lli;
        forall_items(lli, _rem){
          ig_ptse * oldone = _rem.inf(lli);
          rPtsp->RemPTSE(oldone);
        }
      }
      // Added Aug 25
      if (!((sPtsp->GetElements()).empty())) {
        // Send our more recent ptsp's
        diag("fsm.nodepeer", DIAG_DEBUG, 
	     "%s: Sending our PTSEs which are more recent than the PTSE of \
the other node.\n",
	     OwnerName());
	diag("fsm.nodepeer", DIAG_DEBUG, "%s: Started PTSE Request Timer.\n", 
	     OwnerName());
        PTSPPkt * rxPtsp = (PTSPPkt *)(sPtsp->copy());
        PTSPRxTimer *timer = new PTSPRxTimer(this, rxPtsp,  
					     _PTSEReqRxInterval);
        SendPTSPPkt(sPtsp);
        Register(timer);
	RXEntry * entry = new RXEntry(rxPtsp, timer); 
        _PTSPRxList.append(entry);
      } else 
	delete sPtsp;

      if (_PTSEAckpkt && _SendAck)
	SendAckPkt();

      DIAG("fsm.nodepeer", DIAG_DEBUG, 
	   cout << OwnerName() << ": Received a PTSPPkt " << endl;
	   cout << OwnerName() << ": " << *rPtsp << endl;);
      diag("fsm.nodepeer", DIAG_DEBUG, 
	   "%s: Inserting the more recent PTSPs into the Database.\n", 
	   OwnerName());

      if (!((rPtsp->GetElements()).empty())) {
	NPFloodVisitor * npv = new NPFloodVisitor(rPtsp, 0, _RemNodeID);
	PassVisitorToB(npv);
      } else
        delete rPtsp;
      return NodePeerState::fsm_ok;
      //    default:
      //      return NodePeerState::fsm_ok;
  }
  return NodePeerState::fsm_ok;
}

// RcvPTSEAckPkt(AckVisitor)
// Function: Remove the ptse's acknowledged from the
//             _PTSPRxlist. If all the ptse's in  a 
//             PTSP transmitted are received, then 
//             stop the ptsp retransmission timer. If
//             an acknowledgement for a dead ptse 
//             received, remove the ptse from the 
//             _death_row, which eventually cause it
//             to be wiped from the database 
int NodePeerState::RcvPTSEAckPkt(AckVisitor * v)
{
  assert(v);
  PTSEAckPkt * rApkt = v->GetAckPktptr();
  ig_nodal_ptse_ack * rAhr;

  diag("fsm.nodepeer", DIAG_DEBUG, "%s: Received Ack Packet.\n", OwnerName());
  DIAG("fsm.nodepeer", DIAG_DEBUG, cout <<  OwnerName() << ": " << *rApkt 
       << endl;);

  if (_PTSPRxList.empty()) {
    diag("fsm.nodepeer", DIAG_DEBUG, 
	 "%s: No PTSE in request list to remove!\n", OwnerName());
    return NodePeerState::fsm_ok;
  }
  
  AckContainer * rAcont;
  if (rApkt) {
    list_item li, li2;
    list<ig_nodal_ptse_ack *> & lst = 
      (list<ig_nodal_ptse_ack *> &)rApkt->GetNodalPTSEAcks();
    forall_items(li, lst) {
      rAhr = lst.inf(li);
      list<AckContainer *> & inner_lst = 
	(list<AckContainer *> &)rAhr->GetAcks();
      forall_items(li2, inner_lst) {
	rAcont = inner_lst.inf(li2);
        RemoveAckedRxPtses(rAcont, rAhr);  // THIS COULD BE VERY BAD!!!!
      }
    }
  } else {
    diag("fsm.nodepeer", DIAG_ERROR, "%s: Ack Packet with Null pointer!\n", 
	 OwnerName());
  }

  return NodePeerState::fsm_ok;
}

// RcvAddPort(PortUpVisitor)
// Function: This is an indicaction of a link being up
//           and called when a linkup visitor comes.
//           Processed as follows 
//           if NPdown: then state changes to negotiating
//           and the first empty DS Pkt is sent.
//           The portid is stored in _Port array
//           if anyother state: Port is just added to the _portID
//           array
int NodePeerState::RcvAddPort(PortUpVisitor *v)
{
  assert(v);
  switch (_State) {
    case NodePeerState::NpDown :
      diag("fsm.nodepeer", DIAG_DEBUG, "%s: Received AddPort in NPDOWN.\n", 
	   OwnerName());
      do_ds(v, 1);
      return NodePeerState::fsm_ok;
    case NodePeerState::Negotiating  :
    case NodePeerState::Exchanging   :
    case NodePeerState::Loading      :
      diag("fsm.nodepeer", DIAG_DEBUG, "%s: Received AddPort in %d\n",
	   OwnerName(), _State);
      do_ds(v, 7);
      return NodePeerState::fsm_ok;
    case NodePeerState::Full :
      diag("fsm.nodepeer", DIAG_DEBUG, "%s: Received AddPort in Full.\n", 
	   OwnerName());
      do_ds(v, 8);
      return NodePeerState::fsm_ok;
    default:
      return NodePeerState::fsm_ok;
  }
}

int NodePeerState::RcvSvccPortInfo(SVCCPortInfoVisitor *v)
{
  assert(v);
  if (v->PortUp()) {
    // AddPort Event
    switch (_State) {
      case NodePeerState::NpDown :
        diag("fsm.nodepeer", DIAG_DEBUG, "%s: Received AddPort in NPDOWN.\n", 
	     OwnerName());
        do_ds(v, 1);
        return NodePeerState::fsm_ok;
      case NodePeerState::Negotiating  :
      case NodePeerState::Exchanging   :
      case NodePeerState::Loading      :
        diag("fsm.nodepeer", DIAG_DEBUG, "%s: Received AddPort in %d\n", 
	     OwnerName(), _State);
        do_ds(v, 7);
        return NodePeerState::fsm_ok;
      case NodePeerState::Full :
        diag("fsm.nodepeer", DIAG_DEBUG, "%s: Received AddPort in Full.\n",
	     OwnerName());
        do_ds(v, 8);
        return NodePeerState::fsm_ok;
      default:
        return NodePeerState::fsm_ok;
    }
  } else {
    // Drop Port case
    assert(v);
    switch (_State) {
      case NodePeerState::NpDown :
        return NodePeerState::fsm_ok;
      default:
        diag("fsm.nodepeer", DIAG_DEBUG, "%s: Received DropPort.\n", 
	     OwnerName());
        do_ds(v, 9);
        return NodePeerState::fsm_ok;
    }
  }
  return NodePeerState::fsm_ok;
}

// RcvDropPort(PortDownVisitor)
// Function: Called when we get a linkup visitor 
//           has arrived. The corresponding port entry 
//           is removed from the _Port array. 
//           if the port is the last one, then the state is 
//           changed to Npdown. 
int NodePeerState::RcvDropPort(PortDownVisitor *v)
{
  assert(v);
  switch (_State) {
    case NodePeerState::NpDown :
      return NodePeerState::fsm_ok;
    default:
      diag("fsm.nodepeer", DIAG_DEBUG, "%s: Received DropPort.\n", 
	   OwnerName());
      do_ds(v, 9);
      return NodePeerState::fsm_ok;
  }
  return NodePeerState::fsm_ok;
}

//  RcvFloodPkt(NPFloodVisitor)
//  Function: This method is called when the 
//  database wants to flood more recent ptse
//  is inserted into it or when it wants to
//  Flush out a dead ptse. The database sends
//  the packets into NPFloodvisitor.
//  Processing involves 3 cases.
//  1. If the flooding is due to insertion of
//     a new ptse or a more recent ptse by this
//     FSM's peer itself, then there is no need
//     to flood it back the peer. The packet is
//     ignored.
//  2. If the ptse a new one or more recent one
//     being inserted by some other peer of this
//     node, then it is flooded to the peer of 
//     this fsm.
//  3. If it is a ptse either being emaciated
//     and flushed out or being reoriginated
//     by this node itself, it is flooded to 
//     the peer node
int NodePeerState::RcvFloodPkt(NPFloodVisitor * v)
{
  assert(v);

  const PTSPPkt * rPtsp = v->GetFloodPTSP();
  const NodeID  * rnid  = ((VPVCVisitor *)v)->GetDestNID();
  PTSPPkt       * dPtsp = 0;
 
  if ((_State == NodePeerState::NpDown) ||
      (_State == NodePeerState::Negotiating) ||
      (rnid && (*rnid == *_RemNodeID))) {
    diag("fsm.nodepeer", DIAG_DEBUG, "%s: PTSP %ld needs no flooding.\n",
	 OwnerName(), v);
    
    char buf[255];
    strcpy(buf,"");

    if (_State == NodePeerState::NpDown) {
      strcat(buf,".  _State == NodePeerState::NpDown");
    }

    if (_State == NodePeerState::Negotiating) {
      strcat(buf,".  _State == NodePeerState::Negotiating");
    }
    if (strcmp(buf,"")==0)
      AppendCommentToLog(buf);

    if (rnid && (*rnid == *_RemNodeID)) {      
      sprintf(buf, ".  %s == %s", rnid ? rnid->Print() : "(null)",
              _RemNodeID->Print());
    }
    AppendCommentToLog(buf);

    return NodePeerState::fsm_ok;
  }
  
  char buf[255];
  sprintf(buf, "About to flood PTSP since %s != %s", rnid ? rnid->Print() : "(null)",
	  _RemNodeID->Print());
  AppendCommentToLog(buf);

  if (rPtsp != 0) {
    DIAG("fsm.nodepeer", DIAG_DEBUG, cout <<
	 OwnerName() << ":: Received Flood PTSP Packet." << endl;
	 if (rnid) cout << OwnerName() << ": Flood pkt RemotenodeID" 
	 << *rnid << endl;
	 cout << OwnerName() << ": Taking action on FloodPacket." << endl);
    
    list<ig_ptse *> ptse_list = rPtsp->GetElements();
    list_item li;
    forall_items(li,  ptse_list) {
      ig_ptse * rPtse = ptse_list.inf(li);

      diag("fsm.nodepeer", DIAG_DEBUG, "%s processing PTSE of type %d.\n",
	   OwnerName(), rPtse->GetType());
      // Remove entry in the death list , because only flooding
      // adds an entry over there there will be one entry only
      if (rPtsp && rPtse)
        DelInDeathList(rPtsp->GetOID(), rPtse->GetID());
    
      // Remove entries in the Rxlist
      // May be Multiple one's
      if (rPtsp && rPtse)
        DelPTSEInPTSPRxList(rPtsp->GetOID(), rPtse->GetID());
      
      // Remove the entries in the Ack list
      // Make this a funciton.
      if (rPtsp && rPtse)
        DelInAckList(rPtsp->GetOID(), rPtse->GetID());
      
      if (rPtse && rPtse->GetTTL() == DBKey::ExpiredAge) {
        rPtse->Reference();
        if (!dPtsp)
          dPtsp = new PTSPPkt(rPtsp->GetOID(), rPtsp->GetPGID()); 
        
        dPtsp->AddPTSE(rPtse);
      }
    }
    // Append to the Rx list
    diag("fsm.nodepeer", DIAG_DEBUG,
	 "%s: Appending the flood PTSE to the Rx list list.\n", OwnerName());
    PTSPPkt * sPtsp  = (PTSPPkt *)rPtsp->copy();
    PTSPPkt * rxPtsp = (PTSPPkt *)sPtsp->copy(); 
    PTSPRxTimer * timer = new PTSPRxTimer(this, rxPtsp, _PTSEReqRxInterval);
    
    diag("fsm.nodepeer", DIAG_DEBUG, "%s: Sending PTSP %x.\n", OwnerName(), 
	 sPtsp);
    SendPTSPPkt(sPtsp);
    Register(timer);
    RXEntry * entry = new RXEntry(rxPtsp, timer);
    _PTSPRxList.append(entry);
    
    int test = _PTSPRxList.size();
    diag("fsm.nodepeer", DIAG_DEBUG, 
	 "%s: Number of PTSPs in PTSPRxlist list is %d.\n", OwnerName(), test);
    if (dPtsp) { 
      diag("fsm.nodepeer", DIAG_DEBUG, 
	   "Appending Dead ptse's in the death list \n", OwnerName()); 
      _death_row.append((PTSPPkt *)dPtsp);
    }
  } else {
    
    AppendCommentToLog("null ptsp pointer");

    diag("fsm.nodepeer", DIAG_ERROR, 
	 "%s: Received NPFlood with Null pointer to PTSP.\n", OwnerName());
    return NodePeerState::fsm_error;
  } 
  return NodePeerState::fsm_ok;
}

// do_ds(Visitor, code)
// Function: Here the code specifies
//           which action to be taken as in 
//           page 90, 91, 92 of specs related 
//           to table 5-12
void NodePeerState::do_ds(Visitor * v, int code)
{
  u_int RPID;
  list_item li;
  DatabaseSumPkt *lDpkt;
  NPStateVisitor * fsv_for_svc_hello; 
  NPStateVisitor * fsv_for_acac;

  diag("fsm.nodepeer", DIAG_DEBUG, 
       "%s: Doing Database Summary %d.\n", OwnerName(), code);
  switch (code) {
    case 1 :
      if (_NodeType == PhysicalNode) {
	if (((PortUpVisitor *)v)->GetInPort() > 0)
	  RPID = ((PortUpVisitor *)v)->GetInPort();
	else
	  RPID = ((PortUpVisitor *)v)->GetRemotePort();
	
	//	_PortID.insert(_NextPort, RPID);
	_PortID.append(RPID);
	//	_NextPort++;
      } else {
	_vpi = ((SVCCPortInfoVisitor *)v)->GetInVP();
	_vci = ((SVCCPortInfoVisitor *)v)->GetInVC();
      }
      // upon entering this state the node increments the DS Seq Num
      SetDSSequenceNum(_DSSequenceNum + 1);
      // it declares itself master
      _Mode = NodePeerState::Master;
      // and begins sending DS Summary packets with the Init, More,
      // and Master bits set
      lDpkt = new DatabaseSumPkt(_DSSequenceNum);
      lDpkt->SetBit(DatabaseSumPkt::init_bit);
      lDpkt->SetBit(DatabaseSumPkt::more_bit);
      lDpkt->SetBit(DatabaseSumPkt::master_bit);
      // Transition to the Negotiating phase
      diag("fsm.nodepeer", DIAG_DEBUG,"%s: going to Negotiating @line = %d\n", OwnerName(),__LINE__);
      ChangeState( NodePeerState::Negotiating );
      diag("fsm.nodepeer", DIAG_DEBUG, "%s: State is Negotiating.\n", 
	   OwnerName());
      SetDSPkt(lDpkt); 
      SendDSPkt();
      break;
      
    case 2 :
      SendDSPkt();
      break;

    case 3 :
      Cancel(_DSRxTimer);
      diag("fsm.nodepeer", DIAG_DEBUG, "%s: DSTimer Cancelled.\n", 
	   OwnerName());
      diag("fsm.nodepeer", DIAG_DEBUG,"%s: going to Loading @line = %d\n", OwnerName(),__LINE__);
      ChangeState( NodePeerState::Loading );
      diag("fsm.nodepeer", DIAG_DEBUG, "%s: State is Loading.\n", 
	   OwnerName());
      break;

    case 4 :
      {
      Cancel(_DSRxTimer);
      diag("fsm.nodepeer", DIAG_DEBUG, "%s: DSTimer Cancelled.\n",
	   OwnerName());
      diag("fsm.nodepeer", DIAG_DEBUG,"%s: going to Full @line = %d\n", OwnerName(),__LINE__);
      ChangeState( NodePeerState::Full );

      DBNodePeerInterface * dbi = (DBNodePeerInterface *)QueryInterface( "Database" );
      assert( dbi != 0 && dbi->good() );
      dbi->Reference();
      dbi->FlushDeadPTSES( );
      dbi->Unreference();

      DIAG("fsm.nodepeer", DIAG_INFO, cout << 
	   "NodePeer Full State from " << *(_LocNodeID) << endl <<
           " to " << *(_RemNodeID) << endl);
      }
      break;

    case 5 :
      Cancel(_AckTimer);
      diag("fsm.nodepeer", DIAG_DEBUG, 
	   "%s: AckTimer, DSTimer, and ReqTimer Cancelled.\n", OwnerName());
      Cancel(_DSRxTimer);
      Cancel(_ReqRxTimer);
      StopAllPTSETimers();
      ClearDeathRow();
      if (_PTSEAckpkt) 
	delete _PTSEAckpkt;
      if (_PTSEReqRxpkt) 
	delete _PTSEReqRxpkt;
      if (_DSPktOutStanding) 
	delete _DSPktOutStanding;
      _PTSEAckpkt = 0L;
      _PTSEReqRxpkt = 0L;
      _DSPktOutStanding = 0L;
      lDpkt = new DatabaseSumPkt(_DSSequenceNum);
      SetDSPkt(lDpkt);
      lDpkt->SetBit(DatabaseSumPkt::init_bit);
      lDpkt->SetBit(DatabaseSumPkt::more_bit);
      lDpkt->SetBit(DatabaseSumPkt::master_bit);
      if (_State == NodePeerState::Full) 
	do_ds(0, 6);
      else if (_NodeType == LogicalNode) {
        fsv_for_svc_hello =
          new NPStateVisitor(NPStateVisitor::BadPTSEReq ,_LocNodeID,
			     _RemNodeID,0);
        PassVisitorToA(fsv_for_svc_hello);
      }
      diag("fsm.nodepeer", DIAG_DEBUG,"%s: going to Negotiating @line = %d\n", OwnerName(),__LINE__);
      ChangeState( NodePeerState::Negotiating );
      diag("fsm.nodepeer", DIAG_DEBUG, "%s: State is Negotiating.\n", 
	   OwnerName());
      SendDSPkt();
      break;

   case 6 :
     // Send BadPTSE to SVC hello &&
     // NonFullState to acac
     if (_NodeType == LogicalNode) {
       fsv_for_svc_hello =
         new NPStateVisitor(NPStateVisitor::BadPTSEReq ,_LocNodeID,
  			    _RemNodeID,0);
       PassVisitorToA(fsv_for_svc_hello);
     } else {
       fsv_for_acac =
         new NPStateVisitor(NPStateVisitor::NonFullState, _LocNodeID,
			  _RemNodeID,0);
       PassVisitorToB(fsv_for_acac);
     }
     break;
     
   case 7 :
     if (_NodeType == PhysicalNode) {
       //       _PortID.insert(_NextPort, ((PortUpVisitor *)v)->GetInPort());
       //       _NextPort++;
       _PortID.append(((PortUpVisitor *)v)->GetInPort());
     }
     break;

   case 8 :
     do_ds(v, 7);
     // A new ptse needs to be originated & happens
     // in Acac.
     break;
   
   case 9 :
     if (_NodeType == PhysicalNode) {
       if (((PortDownVisitor *)v)->GetInPort() > 0)
         RPID = ((PortDownVisitor *)v)->GetInPort();
       else
         RPID = ((PortDownVisitor *)v)->GetRemotePort();

       if (li = _PortID.lookup(RPID))
	 _PortID.del_item(li);

       if (_State == NodePeerState::Full) {
         // Reorigination of ptse -acac,
       }
       //       if (_NextPort == 0)
       if (_PortID.empty())
         do_ds(v, 10);
     } else {
       _vpi = 0;
       _vci = 0;
       do_ds(v, 10);
     }
     break;

   case 10 :
     Cancel(_AckTimer);
     diag("fsm.nodepeer", DIAG_DEBUG, 
	  "%s: AckTimer, DSTimer, and ReqTimer Cancelled.\n", OwnerName());
     Cancel(_DSRxTimer);
     Cancel(_ReqRxTimer);
     StopAllPTSETimers();
     ClearDeathRow(); 
     if (_PTSEAckpkt) 
       delete _PTSEAckpkt;
     if (_PTSEReqRxpkt) 
       delete _PTSEReqRxpkt;
     if (_DSPktOutStanding) 
       delete _DSPktOutStanding;
     _PTSEAckpkt = 0L;
     _PTSEReqRxpkt = 0L;
     _DSPktOutStanding = 0L;

     if (_NodeType == PhysicalNode) {
       fsv_for_acac =
         new NPStateVisitor(NPStateVisitor::NonFullState, _LocNodeID,
  			    _RemNodeID,0);
       PassVisitorToB(fsv_for_acac);
     }
     
     diag("fsm.nodepeer", DIAG_DEBUG,"%s: going to NpDown @line = %d\n", OwnerName(),__LINE__);
     ChangeState( NodePeerState::NpDown );
     diag("fsm.nodepeer", DIAG_DEBUG, "%s: State is NPDOWN.\n", OwnerName());
  }
}

// MasterProcessDS(DatabaseSumPkt *)
// Function: This method implements the processing of
//           the database summary packets as in page 94
//           of the specs under the subtitle Exchanging
//           For a node acting as a master in summary exchange
//           Major features include 
//           1. To see if more summary packets need to come.
//           2. If all the summary packets are received 
//              get into the Loading state if all the ptsp's
//              requested have not yet arrived or Full state
//              if we have all the ptsp's that we requested from 
//              peer.
void NodePeerState::MasterProcessDS(DatabaseSumPkt *rDpkt)
{
  diag("fsm.nodepeer", DIAG_DEBUG, "%s: Master is processing DS Packet.\n", 
       OwnerName());

  if (_DSSequenceNum == rDpkt->GetSequenceNum()) {
    if (!_PTSEReqRxpkt) {
      DBNodePeerInterface * dbi = (DBNodePeerInterface *)QueryInterface( "Database" );
      assert( dbi != 0 && dbi->good() );
      dbi->Reference();
      SetReqPkt( dbi->ReqDiffDB(rDpkt, _RemNodeID) );
      dbi->Unreference();
      if (_PTSEReqRxpkt) 
	SendReqPkt();
    } else {
      DBNodePeerInterface * dbi = (DBNodePeerInterface *)QueryInterface( "Database" );
      assert( dbi != 0 && dbi->good() );
      dbi->Reference();
      PTSEReqPkt * sRpkt = dbi->ReqDiffDB(rDpkt, _RemNodeID);
      dbi->Unreference();
      if (sRpkt) {
	_PTSEReqRxpkt->AddMoreReq(sRpkt);
	delete sRpkt;
      }
    }
    // Need to split this one into 2 conditions
    // where if 1 0r second condtions are not
    // true we need to still in exchanging.
    assert(rDpkt);
    
    if (!(rDpkt->IsSet(DatabaseSumPkt::more_bit)) &&
	!(_DSPktOutStanding->IsSet(DatabaseSumPkt::more_bit))) {
      if (!(_PTSEReqRxpkt)) {
        do_ds(0, 4);

	if ( _State == Full ) {
	  if (_NodeType == LogicalNode) {
	    DIAG("fsm.nodepeer", DIAG_DEBUG, cout << 
		 "Logical NodePeer " << *(_LocNodeID) <<
		 " got into Full State, " << endl << 
		 "     Sending NPFull to RCCHello." << endl);
	    
	    NPStateVisitor * fsv_for_svc_hello =
	      new NPStateVisitor(NPStateVisitor::FullState,
				 _LocNodeID, _RemNodeID, 0);
	    PassVisitorToA(fsv_for_svc_hello);
	  }
	  
	  // Both the logical and physical need to notify the ElectionFSM
	  DIAG("fsm.nodepeer", DIAG_DEBUG, cout <<
	       "NodePeer " << *(_LocNodeID) <<
	       " got into Full State, " << endl << 
	       "     Sending NPFull to Election and ACAC." << endl);
	  
	  NPStateVisitor * fsv_for_acac =
	    new NPStateVisitor(NPStateVisitor::FullState,
			       _LocNodeID, _RemNodeID, 0);
	  PassVisitorToB(fsv_for_acac);
	}
        return;
      } else {
        do_ds(0, 3);
        return;
      }
    }
    
    SetDSSequenceNum(_DSSequenceNum+1);
    if (!(_DSPktOutStanding->IsSet(DatabaseSumPkt::more_bit))) {
      _DSPktOutStanding->SetSequenceNum(_DSSequenceNum);
      SendDSPkt();
      return;
    } else {
      delete _DSPktOutStanding;
      _DSPktOutStanding = 0L;
      
      DBNodePeerInterface * dbi = (DBNodePeerInterface *)QueryInterface( "Database" );
      assert( dbi != 0 && dbi->good() );
      dbi->Reference();
      SetDSPkt( dbi->GetDatabaseSummary(_level) );
      dbi->Unreference();
      
      if (_DSPktOutStanding) {
	// May need to set the flags(more) in future
	// if we send DS pkts in chunks
	_DSPktOutStanding->SetSequenceNum(_DSSequenceNum);
	// ONLY the MASTER should be calling this function ...
	_DSPktOutStanding->SetBit(DatabaseSumPkt::master_bit);
	SendDSPkt();
	return;
      } else {
	diag("fsm.nodepeer", DIAG_ERROR, 
	     "%s: No DS Packet from the Database.\n", OwnerName());
	return;
      }
    }
  } else if ((_DSSequenceNum-1) == rDpkt->GetSequenceNum()) {
    Register(_DSRxTimer);
    return;
  }
  // DS MISMatch
  SetDSSequenceNum((rDpkt->GetSequenceNum()+1));
  do_ds(0, 5);
  return;
}

// SlaveProcessDS(DatabaseSumPkt *)
// Function: This method implements the processing of
//           the database summary packets as in page 94
//           of the specs under the subtitle Exchanging
//           For a node acting as a slave in summary exchange
//           Major features include
//           1. To see if more summary packets need to come.
//           2. If all the summary packets are received
//              get into the Loading state if all the ptsp's
//              requested have not yet arrived or Full state
//              if we have all the ptsp's that we requested from
//              peer.
void NodePeerState::SlaveProcessDS(DatabaseSumPkt *rDpkt)
{
  diag("fsm.nodepeer", DIAG_DEBUG, "%s: Slave is processing DS Packet.\n", 
       OwnerName());

  if (_DSSequenceNum == (rDpkt->GetSequenceNum()-1)) {
    diag("fsm.nodepeer", DIAG_DEBUG, "%s: DSTimer Cancelled.\n", OwnerName());

    if (!_PTSEReqRxpkt) {
      DBNodePeerInterface * dbi = (DBNodePeerInterface *)QueryInterface( "Database" );
      assert( dbi != 0 && dbi->good() );
      dbi->Reference();
      SetReqPkt( dbi->ReqDiffDB(rDpkt, _RemNodeID) );
      dbi->Unreference();
      if (_PTSEReqRxpkt) 
	SendReqPkt();
    } else {
      DBNodePeerInterface * dbi = (DBNodePeerInterface *)QueryInterface( "Database" );
      assert( dbi != 0 && dbi->good() );
      dbi->Reference();
      PTSEReqPkt * sRpkt = dbi->ReqDiffDB(rDpkt, _RemNodeID);
      dbi->Unreference();
      if (sRpkt) {
	_PTSEReqRxpkt->AddMoreReq(sRpkt);
	delete sRpkt;
      }
    }

    if (_DSPktOutStanding->IsSet(DatabaseSumPkt::more_bit)) {
      delete _DSPktOutStanding;
      _DSPktOutStanding = 0L;
      SetDSSequenceNum(rDpkt->GetSequenceNum());

      DBNodePeerInterface * dbi = (DBNodePeerInterface *)QueryInterface( "Database" );
      assert( dbi != 0 && dbi->good() );
      dbi->Reference();
      SetDSPkt( dbi->GetDatabaseSummary(_level) );
      dbi->Unreference();

      if (!_DSPktOutStanding)
	_DSPktOutStanding = new DatabaseSumPkt(_DSSequenceNum);

      _DSPktOutStanding->SetSequenceNum(_DSSequenceNum);
      _DSPktOutStanding->RemBit(DatabaseSumPkt::init_bit);
      _DSPktOutStanding->RemBit(DatabaseSumPkt::master_bit);
      _DSPktOutStanding->RemBit(DatabaseSumPkt::more_bit);
    } else { 
      SetDSSequenceNum(rDpkt->GetSequenceNum());
      _DSPktOutStanding->SetSequenceNum(_DSSequenceNum);
    }
    SendDSPkt();
    
    if (!(rDpkt->IsSet(DatabaseSumPkt::more_bit)) &&
	!(_DSPktOutStanding->IsSet(DatabaseSumPkt::more_bit))) {
      if (!(_PTSEReqRxpkt)) {
	do_ds(0, 4);

	if ( _State == Full ) {
	  if (_NodeType == LogicalNode) {
	    DIAG("fsm.nodepeer", DIAG_DEBUG, cout << 
		 "Logical NodePeer " << *(_LocNodeID) <<
		 " got into Full State, " << endl <<
		 "     Sending NPFull to RCCHello." << endl);
	    
	    NPStateVisitor * fsv_for_svc_hello =
	      new NPStateVisitor(NPStateVisitor::FullState,
				 _LocNodeID, _RemNodeID, 0);
	    PassVisitorToA(fsv_for_svc_hello);
	  }
	  // Both log and phy need to notify election
	  DIAG("fsm.nodepeer", DIAG_DEBUG, cout << 
	       "NodePeer " << *(_LocNodeID) <<
	       " got into Full State," << endl <<
	       "     Sending NPFull to Election and ACAC." << endl);
	  
	  NPStateVisitor * fsv_for_acac =
	    new NPStateVisitor(NPStateVisitor::FullState,
			       _LocNodeID, _RemNodeID, 0);
	  PassVisitorToB(fsv_for_acac);
	}
	return;
      } else {
	do_ds(0, 3);
	return;
      }
    }
    return;
  } else if (_DSSequenceNum == rDpkt->GetSequenceNum()) {
    SendDSPkt();
    return;
  }
  // DS MisMatch
  SetDSSequenceNum((rDpkt->GetSequenceNum()+1));
  do_ds(0, 5);
  return;
}

// ExpireDSTimer()
// Function: Called when summary pkt response time expired
//           Just resend the summary packet from this side
void NodePeerState::ExpireDSTimer(void)
{
  diag("fsm.nodepeer", DIAG_DEBUG, "%s: DSPacket timer expired.\n", 
       OwnerName());
  
  if (!_DSPktOutStanding) 
    return;
  SendDSPkt();
}

// ExpireReqTimer()
// Function: Called when request pkt response time expired
//           Just resend the summary packet from this side
void NodePeerState::ExpireReqTimer(void)
{
  diag("fsm.nodepeer", DIAG_DEBUG, "%s: Request packet timer expired.\n", 
       OwnerName());

  if (_State == Full) 
    return;

  if (!_PTSEReqRxpkt) {
    diag("fsm.nodepeer", DIAG_DEBUG, 
	 "%s: Request timer expired without packet!\n", OwnerName());
    return;
  }
  SendReqPkt();
}

// ExpireDSTimer()
// Function: Called when ptsp pkt response time expired
//           Just resend the summary packet from this side
void NodePeerState::ExpirePTSPTimer(PTSPPkt * rxPtsp, PTSPRxTimer *timer)
{
  diag("fsm.nodepeer", DIAG_DEBUG, "%s: PTSP Packet Timer expired.\n", 
       OwnerName());

  assert(rxPtsp);
  SendPTSPPkt((PTSPPkt *)rxPtsp->copy());
  Register(timer);
}

// ExpireDSTimer()
// Function: Called when periodic acknowledgement timer expired
//           Just the ack packet
void NodePeerState::ExpireAckTimer(void)
{
  diag("fsm.nodepeer", DIAG_DEBUG, "%s: Ack Packet Timer expired.\n", 
       OwnerName());

  if (!_PTSEAckpkt) 
    return;
  SendAckPkt();
}

// SendDSPkt()
// Function: SendDatabaseSummaryPacket
void NodePeerState::SendDSPkt(void)
{
  DIAG("fsm.nodepeer", DIAG_DEBUG, 
       cout << OwnerName() << ": Sending DSPkt" << endl;
       cout << OwnerName() << ": " << *_DSPktOutStanding;
       cout << OwnerName() << ": From " << *_LocNodeID << " to " << endl 
       << *_RemNodeID << endl;
       cout << OwnerName() << ": The sequence number is " 
       << _DSPktOutStanding->GetSequenceNum() << endl;);

  // Send a Copy  Added Aug 17 b
  DatabaseSumPkt * sDpkt = (DatabaseSumPkt *)(_DSPktOutStanding->copy());

  if (_State != Loading && _State != Full)
    Register(_DSRxTimer);

  u_int val = 0;
  // Why is this hardcoded to PortID[0]
  list_item li;
  //  if ( di = _PortID.lookup(0) )
  if (li = _PortID.first())
    val =  _PortID.inf( li );

  DSVisitor * v = new DSVisitor(_LocNodeID, _RemNodeID, sDpkt, val);
  if (_NodeType == PhysicalNode) {
    v->SetOutPort(val); v->SetOutVP(0); v->SetOutVC(18);
    v->SetInPort(0);  v->SetInVP(0);  v->SetInVC(18);
  } else {
    v->SetInPort(0); v->SetInVP(_vpi); v->SetInVC(_vci); v->SetCREF(_cref);
  }
  PassVisitorToA(v);
}

// SendReqPkt()
// Function: SendPTSERequestPacket
void NodePeerState::SendReqPkt(void)
{
  DIAG("fsm.nodepeer", DIAG_DEBUG, 
       cout << OwnerName() << ": Sending ReqPkt" << endl;
       cout << OwnerName() << ": " << *_PTSEReqRxpkt << endl; 
       cout <<  OwnerName() << ": Reqtimer Started" << endl;);

  u_int val = 0;
  list_item li;
  //  if (di = _PortID.lookup(0))
  if (li = _PortID.first())
    val = _PortID.inf( li );
  ReqVisitor * v = new ReqVisitor(_LocNodeID, _RemNodeID,_PTSEReqRxpkt, val);
  if (_NodeType == PhysicalNode) {
    v->SetOutPort(val); v->SetOutVP(0); v->SetOutVC(18);
    v->SetInPort(0);  v->SetInVP(0);  v->SetInVC(18);
  } else {
    v->SetInPort(0); v->SetInVP(_vpi); v->SetInVC(_vci); v->SetCREF(_cref);
  }
  PassVisitorToA(v); 
  Register(_ReqRxTimer); 
}

// SendPTSPPkt()
// Function: Send PTSP Packet
void NodePeerState::SendPTSPPkt(PTSPPkt *sPtsp)
{
  DIAG("fsm.nodepeer", DIAG_DEBUG, 
       cout << OwnerName() << ": Sending PTSPPkt: " << sPtsp << endl;
       cout << OwnerName() << ": " << *(sPtsp) << endl;);

  //  this PTSP visitor doesn't have 
  //  anything to hold port. Why was it designed? 
  //  We can create a new one otherwise.
  u_int val = 0;
  list_item li;
  //  if (di = _PortID.lookup(0))
  if (li = _PortID.first())
    val = _PortID.inf( li );
  PTSPVisitor * v = new PTSPVisitor(_LocNodeID, _RemNodeID,sPtsp, val);
  if (_NodeType == PhysicalNode) {
    v->SetOutPort(val); v->SetOutVP(0); v->SetOutVC(18);
    v->SetInPort(0);  v->SetInVP(0);  v->SetInVC(18);
  } else {
    v->SetInPort(0); v->SetInVP(_vpi); v->SetInVC(_vci); v->SetCREF(_cref);
  }
  PassVisitorToA(v);
}

// SendPTSPPkt()
// Function: SendPTSPAcknowledgementPacket
void NodePeerState::SendAckPkt(void)
{
  DIAG("fsm.nodepeer", DIAG_DEBUG, 
       cout << OwnerName() << ": Sending AckPkt" << endl;
       cout << OwnerName() << ": " << *_PTSEAckpkt << endl;);

  u_int val = 0;
  list_item li;
  //  if (di = _PortID.lookup(0))
  if (li = _PortID.first())
    val = _PortID.inf( li );
  AckVisitor * v = new AckVisitor(_LocNodeID, _RemNodeID,_PTSEAckpkt, val);
  if (_NodeType == PhysicalNode) {
    v->SetOutPort(val); v->SetOutVP(0); v->SetOutVC(18);
    v->SetInPort(0);  v->SetInVP(0);  v->SetInVC(18);
  } else {
    v->SetInPort(0); v->SetInVP(_vpi); v->SetInVC(_vci); v->SetCREF(_cref);
  }
  PassVisitorToA(v);
  _SendAck = false;
  _PTSEAckpkt = 0L;
}

// IsBadRequest(onid, ptse)
// Function: See if the ptse we received from the
//           the peer is equal or more recent than the
//           one we requested by checking in the request list
//           If so remove the request. If the request list 
//           is empty turn to Full state. Return a false indicating
//           that the ptse is a more recent one If ptse is less
//           recent than the requested one, then a return a  true
//           indicating a bad request
bool NodePeerState::IsBadRequest(const u_char * onid, ig_ptse * rPtse)
{   
  if (!_PTSEReqRxpkt) 
    return false;

  assert(onid && rPtse);

  ig_req_ptse_header * lRhr = _PTSEReqRxpkt->Search(onid);
  // PTSEReqPkt::Search(rnid)
  if (!lRhr)
    return false;

  ReqContainer * lRcont = lRhr->Search(rPtse->GetID());
  if (lRcont) {
    if (*(lRcont) > *(rPtse)) {
      diag("fsm.nodepeer", DIAG_DEBUG, "%s: A bad PTSE request.\n", 
	   OwnerName());
      return true;
    }
    diag("fsm.nodepeer", DIAG_DEBUG, "%s: PTSE request is alright.\n",
	 OwnerName());

    lRhr->RemReqContainer(lRcont);
    // Does that mean that lRhr has deleted lRcont?  just looked at
    // the code in req_ptse_header.cc, No it doesn't - mountcas
    delete lRcont;
    
    if ((lRhr->GetReqSummary()).empty())
      _PTSEReqRxpkt->RemNodalPTSEReq(lRhr);
    if ((_PTSEReqRxpkt->GetHeaders()).empty()) { 
      Cancel(_ReqRxTimer);
      diag("fsm.nodepeer", DIAG_DEBUG, "%s: Request Timer Cancelled.\n",
	   OwnerName());
      delete _PTSEReqRxpkt;
      _PTSEReqRxpkt = 0L;

      if (_State == NodePeerState::Loading) {
	// we definitely change to Full state in ds 4
	do_ds(0, 4); 

	if ( _State == Full ) {
	  if (_NodeType == LogicalNode) {
	    DIAG("fsm.nodepeer", DIAG_DEBUG, cout << 
		 "Logical NodePeer " << *(_LocNodeID) << 
		 " got into Full State," << endl <<
		 "     Sending NPFull to RCCHello." << endl);
	    
	    NPStateVisitor * fsv_for_svc_hello =
	      new NPStateVisitor(NPStateVisitor::FullState, _LocNodeID,
				 _RemNodeID,0);
	    PassVisitorToA(fsv_for_svc_hello);
	  }
	  DIAG("fsm.nodepeer", DIAG_DEBUG, cout << 
	       "NodePeer " << *(_LocNodeID) <<
	       " got into Full State," << endl <<
	       "     Sending NPFull to Election and ACAC." << endl);
	  
	  NPStateVisitor * fsv_for_acac =
	    new NPStateVisitor(NPStateVisitor::FullState, _LocNodeID,
			       _RemNodeID,0);
	  PassVisitorToB(fsv_for_acac);
	}
      } 
    }
  }
  return false;
}

// CheckPTSPRxlistForCase3(ourPtse, hisPtse ptsp)
// Function: To see if the ptse obtained is more recent
//           than one we have in our database. If so
//           put our ptse in the ptse list to be sent
//           to the peer. 
void NodePeerState::CheckPTSPRxlistForCase3(ig_ptse * lPtse, 
					    ig_ptse * rPtse, PTSPPkt * sPtsp)
{
  list_item li;
  bool present;
  present = 0;

  diag("fsm.nodepeer", DIAG_DEBUG, 
       "%s: Doing 5.8.3.3 section 3. our PTSE is more recent.\n", OwnerName());

  forall_items(li, _PTSPRxList) {
    RXEntry * entry = _PTSPRxList.inf(li);
    PTSPPkt * lPtsp = entry->GetPTSP();
    if (*(lPtsp) == *(sPtsp)) {
      list_item li;
      list<ig_ptse *> & lst = (list<ig_ptse *> &)lPtsp->GetElements();
      forall_items(li, lst){ 
	ig_ptse * tmp = lst.inf(li);
        // Only PTSE ID is checked while comparison
        // As Rx list contains latest one's in DB
        if (tmp->GetID() == lPtse->GetID()) {
          present = 1;
          break;
        }
      }
      if (present) {
        _SendAck = true;
        if (!_PTSEAckpkt) {
          _PTSEAckpkt = new PTSEAckPkt();
          Register(_AckTimer);
	  diag("fsm.nodepeer", DIAG_DEBUG, "%s: AckTimer Started.\n",
	       OwnerName());
        }
        _PTSEAckpkt->AddNodalPTSEAck(sPtsp->GetOID(), rPtse);
        return;
      }
    }
  }
  ig_ptse * sPtse = (ig_ptse *)(lPtse->copy());
  if ((sPtse->GetTTL()) != DBKey::ExpiredAge) 
    sPtse->SetTTL((lPtse->GetTTL())-1);
  sPtsp->AddPTSE(sPtse);
}

// CheckPTSPRxlistForCase4(ourptse, peerptse, ptsp)
// Funtion: To see if the ptse received is equal to one we
//          have in the  our database. if so see if the
//          ptse is in the retransmission list. If so remove
//          it, otherwise send an acknowledgement. 
//          This is as in page 108 section 5.8.3.3 9 under (4)
void NodePeerState::CheckPTSPRxlistForCase4(ig_ptse * lPtse,
					    ig_ptse * rPtse, PTSPPkt * sPtsp)
{
  diag("fsm.nodepeer", DIAG_DEBUG,
       "%s: Doing 5.8.3.3 section 4: are PTSEs equal?\n", OwnerName());
  assert(lPtse && rPtse && sPtsp);

  list_item li;
  list_item lli;
  forall_items(li, _PTSPRxList) {
    RXEntry * entry  = _PTSPRxList.inf(li);
    PTSPPkt * rxPtsp = entry->GetPTSP();
    if (*(rxPtsp) == *(sPtsp)) {
      ig_ptse * rxPtse;
      forall_items(lli, (rxPtsp->GetElements())){
        rxPtse = (rxPtsp->GetElements()).inf(lli);
        if (rxPtse->GetID() == lPtse->GetID()){
          // delete in death list if expired.
          if (rxPtse->GetTTL() ==  DBKey::ExpiredAge)
            DelInDeathList(rxPtsp->GetOID(), rxPtse->GetID());
	  // Delete fromRx list
	  rxPtsp->RemPTSE(rxPtse);
	  if ((rxPtsp->GetElements()).empty()) {
	    PTSPRxTimer * timer = entry->GetTimer();
	    Cancel(timer);
            delete entry;
	    _PTSPRxList.del_item(li);
	  }
          return;
        }
      }
    }
  }
  _SendAck = true;
  if (!_PTSEAckpkt) {  
    _PTSEAckpkt = new PTSEAckPkt();
    Register(_AckTimer);
    diag("fsm.nodepeer", DIAG_DEBUG, "%s: AckTimer started.\n", OwnerName());
  }
  _PTSEAckpkt->AddNodalPTSEAck(sPtsp->GetOID(), rPtse);
}

// DelInDeathList(onid, ptse id)
// Function: Remove the ptse with the ptse id in
// the _dealth_row by unreferencing the ptse
void NodePeerState::DelInDeathList(const u_char * onid, int pid)
{
  list_item li;
  forall_items(li, _death_row) {
    PTSPPkt * lPtsp = _death_row.inf(li);

    if (!memcmp((void *)lPtsp->GetOID(), (void *)onid, 22)) {
      const list<ig_ptse *> Plist = lPtsp->GetElements();

      list_item lli;
      forall_items(lli, Plist) {
        ig_ptse * lPtse = Plist.inf(lli);

        if (lPtse->GetID() == pid) {
          // Whenever we do remove in a
          // for loop make an immediate exit 
          lPtsp->RemPTSE(lPtse);
          if (lPtsp->GetElements().empty()) { 
	    diag("fsm.nodepeer", DIAG_DEBUG,
		 "%s: Removing an entry in death list.\n", OwnerName());
            delete lPtsp;
            _death_row.del_item(li);
          }
          return;
        }
      } // end of forall
    }
  } // end of forall
}
  
// DelPTSEInPTSPRxList(onid, ptse id)   
// Function: To provide the ptse with ptse id in
//           our retransmission list. Called when
//           a ptse which is more recent than one in the
//           retransmission list is flooded.
void NodePeerState::DelPTSEInPTSPRxList(const u_char * onid, int pid)
{
  list_item li;
  forall_items(li, _PTSPRxList) {
    RXEntry * entry = _PTSPRxList.inf(li);

    PTSPPkt * lPtsp = entry->GetPTSP();
    if (!memcmp((void *)lPtsp->GetOID(), (void *)onid, 22)) {

      list_item lli;
      forall_items(lli, lPtsp->GetElements()) {
	ig_ptse * lPtse = (lPtsp->GetElements()).inf(lli);
        // Replacing this to generate segv if possible
	// if (lPtse && lPtse->GetID() == pid) 
        if (lPtse->GetID() == pid){
	  lPtsp->RemPTSE(lPtse);
          // Clear PTSPRx & timer if the pkt is empty
          if ((lPtsp->GetElements()).empty()) {
	    diag("fsm.nodepeer", DIAG_DEBUG,
		 "%s: Removing an entry in PTSPRxlist list.\n", OwnerName());
	    PTSPRxTimer *timer = entry->GetTimer();
	    Cancel(timer);
            delete entry;
	    _PTSPRxList.del_item(li);
          }
          return;
	}
      } // end of forall
    }
  } // end of forall
}

// DelInAckList(u_char *, ptse id)
// Function: An acknowledgement for a 
//           ptse id is removed from the
//           ack list. Called when a more recent
//           ptse is being flooded from this node
void NodePeerState::DelInAckList(const u_char * onid, int pid)
{
  if (_PTSEAckpkt) {

    const list<ig_nodal_ptse_ack *> & nodal_list =
      _PTSEAckpkt->GetNodalPTSEAcks();

    if (nodal_list.empty() == false) {

      list_item li;
      forall_items(li, nodal_list) {
	ig_nodal_ptse_ack * lAhr = nodal_list.inf(li);

	assert( lAhr != 0 );

	NodeID lhs(lAhr->GetOID()), rhs(onid);
	if (lhs == rhs) {
	  const list<AckContainer *> & ack_list = lAhr->GetAcks();

	  if (ack_list.empty()) {
	    _PTSEAckpkt->RemNodalPTSEAck(lAhr);
	    // Once you remove this ig_nodal_ptse_ack from the list, the
	    // forall is UNSTABLE!!!
	    return;
	  } else {
	    list_item lli;
	    forall_items(lli, ack_list) {
	      AckContainer * lAcont = ack_list.inf(lli);
	      assert( lAcont != 0 );

	      if (lAcont->_ptse_id == pid) {
		diag("fsm.nodepeer", DIAG_DEBUG, "%s: Removing an entry in Ack list.\n", OwnerName());
		lAhr->RemAck(lAcont->_ptse_id, lAcont->_ptse_seq, 
			     lAcont->_ptse_checksum, lAcont->_ptse_rem_life);
		// Once you remove this AckContainer from the list, the
		// forall is UNSTABLE!!!
		return;
	      }
	    } // forall
	  }
	} else {
	  diag("fsm.nodepeer", DIAG_DEBUG, "%s: Ack list with null Ack header!\n",
	       OwnerName());
	}
      } // forall
    }
  }
}

// StopAllPTSETimers()
// Function: Stop all the ptse timers in the
//           Retransmission list and remove the
//           cooresponding entries during error scenarios
void NodePeerState::StopAllPTSETimers(void) 
{
  diag("fsm.nodepeer", DIAG_DEBUG, "%s: Stopping all PTSP Request Timers.\n", 
       OwnerName());
  list_item li; 
  forall_items(li, _PTSPRxList) {
    RXEntry * entry = _PTSPRxList.inf(li);
    PTSPRxTimer *timer = entry->GetTimer();
    Cancel(timer);
    delete entry;
  }
  _PTSPRxList.clear();
}

// RemoveAckedRxPtses()
// Function: When a ptse is acked, remove
//           the corresponding entry from
//           the PTSPRetransmission list
void NodePeerState::RemoveAckedRxPtses(AckContainer * rAcont, 
				       ig_nodal_ptse_ack * rAhr)
{
  int test;
  PTSPPkt * lPtsp;
  ig_ptse * lPtse;
  list_item li;
  test = _PTSPRxList.size();
  diag("fsm.nodepeer", DIAG_DEBUG, 
       "%s: Number of packets in the PTSPRxlist list is %d.\n", 
       OwnerName(), test);

  if (_PTSPRxList.empty()) 
    return;
  diag("fsm.nodepeer", DIAG_DEBUG, 
       "%s: Removing the Acknowledged PTSEs from the request list.\n", 
       OwnerName());
  forall_items(li, _PTSPRxList) {
    RXEntry * entry = _PTSPRxList.inf(li);
    lPtsp = entry->GetPTSP();
    if (memcmp((void *)lPtsp->GetOID(), (void *)rAhr->GetOID(), 22) == 0) {
      list_item lli;
      forall_items(lli, lPtsp->GetElements()) {
        lPtse = (lPtsp->GetElements()).inf(lli);
        if (*(lPtse) == *(rAcont)) {
          if (lPtse->GetTTL() == DBKey::ExpiredAge)
	    DelInDeathList(lPtsp->GetOID(), lPtse->GetID());

          lPtsp->RemPTSE(lPtse);
          if ((lPtsp->GetElements()).empty()) {
            PTSPRxTimer * timer = entry->GetTimer();
            Cancel(timer);
            delete entry;
            _PTSPRxList.del_item(li);
	  }
          return;
	} // else continue;
      }
    } // continue
  }
}

void NodePeerState::ClearDeathRow(void)
{
  list_item li;
  forall_items(li, _death_row) {
    PTSPPkt * lPtsp = _death_row.inf(li);
    delete lPtsp;
  }
  _death_row.clear();
}

void NodePeerState::SetDSSequenceNum(u_int num)
{
  _DSSequenceNum = num;
  diag("fsm.nodepeer", DIAG_DEBUG, "%s: DS Sequence Number is %d.\n",
       OwnerName(), _DSSequenceNum);
}

void NodePeerState::SetDSPkt(DatabaseSumPkt *pkt)
{_DSPktOutStanding = pkt; }

void NodePeerState::SetReqPkt(PTSEReqPkt * lRpkt) 
{ _PTSEReqRxpkt = lRpkt;}

void NodePeerState::ChangeState( NodePeerState::NodePeerStates newState )
{
  if ( newState == NodePeerState::Exchanging ) {
    theNetStatsCollector().ReportNetEvent("NP_Exchanging",
					  OwnerName(),
					  0, _LocNodeID);
  } else if ( newState == NodePeerState::NpDown ) {
    theNetStatsCollector().ReportNetEvent("NP_Down",
					  OwnerName(),
					  0, _LocNodeID);
  } else if ( _State != NodePeerState::Full && newState == NodePeerState::Full ) {
    // Only print full if we are not already in Full
    theNetStatsCollector().ReportNetEvent("NP_Full",
					  OwnerName(),
					  0, _LocNodeID);
  }
  _State = newState;
}

int compare(RXEntry *const & lhs, RXEntry *const & rhs)
{
  PTSPPkt * lPtsp = lhs->_pp;
  PTSPPkt * rPtsp = rhs->_pp;
  int comp1 = memcmp((void *)lPtsp->GetOID(), (void *)rPtsp->GetOID(), 22);

  if (comp1 < 0) 
    return -1;
  if (comp1 > 0) 
    return 1; 
  return 0;
}

const char * NodePeerState::StateName(void) const
{
  switch ( _State ) {
    case NodePeerState::NpDown :
      return "NPDown";
      break;
    case NodePeerState::Negotiating :
      return "Negotiating";
      break;
    case NodePeerState::Loading :
      return "Loading";
      break;
    case NodePeerState::Exchanging:
      return "Exchanging";
      break;
    case NodePeerState::Full :
      return "Full";
      break;
  }
  return "!ERROR!";
}
