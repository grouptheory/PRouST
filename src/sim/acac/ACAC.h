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
 * @file ACAC.cc
 * @author talmage
 * @version $Id: ACAC.h,v 1.74 1999/02/19 21:23:02 marsh Exp $
 *
 * Purpose: ACAC is the Actual Call Admission Control (ACAC)
 * for a switch.  It decides whether to admit a call or not.  It
 * allocates the resources to support each call.  It originates PTSEs
 * that describe the state of the switch.
 *
 * Logos
 *   A
 *   B
 * RouteControl
 *   A
 *   B
 * ACAC
 *   A
 *   B
 * Rest of Control
 *   A
 *
 * ACAC processes the following kinds of Visitors:
 * BorderDownVisitor, BorderUpVisitor, DBIntroVisitor, FastUNIVisitor,
 * NPStateVisitor, PortDownVisitor, PortUpVisitor, SaveVisitor,
 * ShowVisitor, StreamSaveVisitor.  It passes other Visitors to the
 * next Conduit.
 *
 * A-side processing
 *
 * ACAC expects FastUNIVisitors of the following types to
 * arrive on its A side: FastUNIRelease, FastUNISetup,
 * FastUNISetupFailure.  It passes other
 * FastUNIVisitors to RouteControl
 *
 * FastUNIRelease:
 *
 * Indicates that a call is at the end of its useful life.  It
 * contains the input port and the call reference.
 *
 * ACAC gives the FastUNIVisitor to ACAC::Release().
 * Release() frees the outgoing bandwidth for the call on the input
 * port.  It finds the appropriate output port and frees the incoming
 * bandwidth on it.  If the output port is zero, Release() Suicide()s
 * the FastUNIVisitor because the call obviously originated in the
 * switch and has nowhere else to go.  Release() returns fals in this
 * case.  Otherwise, Release() sets the output port of the
 * FastUNIVisitor to the input port, sets the input port of the
 * FastUNIVisitor to zero, and returns true.  The setting of input
 * port and outport port is an aide to Forwarder.
 *
 * After that, if Release() returned true, ACAC either
 * Suicide()s the FastUNIVisitor if its destination NodeID is equal to
 * the physical NodeID of the switch or passes the FastUNIVisitor to
 * the next Conduit it it is not.
 *
 * @see ACAC::Release()
 *
 * FastUNISetup is passed to RouteControl for DTL processing.
 *
 * FastUNISetupFailure is Suicide()-ed.  If any should arrive on the A
 * side, then there is something wrong.
 *
 * B-side processing
 *
 * ACAC expects FastUNIVisitors of the following types to
 * arrive on its B side: FastUNIRelease, FastUNISetup,
 * FastUNISetupFailure.  It passes other
 * FastUNIVisitors to the next Conduit.
 * 
 * Indicates that a call is at the end of its useful life.  It
 * contains the input port and the call reference.
 *
 * ACAC gives the FastUNIVisitor to ACAC::Release().
 * Release() frees the outgoing bandwidth for the call on the input
 * port.  It finds the appropriate output port and frees the incoming
 * bandwidth on it.  If the output port is zero, Release() Suicide()s
 * the FastUNIVisitor because the call obviously originated in the
 * switch and has nowhere else to go.  Release() returns fals in this
 * case.  Otherwise, Release() sets the output port of the
 * FastUNIVisitor to the input port, sets the input port of the
 * FastUNIVisitor to zero, and returns true.  The setting of input
 * port and outport port is an aide to Forwarder.
 *
 * After that, if Release() returned true, ACAC either
 * Suicide()s the FastUNIVisitor if its destination NodeID is equal to
 * the physical NodeID of the switch or passes the FastUNIVisitor to
 * the next Conduit it it is not.
 *
 * @see ACAC::Release()
 *
 * FastUNISetup:
 *
 * Indicates which host to call.  Contains called party number, input
 * port, input call reference, input vpi, input vci, and the
 * designated transit list (DTL), traffic descriptor and broadbadn
 * bearer capability information elements.
 *
 * ACAC passes the FastUNIVisitor to
 * ACAC::SetupFromGCAC().  SetupFromGCAC() passes the
 * FastUNIVisitor to ACAC::CallAdmissionControl().  
 *
 * If CallAdmissionControl() can find an output port that goes to the
 * next hop on the DTL and has enough bandwidth to support the call,
 * it reserves that bandwidth, sets the output vpi to -1 and the
 * output vci to -1, sets the output port to the port that connects to
 * the next node on the DTL, sets the input port to zero, sets the
 * call reference to the new one allocated for the segment of the call
 * to the next node, and returns true.
 *
 * If CallAdmissionControl cannot find a port with enough available
 * bandwidth to support the call, it sets the cause and crankback
 * information elements appropriately and returns false.
 *
 * If CallAdmissionControl() returned true, SetupFromGCAC() delivers
 * the CREFEvent from the FastUNIVisitor to the RouteControl
 * submodule. chooses the input vpi and vci if the switch is the
 * originator of the call.  It passes the FastUNIVisitor to the
 * Conduit on its A-side.
 *
 * If CallAdmissionControl() returned false, SetupFromGCAC() sets the
 * output port to the input port and sets the input port to zero.
 *
 * @see ACAC::SetupFromGCAC()
 *
 * @see ACAC::CallAdmissionControl()
 *
 * FastUNISetupFailure is passed to the rest of Control.
 *
 * Either side processing
 *
 * BorderDownVisitor:
 *
 * Indicates that a border is "down".  It contains the local port number.
 *
 * If the BordertDownVisitor's source NodeID is the same as the physical
 * NodeID of the switch, CACRouingFSM passes the local port number to
 * BorderDown() and Suicide()s the BorderDownVisitor.  Otherwise, the
 * source NodeID is a logical address, so ACAC passes the
 * BorderDownVisitor along.
 *
 * @see ACAC::BorderDown()
 * 
 * BorderUpVisitor:
 *
 * Indicates that a border is "up".  It contains the local and remote
 * port numbers, the NodeID of the remote switch at the remote port,
 * the aggregation token, and the uplink information attributes
 * information group (ULIA) for the remote node.  The ULIA contains
 * the resource availailability information groups (RAIGs).
 *
 * If the BorderUpVisitor's destination NodeID is the same as the
 * physical NodeID of the switch, ACAC passes the contents to
 * BorderUp(), deletes the RAIGs, and Suicide()s the BorderUpVisitor.
 * Otherwise, the destination NodeID is a logical address, so
 * ACAC passes the BorderUpVisitor along.
 *
 * @see ACAC::BorderUp()
 *
 * DBIntroVisitor:
 *
 * NPStateVisitor:
 *
 * Indicates the state of the NodePeer finite state machine for some
 * port in the switch.  The NPStateVisitor contains the NodeID of a
 * remote node.
 *
 * If the NPStateVisitor is of type NPStateVisitor::FullState,
 * ACAC passes the NPStateVisitor's destination NodeID to
 * NodeFull().  Otherwise, it passes the destination NodeID to
 * NodeNonFull().  In either case, ACAC passes the
 * NPStateVisitor along.
 *
 * @see ACAC::NodeFull()
 *
 # @see ACAC::NodeNonFull()
 *
 * PortDownVisitor:
 *
 * Indicates that a port is "down".  That means that the Hello on the
 * port has fallen out of agreement with its peer on a port of a
 * remote switch.  It contains the local port number.
 *
 * If the PortDownVisitor's source NodeID is the same as the physical
 * NodeID of the switch, CACRouingFSM passes the local port number to
 * PortDown() and Suicide()s the PortDownVisitor.  Otherwise, the
 * source NodeID is a logical address, so ACAC passes the
 * PortDownVisitor along.
 *
 * @see ACAC::PortDown()
 *
 * PortUpVisitor: 
 *
 * Indicates that a port is "up".  That means that the Hello on the
 * port is in agreement with its peer on a port of a remote switch.
 * It contains the local and remote port numbers, the NodeID of the
 * remote switch at the remote port, the aggregation token, and the
 * resource availability information groups (RAIGs) for the remote
 * node.
 *
 * If the PortUpVisitor's destination NodeID is the same as the
 * physical NodeID of the switch, ACAC passes the contents to
 * PortUp(), deletes the RAIGs and Suicide()s the PortUpVisitor.
 * Otherwise, the destination NodeID is a logical address, so
 * ACAC passes the PortUpVisitor along.
 *
 * @see ACAC::PortUp().  Especially, note the BUG!
 *
 * SaveVisitor:
 *
 * Indicates the base name of the file to write a snapshot of the
 * state of ACAC.
 *
 * ACAC extracts the string from the SaveVisitor and appends
 * OwnerName(), the name of its conduit.  It writes its state into a
 * file of that name, overwriting an existing file of the same name if
 * necessary.  After saving its state in the file, ACAC
 * passes the SaveVisitor along to the next Conduit.
 *
 * @see ACAC::Show().
 *
 * ShowVisitor: See the actions for SaveVisitor.
 *
 * StreamSaveVisitor:
 *
 * Indicates the ostream to which ACAC must save its state.
 *
 * ACAC writes its state to the ostream and passes the
 * StreamSaveVisitor along to the next Conduit.  
 *
 * @see ACAC::StreamSave().
 *  */
