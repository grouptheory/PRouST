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
#ifndef __COMPLEX_REP_H__
#define __COMPLEX_REP_H__
#ifndef LINT
static char const _ComplexRep_h_rcsid_[] =
"$Id: ComplexRep.h,v 1.3 1999/02/26 15:14:25 talmage Exp $";
#endif

#include <DS/containers/list.h>
#include <FW/kernel/Handlers.h>

typedef short int sh_int;
typedef unsigned short int u_short;

class ostream;
class NodeID;
class ig_resrc_avail_info;
class AggregationPolicy;
class Leadership;
class Logos;
class LogosGraph;
class Aggregator;

class ComplexRep : public TimerHandler {
public:
  // default periodicity between reaggregation requests
  static const double _default_update_interval = 90.0;  

  // quality of services classes
  enum service_class {    
    CBR    = 0, 
    RTVBR  = 1, 
    NRTVBR = 2, 
    ABR    = 3, 
    UBR    = 4, 
    ALL_SERVICE_CLASSES };

  // possible error codes
  enum errcode {
    success,
    lgport1_out_of_range,
    lgport2_out_of_range,
    link_not_present,
    ambiguous_service_class,
    not_supported
  };

  // literal queries ask about links in the complex node representation,
  // interpreted queries ask about paths in the complex node representation
  enum query_mode { literal, interpreted };

  //----------------------------------------------------------
  // remove all links in the complex node representation
  void Clear_Links(void);

  // add a link to the complex node representation,
  // logical port 0, represents the nucleus. link 0,0 is the radius.
  errcode Set_Link(int lgport1, int lgport2,
		   service_class sc, 
		   int weight, 
		   int mcr, int acr, int ctd, int cdv, 
		   sh_int clr0, sh_int clr01);

  // used in Set_Link to verify that each class is added only once
  bool RAIG_not_already_present( list<ig_resrc_avail_info *> * entry,
				 service_class min_sc,  service_class max_sc);
  
  // remove a link from the complex node representation,
  // logical port 0, represents the nucleus. link 0,0 is the radius.
  errcode Remove_Link(int lgport, int lgport2,
				 service_class sc);

  // query the attributes of a link in the complex node representation,
  // logical port 0, represents the nucleus. link 0,0 is the radius.
  errcode Query_Link(query_mode m,
		     int lgport1, int lgport2, service_class sc, 
		     int& weight, 
		     int& mcr, int& acr, int& ctd, int& cdv, 
		     sh_int& clr0, sh_int& clr01) const;

  // evaluate how well a transit in the complex node representation
  // captures the reality in a given LogosGraph
  errcode Evaluate(LogosGraph* g,
  		   int lgport, int lgport2,
  		   service_class sc, 
		   double& weight, 
  		   double& mcr, double& acr, double& ctd, double& cdv, 
  		   double& clr0, double& clr01) const;

  // verify that this complex node representation has 
  // enough information to be considered valid
  bool Validate(void) const;

  //----------------------------------------------------------
  // print aggregated topology and metric information 
  // encapsulated in this complex node representation object
  void Print_Topology(ostream& os) const;
  void Print_Metrics(ostream& os) const;

  //----------------------------------------------------------
  // how long this complex node representation is to be considered valid, 
  // after which the aggregator will be given a chance to update it again
  void Set_TTL(double seconds);
  double Get_TTL(void) const;

  //----------------------------------------------------------
  // the address of the peergroup leader and the logical group node
  NodeID* Get_PGL(void) const;
  NodeID* Get_LGN(void) const;

  //----------------------------------------------------------
  // the list of logical ports for this logical node
  list<int>* Get_Logical_Ports_List(void) const;

private:

friend class Leadership;  
  //-------------------------------------------------
  // Leadership is the class that makes and destroys ComplexRep objects
  ComplexRep(const NodeID * lgnode, const NodeID * pgl, 
	     Aggregator * agg, Leadership * leadership,
	     Logos * logos, int num_lgports = 0);
  virtual ~ComplexRep();

friend class AggregationPolicy;
  //-------------------------------------------------
  // Aggregator is the class that can change number 
  // of logical ports in ComplexRep objects
  bool Add_LogicalPort(int lgportnumber);
  bool Remove_LogicalPort(int lgportnumber);
  void ReAggregate_PortCountChanged(void);

  // ----- Utility methods ----------
  // internal routine to remove all links
  void __Clear_Links(bool called_from_dtor=false);
  // called when this ComplexRep is no longer valid, i.e. the TTL expires
  void Callback(void);
  // compute the RAIG flags based on service class
  u_short service_class_2_flags(ComplexRep::service_class sc) const;
  errcode Get_RAIG(int lgport1,int lgport2, 
		   list<ig_resrc_avail_info*>*& raiglist) const ;

  void Reoriginate_NSP(void);

  // ----- Data members ----------
  dictionary< int, 
              dictionary< int, 
                          list<ig_resrc_avail_info*>*
                        >*
            > *_links;

  NodeID*            _pgl;
  NodeID*            _lgnode;
  int                _level;
  int                _seqnum;
  double             _ttl;

  AggregationPolicy* _aggr;
  Leadership*        _leadership;
  Logos*             _logos;

};

void Print_ComplexRep_Error(ComplexRep::errcode e, ostream& os);

#endif
