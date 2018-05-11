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
 * @file pluggable_port_info.h
 * @author talmage
 * @version $Id: pluggable_port_info.h,v 1.43 1999/01/22 16:11:17 talmage Exp $
 *
 * Purpose: pluggable_port_info maintains the state of one data port in a switch.
 */
#ifndef __PLUGGABLE_PORT_INFO_H__
#define __PLUGGABLE_PORT_INFO_H__

#ifndef LINT
static char const _pluggable_port_info_h_rcsid_[] =
"$Id: pluggable_port_info.h,v 1.43 1999/01/22 16:11:17 talmage Exp $";
#endif

class ACACPolicy;
class Addr;
class FastUNIVisitor;
class NodeID;
class PeerID;
class PNNI_crankback;
class UNI_40_traffic_desc;
class ie_bbc;
class ie_cause;
class ig_ptse;
class ig_resrc_avail_info;
class ig_uplink_info_attr;
class q93b_setup_message;
class UNI40_traffic_desc;

#include <sim/acac/ACACPolicy.h>
#include <fsm/nodepeer/NPVisitors.h>
#include <DS/containers/dictionary.h>
#include <DS/containers/sortseq.h>

extern u_int ServiceCategory(const ie_bbc * const bbc, 
			     const UNI40_traffic_desc * const td);

/**
 * pluggable_port_info keeps track of one physical port of a switch.
 * It knows how much bandwidth is in use and whether the
 * port is connected to another node in its peer group or to
 * another node in another peer group.
 */
class pluggable_port_info {
public:

  enum port_type {
    unspecified = 0,	// The port's type hasn't been determined yet.
    horizontal_link,	// The port is used as an HLINK.
    uplink		// The port is used as an UPLINK.
  };

  /** Constructor 
   * @param sharedACACPolicy A pointer to an object that implements a
   * call admission policy.  AllocateBandwidth() calls one of
   * the object's AdmitCall() methods to determine whether to admit
   * the call or not.  The same sharedACACPolicy might be used by any
   * number of pluggable_port_info objects, so none may delete it.
   *
   * @param up Set to true if the port is already up.  Set to false otherwise.
   *
   * @param type Tells whether the port is used as a horizontal link
   * or an uplink or whether that hasn't been determined yet.
   *
   * @param npstate Tells whether the port's Node Peer Finite State Machine
   * is in FullState or not.  No UPLINK or HLINK may be advertised for this
   * port until its NP FSM is in FullState.
   *
   * @param id The database identifier that corresponds to the
   * information about this port.  It is not necessarily the port number.
   *
   * @param localPort The number of the port on this end of the fiber;
   * the number of the port on this switch.
   *
   * @param remotePort The number of the port on the other end of the
   * fiber, at the remote switch or host.
   *
   * @param remoteNode The address of the host or switch at the other
   * end of the fiber.
   *
   * @param commonPeerGroup The PeerID of a border node.  It is zero
   * if the remote node is not a border node.
   *
   * @param aggregationToken 
   */
  pluggable_port_info(ACACPolicy *sharedACACPolicy,
		      bool up, enum port_type type, 
		      NPStateVisitor::NPStateVisitorType npstate, 
		      int id, 
		      u_int localPort, 
		      u_int remotePort, 
		      NodeID *remoteNode, 
		      PeerID *commonPeerGroup, 
		      int aggregationToken);
  
  /** Destructor
   */
  ~pluggable_port_info(void);

  /**
   * Delete the RAIGs for this port.
   */
  void ClearRAIGs(void);

  /**
   * Release all calls.  Empty the _Calls dictionary.
   */
  void ClearCalls(void);

  /**
   * Set the kind of port
   * @param type the type of the port
   */
  void PortType(enum port_type type);

  /**
   * What type of port is this?
   * @return the type
   */
  enum port_type PortType(void);

  /**
   * What is the ID of the port?  This is different from the port number.
   * @return the ID
   */
  int ID(void);

  /** 
   * Set the address of the local node.  Also sets the ATM
   * address of the local node.  Makes a copy of localNode.
   *
   * @param localNode the node address
   */
  void LocalNode(NodeID *localNode);

  /**
   * What is the ID of the local node?
   * @return the local NodeID
   */
  NodeID *LocalNode(void) const;

  /**
   * What is the number of the port?  This is different from the port ID.
   * @return the port number
   */
  u_int LocalPort(void);

