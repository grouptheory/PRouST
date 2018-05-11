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
#ifndef __FORWARDER_H__
#define __FORWARDER_H__

#ifndef LINT
static char const _Forwarder_h_rcsid_[] =
"$Id: Forwarder.h,v 1.48 1999/02/19 22:42:17 marsh Exp $";
#endif

#include <FW/actors/Terminal.h>
#include <DS/containers/list.h>
#include <DS/containers/dictionary.h>
#include <DS/containers/sortseq.h>
#include <DS/containers/queue.h>

class Fab;
class VCAllocator;
class Visitor;
class FastUNIVisitor;
class NodeID;
class VPVCVisitor;
class PNNIVisitor;
class Addr;
class ForwarderDefaultInterface;

                         // D a t Q
#define DATA_QUEUE_EVENT 0x44617451
                         // C t l Q
#define CTRL_QUEUE_EVENT 0x43746c51

/**  Forwarder

     The Forwarder is responsible for keeping the bindings from in
     [port, vp, vc, cref, addr] to out [port, vp, vc, cref, addr] and
     directing Visitors out on the appriopriate paths. 

     It is also responsible for negotiating the VP/VC combo with its
     peer across a specific link when setting up calls.
 */
class Forwarder : public Terminal {
  friend class ForwarderDefaultInterface;
  friend int compare(FastUNIVisitor * const &, FastUNIVisitor * const &);
  friend int compare(SimEvent * const &, SimEvent * const &);
public:

  /// Constructor expects Database to be non-null
  Forwarder( const NodeID * nid );
  /// Destructor cleans out the mappings, deleting the bindings
  virtual ~Forwarder();

  // Absorb:
  //    If (v == FastUNIVisitor)
  //      If (inport != 0)
  //        HandleFUVFromPhysical( );
  //      else
  //        HandleFUVFromControl( );
  //    else if (VCI == 18 && VPI == 0)
  //      If (inport != 0)
  //        HandlePNNIFromPhysical( );
  //      else
  //        HandlePNNIFromControl( );
  //    else
  //      If (inport != 0)
  //        HandleDataFromPhysical( );
  //      else
  //        HandleDataFromControl( );
  void Absorb(Visitor * v);
  ///   Do nothing
  void Interrupt(SimEvent * e);

  /// Adds mappings from [iport, ivpi, ivci, icref, iaddr] <--> [oport, ovpi, ovci, ocref, oaddr]
  bool Bind(int iport, int ivpi, int ivci, int icref, const Addr * iaddr,
	    int oport, int ovpi, int ovci, int ocref, const Addr * oaddr);

  bool Bind(Fab * in, Fab * out);
  
  bool Rebind(Fab * in, int vpi, int vci);
  
  /// Returns a pointer to the VCAllocator for Port 0, Caller must call Reference on the VCAllocator
  VCAllocator * GetSharedPortZeroVCAllocator(void);

  /// Used in quasi-switches to declare a port in service
  void NotifyOfPort(int port);
  /// Prints out all the binding fabrics in the switch
  void PrintBindings(ostream & os);

private:

  void SendToControl(  Visitor * v );
  void SendToPhysical( Visitor * v, int port );

  /// Always sends the FUV to Control after processing
  void HandleFUVFromPhysical(FastUNIVisitor *& fv,
			     int iport, int ivpi, int ivci);
  /// Either injects the Visitor back into the Mux (req ACAC to set the ports properly) or
  ///  uses the stored bindings to forwarder the FUV back along its path
  void HandleFUVFromControl(FastUNIVisitor *& fv,
			    int iport, int ivpi, int ivci);
  /// Sends the Visitor out the proper port
  void HandlePNNIFromControl(VPVCVisitor *& vis,
			     int iport, int ivpi, int ivci);
  /// Sends the Visitor down to Hello
  void HandlePNNIFromPhysical(VPVCVisitor *& vis,
			      int iport, int ivpi, int ivci);
  /// calls Forward
  void HandleDataFromControl(VPVCVisitor *& vis,
			     int iport, int ivpi, int ivci);
  /// call Forward
  void HandleDataFromPhysical(VPVCVisitor *& vis,
			      int iport, int ivpi, int ivci);

  /// Looks up the bindings for this Visitor's port, ivci, ivpi and sends it out accordingly
  void Forward(VPVCVisitor *& vis);
  /// Forwards a FastUNIVisitor::Release based upon iport and icref
  void ForwardRelease(FastUNIVisitor *& release);

  /// Removes all bindings associated with the specified port and call reference
  bool UnBind(int port, unsigned int cref);
  /// Removes all bindings associated with the specified fabric
  bool UnBind(Fab * in);
  /// Removes all bindings associated with the specified fabrics
  bool UnBindBothDirections(Fab & in, Fab & out);
  /// Returns true if we are the originator of this call
  bool OriginatedCall(int port, unsigned int crv);

  void PrintInfo(ostream & os);

  // -----------------------------------
  u_int NextPort0CREF(void);
  void  ReturnPort0CREF(u_int cref);
  
  unsigned int _lowestCREF;
  list<u_int>  _freeCREF;
  list<u_int>  _usedCREF;
  // -----------------------------------

  /// Local Node ID
  NodeID                                       * _LocNodeID;
  /// Number of ports this switch contains.
  int                                            _nports;
  dictionary <Fab *, list<Fab *> *>              _FabFabMap;
  dictionary <Fab *, Addr *>                     _FabAddrMap;
  dictionary <int ,  dictionary<unsigned int, Fab *> *> _Port2_CREF2Fab_Map;
  dictionary <FastUNIVisitor *, Fab *>           _setup_in_progress_Map;
  /// sorted sequence of VCAllocators (sorted by port)
  sortseq<int, VCAllocator *>                    _vcallocs;

  /// Returns request if available for the specified port or 
  /// if request is -1 returns the next available VCI for that port.
  int  RequestNewVC(int port, int request = -1);
  /// Returns the specified VC to the VCAllocator for that port
  bool ReturnVC(int port, int vc);

  ds_queue< Visitor * > _dataQueue;
  ds_queue< Visitor * > _ctrlQueue;

  const double _DATA_DELAY;
  const double _CTRL_DELAY;

  /**@name The specific types of Visitors that the Forwarder is knowledgable of
   */
  //@{
  static const VisitorType * _fastunitype;
  static const VisitorType * _electiontype;
  static const VisitorType * _linkuptype;
  static const VisitorType * _portuptype;
  static const VisitorType * _portdowntype;
  static const VisitorType * _borderuptype;
  static const VisitorType * _borderdowntype;
  static const VisitorType * _vpvctype;
  static const VisitorType * _porttype;
  //@}

#ifndef OPTIMIZE
  void checkFabrics(void);
#endif
};

#endif