#ifndef __ACAC_H__
#define __ACAC_H__

#ifndef LINT
static char const _ACAC_h_rcsid_[] =
"$Id: ACAC.h,v 1.74 1999/02/19 21:23:02 marsh Exp $";
#endif

#include <FW/actors/State.h>
#include <codec/pnni_ig/id.h>
#include <fsm/election/ElectionVisitor.h>

#include <DS/containers/dictionary.h>
#include <DS/containers/list.h>

#include <sim/acac/ACACPolicy.h>

class ACAC;
class ACACPolicy;
class BorderUpVisitor;
class Database;
class FastUNIVisitor;
class ForwarderDefaultInterface;
class NodeID;
class PeerID;
class SimEvent;
class ds_String;
class VCAllocator;
class ig_ptse;
class ig_resrc_avail_info;
class ig_uplink_info_attr;
class pluggable_port_info;

/** Code of the wakeup interruption.  ACAC wakes up every
 * Database::PTSERefreshInterval() seconds to reoriginate PTSEs that
 * describe the state of the switch.  If there is no database, then
 * ACAC wakes up every ACAC_WAKEUP_INTERVAL seconds.  */
#define ACAC_WAKEUP_EVENT 1010

/** Code of the reorigination interruption.  Whenever ACAC notices
 * that a port is "dirty" (i.e. that its resources have "changed
 * significantly"), it schedules a reorigination event for immediate
 * delivery.
 */