  /**
   * Set the number of the remote port.
   * @param remotePort the number of the remote port
   */
  void RemotePort(u_int remotePort);

  /**
   * What is the number of the remote port?
   * @return the remote port number
   */
  u_int RemotePort(void);

  /** 
   * Set the address of the remote node.  Also sets the ATM
   * address of the remote node.  Makes a copy of remoteNode.
   *
   * @param remoteNode the node address
   */
  void RemoteNode(const NodeID *remoteNode);

  /**
   * What is the ID of the remote node?
   * @return the remote NodeID
   */
  NodeID *RemoteNode(void) const;

  /**
   * Set the ATM address of the remote node.
   * @param remoteAddress the remote ATM address
   */
  void RemoteAddress(Addr *remoteAddress);

  /**
   * What is the ATM address of the remote node?
   * @return the ATM address
   */
  Addr *RemoteAddress(void) const;

  /** Set the PeerID of that this node shares with the remote node.
   * This node and the remote node are border nodes.
   *
   * @param commonPeerGroup the PeerID
   */
  void CommonPeerGroup(const PeerID *commonPeerGroup);

  /** What is the peer group that this node shares with the remote node?
   *
   * @return the peer group id.
   */
  PeerID *CommonPeerGroup(void) const;
    
  /**
   * Set the aggregation token for the port.
   * @param aggregationToken the aggregation token
   */
  void AggregationToken(int aggregationToken);

  /**
   * What is the aggregation token for the port?
   * @return the aggregation token
   */
  int AggregationToken(void);

  /** 
   * Set the status of the NodePeer Finite State Machine that
   * controls this port.  CACRoutingFSM knows to originate PTSE's only
   * when the port is in FullState.
   *
   * @param npstate the state of the NP FSM 
   */
  void NPState(NPStateVisitor::NPStateVisitorType npstate);

  /**
   * What is the status of the NodePeer Finite State Machine that
   * controls this port?
   *
   * @return the state of the NP FSM
   */
  NPStateVisitor::NPStateVisitorType NPState(void);

  /**
   * Mark the port up or down.
   *
   * @param up the state of the port.  true == up.  false == down
   */
  void Up(bool up);

  /**
   * Is the port up?
   *
   * @return the state of the port.  true == up.  false == down
   */
  bool Up(void);

  /**
   * Set the bit that means some of the port's information has changed.
   *
   * true means that something changed.
   *
   * All of the methods that set an aspect of the port set the dirty
   * bit to true.
   *
   * @param dirty the state of the dirty bit.  true == dirty.  false
   * == not dirty.  
   */
  void Dirty(bool dirty);

  /**
   * Has the port's information has changed?
   *
   * true means that something changed.
   *
   * @return the cleanliness of the port's info.  true == dirty.
   */
  bool Dirty(void);


  /**
   * Return the next available call reference value.  If there is
   * one, it will be in the range _min_call_reference to
   * _max_call_reference, inclusive.  If there is not one, then
   * NextCallReference() returns zero.  
   */
  u_int NextCallReference(void);

  /** AllocateBandwidth() allocates bandwidth for the
   * preceeding end of the following link.  See also PNNI 1.0, page
   * 240, the section about crankback.
   *
   * If there is at least bandwidth available, then
   * AllocateBandwidth() decrements the bandwidth available,
   * increments the bandwidth used, and looks for a new call reference
   * number.  If it finds one, AllocateBandwidth() returns it.
   * AllocateBandwidth() also sets the service category parameter.
   *
   * If there isn't enough bandwidth available or if all call
   * references are in use, AllocateBandwidth() returns zero.
   *
   * @param setup the Q93B SETUP message that describes this call.
   *
   * @param inPort the port on which the call setup arrived
   *
   * @param outPort the port on which the call setup will leave
   *
   * @param crankback_level the level of the NodeID on the top of the DTL
   *
   * @param cause If the call cannot be placed, cause points to the
   * ie_cause object that contains the reason.  Otherwise, it is not
   * changed.
   *
   * @param crankback Pass in a pointer with the value zero.  If the
   * call cannot be placed, crankback may point to the PNNI_crankback
   * object that contains the reason.  Otherwise, it is zero.  If
   * AllocateBandwidth supplies a crankback, it will be for
   * the preceeding end of the following link.
   *
   * @param callState AllocateBandwidth() sets this to the
   * value returned by the AdmitCall() method of the ACACPolicy
   * plugin.
   *
   * @return the call reference of the new call or zero if there is no
   * bandwidth or no free call references.  */
  ACACPolicy::CallStates
  AllocateBandwidth(q93b_setup_message * setup, 
		    u_int inPort, u_int outPort,
		    u_int inCREF, u_int &outCREF,
		    u_int inVPI, u_int inVCI,
		    pluggable_port_info *slavePort,
		    const u_char crankback_level, 
		    const NodeID * called_party, 
		    ie_cause *& cause,
		    PNNI_crankback *& crankback,
		    ACACPolicy::Direction failureDirection);

  /**
   * Set the vpi and vci for some call reference.
   *
   * @param cref the call reference
   *
   * @param vpvc the vpvc that corresponds to the call reference.
   */
  void SetVPVC(int cref, u_int vpvc);
  void SetVPVC(int cref, u_int vpi, u_int vci);

  /**
   * Get the vpi and vci for some call reference.
   * 
   * @param cref the call reference
   *
   * @param vpi the vpi that corresponds to the call reference or
   * zero if there is no such call reference
   *
   * @param vci the vci that corresponds to the call reference or
   * zero if there is no such call reference
   *
   * @return the vpvc; 0 if no such call reference
   */
  u_int GetVPVC(int cref);
  void GetVPVC(int cref, u_int &vpi, u_int &vci);

  /**
   * This is the counterpart to AllocateBandwidth().
   *
   * @param call_reference the cref of the call to release 
   * @see AllocateBandwidth
   */
  void FreeBandwidth(u_int call_reference);

  /**
   * GetDest returns the out-bound port number and call reference
   * that correspond to a call reference for a call on this port.
   * 
   * @param inCREF
   * @param outPort
   * @param outCREF
  */
  void GetDest(int inCREF, u_int &outPort, int &outCREF);

  /**
   * Return the NodeID of the called party for some call reference.
   *
   * @param CREF
   *
   * @return a pointer to a NodeID
   */
  NodeID *CalledParty(int CREF) const;

  /** Returns _theRAIGs.  THIS IS PROBABLY A BAD IDEA.
   *
   * @return the actual list of ig_resrc_avail_info that is kept
   * inside this object.  THIS IS PROBABLY A BAD IDEA.
   */
  dictionary<u_int, ig_resrc_avail_info *> &ShareAllRAIGs(void);

  /** Set the RAIGs.  The raig's flags name the service categories to
   * which it belongs.  pluggable_port_info makes a copy of raig.
   *
   * BUG: Insertion takes O(n^2) time because in order to prevent the
   * duplication of service categories, RAIG() must search for each
   * service category in raig's bitset of service categories.  It uses
   * the other RAIG() function, which takes O(n) per search.  Since
   * there may be n searches, this leads to O(n^2) time for this
   * RAIG().
   *
   * BUG: Permits the insertion of two RAIGs whose flags are identical
   * but for the GCAC_CLP bit.  ig_resrc_avail_info * RAIG() returns the
   * first of them that it finds but there is no telling which one that
   * will be.
   *
   * @param raig a raig for this port */
  void RAIG(ig_resrc_avail_info *raig);

  /**
   * What RAIG corresponds to this RAIG flag?
   *
   * BUG: Each search takes O(n) time where n is the number of
   * service categories.
   *
   * BUG: Ignores the GCAC_CLP bit.  If there are two RAIGs whose
   * flags differ only in the GCAC_CLP bit, the RAIG() returns the
   * first one that it finds.  There is no telling which one that will
   * be.
   *
   * @return a pointer to the RAIG if there is a RAIG for the service
   * category flag.
   *
   * @param flag is the bitset of service categories of interest.  */
  ig_resrc_avail_info *RAIG(u_int flag);
  
  /**
   * What ULIA does this uplink carry?
   */
  ig_uplink_info_attr *ULIA(void);

  /**
   * Set the ULIA for this uplink.
   * Makes a copy of aULIA.
   */
  void ULIA(const ig_uplink_info_attr *aULIA);

  /**
   * Add to sequence number.  increment can be negative.
   * The sequence number is used when originating a PTSE about this port.
   *
   * @param increment The amount to change the sequence number.
   */
  void NextSequenceNumber(int increment = 2);	

  /**
   * What is the current sequence number for this port?
   * The sequence number is used when originating a PTSE about this port.
   *
   * @return the sequence number.
   */
  int SequenceNumber(void);