#define ACAC_REORIGINATION_EVENT 1020

#if 0
/** Seconds between wakeup interruptions Use this constant if there is
 * no database.  If there is a database, use
 * DBACACInterface::PTSERefreshInterval().  */
#define ACAC_WAKEUP_INTERVAL 55.0
#endif

#if 0
/** Number of seconds that a PTSE lives in the database.  After this
 * many seconds, the database expires the PTSE.  Use this constant if
 * there is no database.  If there is a database, use
 * DBACACInterface::PTSELifetime().  */
#define ACAC_PTSE_LIFETIME 120.0
#endif

/** ACAC is a State that manages the data resources of a switch.
 *
 * When a port comes up or down, it marks the port UP or DOWN, respectively.
 * When a port's NodePeer FSM enters FullState, it marks the port Full.
 * When a port's NodePeer FSM leaves FullState, it marks the port NonFull.
 * When a port is in FullState, ACAC originates a PTSE that describes
 * the port, either an HLINK or an UPLINK.  Periodically, it reoriginates
 * PTSEs for each port that is still in FullState.
 *
 * When a call setup arrives, ACAC finds a route to the destination.
 * If there is a route, ACAC tries to allocate the resources 
 * needed to support the call.  If it can do that, it accepts the call.
 * Otherwise, it rejects the call.  */
class ACAC : public State {
  friend class Control;
public:


  /** Constructor
   *
   * @param myNode The address of the switch. ACAC copies it.
   *
   * @param numPorts The number of physical ports in the switch.
   *
   * @param refreshInterval How often to update the database.
   *
   * @param pluginPrefix 
   *
   * The constructor extracts from theConfigurator() the name of the
   * library that contains a call admission policy.  ACAC expects to
   * find a function named extern "C" ACACPolicy *
   * <plugin_prefix>ACACPolicyConstruct(void) that returns a pointer
   * to an object of type <plugin_prefix>ACACPolicy.  The constructor
   * gives the pointer to each of the pluggable_port_info objects it
   * allocates (i.e. one per physical port). 
   */
  ACAC(NodeID *myNode, int numPorts, double refresh_interval,
       ds_String *plugin_prefix);

  /** Destructor 
   */
  virtual ~ACAC(void);

  /** Deal with Visitors
   */
  State * Handle(Visitor* v);

  /** Deal with interruptions.  The only one ACAC recognizes
   * is ACAC_WAKEUP_EVENT
   */
  void Interrupt(SimEvent* e);