  /**
   * Print the state of the port to a file.  If filename is zero or
   * its string length is zero, print the state on cout.
   *
   * @param filename the name of the file
   */
  void Show(const char *filename = 0);

  /** Release all of the calls that go through this port.  Returns a
   * list of FastUNIVisitors of type FastUNIRelease.  The caller must
   * deliver them.  Also returns a list of pairs of <port, call
   * reference> that represent the other half of each call through
   * this port.  The caller must release those calls individually.
   *
   * @param *releases a list of FastUNIVisitors of type FastUNIRelease.
   *
   * @param *call_ids a list of call identifier structures.  */
  void Release(list <FastUNIVisitor *> ** releases,
	       list <struct call_identifier *> ** call_ids);

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
  void Release(int cref, 
	       FastUNIVisitor ** release,
	       struct call_identifier ** call_id);


  /**
   * Permit the printing of any pluggable_port_info object on an ostream.
   *
   * @param os the ostream on which to print
   *
   * @param port the pluggable_port_info object to print
   *
   * @return the ostream
   */
  friend ostream & operator << (ostream & os,
				const pluggable_port_info & port);


protected:
  ReserveBandwidth(u_int inPort, u_int outPort, u_int inCREF, u_int outCREF,
		   u_int inVPI, u_int inVCI, u_int service_category,
		   const ie_bbc *bbc, const UNI40_traffic_desc *td, 
		   const NodeID *called_party,
		   ig_resrc_avail_info *theRAIG,
		   ACACPolicy::Direction direction);

  ACACPolicy * _sharedACACPolicy;	// Implements a call admission policy.

private:

#ifndef MAKE_VPVC
#define MAKE_VPVC(vpi, vci) (((vpi&0xff) << 16) | ((vci) & 0xffff))
//
// MAKE_VPI() and MAKE_VCI() assume 32-bit inputs.
//
#define MAKE_VPI(vpvc) (((vpvc) & 0x00ff0000) >> 16)
#define MAKE_VCI(vpvc) ((vpvc) & 0x0000ffff)
#endif

  /**
   * Call requirements.
   * Stored with key call reference in _Calls.
   *
   * NOTE that iport_oport_td thinks that it owns the BBC and TD.  It
   * will delete them in the destructor.  It will delete the BBC when
   * you change the BBC.  It will delete the TD when you delete the
   * TD.  You may not delete TD or BBC yourself.
   */
  class iport_oport_td {
  public:

    iport_oport_td(u_int iport = 0, u_int oport = 0, u_int scat = 0,
		   u_int input_cref = 0,
		   u_int output_cref = 0,
		   u_int vpi = 0, u_int vci = 0,
		   ie_bbc * bbc = 0, 
		   UNI40_traffic_desc * td = 0,
		   NodeID * called_party = 0);

    ~iport_oport_td();

    inline void InputPort(u_int iport) {_input_port = iport; };
    inline u_int InputPort(void) { return _input_port; };

    inline void InputCREF(int iCREF) {_input_cref = iCREF; };
    inline int InputCREF(void) { return _input_cref; };

    inline void OutputPort(u_int oport) {_output_port = oport; };
    inline u_int OutputPort(void) { return _output_port; };

    inline void OutputCREF(int oCREF) {_output_cref = oCREF; };
    inline int OutputCREF(void) { return _output_cref; };

    inline void VPVC(u_int vpvc) { 
      _vpi = MAKE_VPI(vpvc); 
      _vci = MAKE_VCI(vpvc);
    };

    inline void VPVC(u_int vpi, u_int vci) { _vpi = vpi; _vci = vci; };

    inline u_int VPVC(void) { return MAKE_VPVC(_vpi, _vci); };
    inline u_int VPI(void) { return _vpi; };
    inline u_int VCI(void) { return _vci; };

    inline void ServiceCategory(u_int scat) {_service_category = scat; };
    inline u_int ServiceCategory(void) { return _service_category; };

    void BBC(ie_bbc * bbc);

    inline ie_bbc *BBC(void) {
      return _bbc;
    };

    void TD(UNI40_traffic_desc *td);

    inline UNI40_traffic_desc * TD(void) {
      return _td;
    };

    void CalledParty(NodeID *called_party);

    inline NodeID *CalledParty(void) const {
      return _called_party;
    };

    /// The call comes in on _input_port
    u_int _input_port;