  /** Print the state of ACAC to a file.  If filename is zero
   * or if its string length is zero, print to cout.  Otherwise, print
   * to the filename.
   *
   * @param filename the name of the file
   */
  void Show(const char *filename);

  /** Write the state of ACAC to a stream.  It's in a special format
   * parsable by the route optimality test.  */
  void StreamSave(ostream & os);

  /** Permit the printing of any ACAC object on an ostream.
   *
   * @param os the ostream on which to print
   *
   * @param cac the ACAC object to print
   *
   * @return the ostream */
  friend ostream & operator << (ostream & os, const ACAC & cac);

protected:
  
  int  GetNewVCI(void);
  void ReturnVCI(int vci);

  //
  // Use the DataForwarder's call reference allocator for 
  // calls on port zero.  This prevents cref collisions.
  //
  u_int GetPortZeroCREF(void);
  void ReturnPortZeroCREF(u_int cref);

  /** List of ints.  Stored with key NodeID as a list of output ports
   * in _Nodes.  */
  typedef list<int> int_list;

  /** Call this when a port comes up (HLINK)
   *
   * BUG: Permits the insertion of two RAIGs whose service category
   * flags are identical but for the GCAC_CLP bit.  Any setup that
   * requires one of those service categories will use the first RAIG
   * that ACAC finds but there is no telling which one that will be.
   *
   * @param local_port The port that just came up on this switch
   *
   * @param remote_port The port on the other end of the fiber, on the
   * remote switch or host
   *
   * @param remote_node The remote host or switch.
   *
   * @param aggregation_token "A number assigned to an outside link by
   * the border nodes at the ends of the outside link.  The same number is
   * associated with all uplinks and induced uplinks associated with the
   * outside link.  In the parent and all higher-level peer group, all
   * uplinks with the same aggregation token are aggregated."  
   * From af-pnni-0055.000, page 5.
   *
   * @param theRAIGs a list of resrc_avail_info *, up to one per service class.  */
  void PortUp(u_int local_port, u_int remote_port, NodeID *remote_node, 
	      int aggregation_token, list<ig_resrc_avail_info *> *theRAIGS);

  /** Call this when a port comes down
   *
   * @param local_port The port that went down */
  void PortDown(u_int local_port);
  
  /** Call this when a border node comes up (UPLINK).  It inserts an
   * UPLINK IG into the database.
   *
   * @param buv a pointer to the BorderUpVisitor that describes the
   * remote node.  It must not be zero.
   */
  void BorderUp(BorderUpVisitor *buv);

  /** Call this when a border node comes down.  It exprires an UPLINK
   * IG in the database.
   *
   * @param local_port The port that went down
   * */
  void BorderDown(u_int local_port);

  /** Call this when the NodePeer FSM for local_port enters FullState.
   * It inserts UPLINK IGs and HLINK IGs into the database.
   *
   * @param local_port The port that is now in FullState */
  void PortFull(u_int local_port);

  /** Call this when the NodePeer FSM for local_port leaves FullState.
   * It expires UPLINK IGs and HLINK IGs in the database.
   *
   * @param local_port The port that went out of FullState
   * */
  void PortNonFull(u_int local_port);

  /** Call this when the NodePeer FSM for a remote node enters
   * FullState.  It calls PortFull() for each port connected to the
   * remote node.
   *
   * @param remote_node The node that entered FullState */
  void NodeFull(const NodeID *remote_node);

  /** Call this when the NodePeer FSM for a remote node enters a state
   * that is not FullState.  It calls PortNonFull() for each port
   * connected to the remote node.
   *
   * @param remote_node The node that left FullState */
  void NodeNonFull(const NodeID *remote_node);

  /** Response to FastUNIVisitors that enter our A-side
   *
   * @param fuv The FastUNIVisitor contains a UNI Release
   *
   * @return true if it is necessary to forward fuv to the next
   * Conduit.  Suicide()s fuv and returns false otherwise.  It is
   * necessary to Suicide() and not forward when we are then node that
   * originated the call.  */
  void Release(FastUNIVisitor *fuv);

  /** Release a call given the port and call reference.
   *
   * @param inport the port on which the call enters or exits the
   * switch.
   *
   * @param cref the call reference at inport.
   */
  void Release(int inport, int cref);

  /** Release all calls on a port.
   *
   * @param inport the port on which the call enters or exits the
   * switch.
   *
   */
  void Release(int inport);

  /** Response to FastUNIVisitors that enter our A-side
   *
   * @param fuv The FastUNIVisitor contains a UNI Release */
  void SetupFailure(FastUNIVisitor *fuv);

  /** Responses to FastUNIVisitors that enter our B-side
   *
   * @param fuv The FastUNIVisitor contains a UNI Setup */
  void SetupFromGCAC(FastUNIVisitor *fuv);

  /** Determine if we can support a new call from here to the next
   * NodeID, one of our neighbors.  If we can, reserve resources for
   * the call and return true.  Otherwise, return false.
   *
   * @param fuv The FastUNIVisitor contains a UNI Setup */
  ACACPolicy::CallStates CallAdmissionControl(FastUNIVisitor *fuv);

  /**
   * Send new PTSEs to replace the old ones in the database.
   * Send PTSEs that contain HLINK and UPLINK for the physical connections.
   */
  void Reoriginate(void);

  /** Is a FastUNISetup for me or not?
   *
   * @param fuv the FastUNIVisitor, presumably of type FastUNISetup.
   *
   * @return true if the NSAP of the DestNID matches _myAddr or
   * if that NSAP is zero but the NSAP of the called party number
   * matches _myAddr.  False otherwise.
   */
  bool ItsForMe(FastUNIVisitor *fuv);

  /** Is a FastUNISetup from me or not?
   *
   * @param fuv the FastUNIVisitor, presumably of type FastUNISetup.
   *
   * @return true if the NSAP of the SourceNID matches _myAddr or
   * if that NSAP is zero but the NSAP of the called party number
   * matches _myAddr.  False otherwise.
   */
  bool ItsFromMe(FastUNIVisitor *fuv);

  /// Number of ports in the switch
  int               _numPorts;

  /// For constructing PTSEs
  int               _myID;

  /// Address of this switch
  NodeID          * _myNode;

  /// ATM address of this switch
  Addr *_myAddr;

  /// Address of the switch's peer group
  PeerID          * _myPeerGroup;

  /// true if this switch is the peer group leader
  u_int             _iAmTheLeader;

  /// A pointer to the database that contains the PTSE's that
  /// this ACAC originates and reoriginates.
  Database        * _db;

  /// A pointer to an instance of a class that implements
  /// a call admission policy.
  ACACPolicy *_sharedACACPolicy;

  /** _Nodes maps a remote NodeID into a list of ports that are
   * connected to it.  We use this information to find an output port
   * for a new call.  */
  dictionary<const NodeID *, int_list *> _Nodes;

  /** _ports is an array of pointers to pluggable_port_info objects.
   * There is no entry in _ports[0].  Each of the others (1 through
   * _numPorts - 1) is non-null.  Each of the others represents the
   * state of one data port.  */
  pluggable_port_info **_ports;

  /** _Ignore is a dictionary that maps pointers to FastUNIVisitor to
   * bools.  It lets us avoid processing a FastUNIRelease twice.  We
   * could process it once when it arrives on our A-side.  We could
   * process it again if it arrives on our B-side.  That could happen
   * if RouteControl decides not to perform crankback.  
   *
   * If there is no entry for a FastUNIVisitor in _Ignore, then ACAC
   * knows that it has never seen that particular Visitor.  If there
   * is an entry but the information there is 'false', then ACAC knows
   * that it has seen and processed the Visitor before and that it did
   * not originate the call to which the Visitor pertains.  If the
   * information is 'true', ACAC knows that it has seen and processed
   * the Visitor before and that it originated the call to which the
   * Visitor pertains.*/

  dictionary <FastUNIVisitor *, bool> _Ignore;

  /** _SVCCs is the set of the remote NodeIDs to to which there is a
   * Signalling Virtual Control Channel.  We use it to reduce the
   * number of SETUPs for SVCCs to one per remote NodeID.  
   */
  list <const NodeID *> _SVCCs;

private:

  static const VisitorType * _border_down_type;
  static const VisitorType * _border_up_type;
  static const VisitorType * _db_intro_type;
  static const VisitorType * _fast_uni_type;
  static const VisitorType * _npstate_type;
  static const VisitorType * _port_up_type;
  static const VisitorType * _port_down_type;
  static const VisitorType * _save_type;
  static const VisitorType * _show_type;
  static const VisitorType * _stream_save_type;

  SimEvent * _wakeup;	// Makes us reoriginate PTSEs.

  //
  // Allocates call references for port zero
  //
  ForwarderDefaultInterface *_port_zero_cref_interface;
};

#endif