    /// The call leaves on _output_port
    u_int _output_port;

    /// The call is in one of the PNNI service categories defined by 
    /// the RAIG_FLAGS constants
    u_int _service_category;

    /// The call reference of the "in-bound" side of the call.
    int _input_cref;

    /// The call reference of the "out-bound" side of the call.
    int _output_cref;

    /// The vpi/vci of the "in-bound" side of the call.
    u_int _vpi;
    u_int _vci;

    /// Broadband Bearer Capability
    ie_bbc *_bbc;

    /// Traffic Descriptor
    UNI40_traffic_desc *_td;

    /// The called party
    NodeID *_called_party;

  };

  /*
   * The class knows the range of call reference values.
   * These are defined by the UNI 3.1 spec, pages 185-187.
   */
  static int _min_call_reference;
  static int _max_call_reference;

  /*
   * Each port has its own sequence of call reference values
   * ranging from _min_call_reference to _max_call_reference.
   */
  u_int _call_reference;

  bool _dirty; // true when the state of the pluggable_port_info has
		// changed.


  /* For the port to be useful, its Node Peer FSM must be FullState.
   * We can't originate PTSE's until it is, even if the port is up.
   *
   * The value of _npstate is either FullState or NonFullState.
   */
  NPStateVisitor::NPStateVisitorType _npstate;

  /* Constructor argument.  _up indicates the physical state of the port.
   * When _up is true, the port's physical state is up.  That is, it's 
   * connected to something and the HELLO Finite State Machines on the port
   * and the remote something are talking.
   *
   * When _up is false, the port is down.
   */
  bool _up;

  /* Constructor argument.  _type indicates the job of the port.  The port
   * can be used as an UPLINK or an HLINK.  If the job is not known, then
   * _type is unspecified.
   */
  enum port_type _type;

  /* Constructor argument.  When we make a PTSE that describes the
   * state of this port, we need a unique identifier.  _id is it.
   * There may or may not be a correspondence between _id and the
   * number of the port.
   */
  int _id;

  /* _localNode is the address of the switch or host in which this
   * port resides.
   */
  NodeID *_localNode;

  /* Constructor argument.  This is the number of the port.
   */
  u_int _localPort;

  /* When we make a PTSE that describes the state of this port, we
   * need a sequence number at least one greater than the last one we
   * made for this port.  _sequenceNumber is where we keep that number.
   * Usually, we increment _sequenceNumber by two.  This avoids
   * confusing the database.  The database ages the old ptse,
   * incrementing its sequence number by one in the process.  By
   * incrementing by two, we avoid the untenable position of the new
   * PTSE's sequence number conflicting with that of the PTSE on death
   * row. 
   */
  int _sequenceNumber;

  /* Constructor argument.  _remotePort is the number of the port at
   * the other end of the fiber.
   */
  u_int _remotePort;

  /* _theRAIGs is a table of ig_resrc_avail_info.  There is one such RAIG
   * for each of the PNNI service classes represented by the RAIG_FLAG
   * constants except the RAIG_FLAG_GCAC_CLP.  That flag may be used in
   * conjunction with the other RAIG_FLAGs.
   */
  dictionary<u_int, ig_resrc_avail_info *> _theRAIGs;

  ig_uplink_info_attr *_theULIA;

  /* Constructor argument.  _remoteNode is the address of the switch
   * or host at the other end of the fiber from this port. 
   */
  NodeID *_remoteNode;

  /* _remoteAddress is the ATM address of the switch or host at the other
   * end of the fiber from this port.
   */
  Addr *_remoteAddress;


  /* Constructor argument.  _commonPeerGroup is the ID of the peer
   * group to which the local node and the remote node belong.
   */
  PeerID *_commonPeerGroup;

  /* Constructor argument.  "A number assigned to an outside link by
   * the border nodes at the ends of the outside link.  The same number is
   * associated with all uplinks and induced uplinks associated with the
   * outside link.  In the parent and all higher-level peer group, all
   * uplinks with the same aggregation token are aggregated."  
   *
   * From af-pnni-0055.000, page 5.
   */
  int _aggregationToken;

  /**
   * _Calls maps a call reference into a struct that names the ports
   * and traffic descriptor for the call.  This information is stored
   * in _Calls so that we can reclaim any call's resources when the
   * call is released.
   */
  sortseq <u_int, iport_oport_td *> _Calls;
};

#endif
