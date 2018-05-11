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
#ifndef __COMPLEX_REP_CC__
#define __COMPLEX_REP_CC__
#ifndef LINT
static char const _ComplexRep_cc_rcsid_[] =
"$Id: ComplexRep.cc,v 1.2 1999/02/19 22:55:40 marsh Exp $";
#endif

#include <ostream.h>
#include <FW/basics/diag.h>

#include "ComplexRep.h"
#include "Aggregator.h"
#include "AggregationPolicy.h"

#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <codec/pnni_ig/nodal_state_params.h>
#include <codec/pnni_ig/next_hi_level_binding_info.h>
#include <codec/pnni_ig/nodal_hierarchy_list.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_pkt/ptsp.h>
#include <codec/uni_ie/addr.h>

#include <fsm/netstats/NetStatsCollector.h>

#include <sim/leadership/Leadership.h>
#include <sim/logos/Logos.h>
#include <sim/logos/BaseLogos.h>
#include <sim/logos/LogosGraph.h>

//--------------------------------------------------------------------
ComplexRep::ComplexRep(const NodeID * lgnode, const NodeID * pgl, 
		       Aggregator * agg, 
		       Leadership * leadership,
		       Logos * logos,
		       int num_lgports) 
  : TimerHandler(leadership, _default_update_interval),
  _aggr( agg->_aggr ), _leadership( leadership ), _logos( logos ),
  _lgnode( 0 ), _pgl( 0 ), _seqnum( 5 ), // Is this alright?  Starting at 5?
  _level( -1 ), _links( 0 )
{
  assert( _aggr != 0 &&
	  _leadership != 0 &&
	  _logos != 0 );

  Set_TTL(_default_update_interval);

  assert (num_lgports >= 0);

  if (lgnode) {
    _lgnode = lgnode->copy();  // The Logical Group Node
    _level  = lgnode->GetLevel();  // Our level in hierarchy
  }
  if (pgl)
    _pgl    = pgl->copy();     // and the Peergroup Leader Address

  // The links in the complex node representation
  _links = new dictionary< int , 
                           dictionary< int , 
                                       list<ig_resrc_avail_info*>* >* >;

  // clear the representation; no links
  for (int i = 0; i <= num_lgports; i++) {
    dictionary< int,list<ig_resrc_avail_info*>* > * row 
      = new dictionary< int , list<ig_resrc_avail_info*>* >;

    _links->insert(i, row);
    for (int j = 0; j <= num_lgports; j++) {
      row->insert(j, new list<ig_resrc_avail_info*>);
    }
  }

  _aggr->Register_ComplexRep(this);
}

//--------------------------------------------------------------------
ComplexRep::~ComplexRep() 
{
  _aggr->Deregister_ComplexRep(this);

  delete _lgnode;            _lgnode = 0;
  delete _pgl;               _pgl    = 0;
  __Clear_Links( true );
}
  
//--------------------------------------------------------------------
void ComplexRep::Clear_Links(void) 
{
  __Clear_Links( false );
}

//--------------------------------------------------------------------
void ComplexRep::__Clear_Links(bool called_from_dtor) 
{
  dic_item li_row, li_col;
  forall_items( li_row , *_links )  // reset all raig lists to empty
    {  
      dictionary< int,list<ig_resrc_avail_info*>* > * row
	= _links->inf(li_row);
      
      forall_items( li_col , *row ) 
	{
	  list<ig_resrc_avail_info*>* list_raigs = row->inf(li_col);
	  while (list_raigs->size() > 0) {
	    ig_resrc_avail_info* raig = list_raigs->inf( list_raigs->first() );
	    list_raigs->del_item( list_raigs->first() );
	    delete raig;
	    raig = 0;
	  };
	  list_raigs->clear();
	}
      
      if (called_from_dtor) {         // free everything, if in dtor
	while (row->size() > 0) {
	  list<ig_resrc_avail_info*>* list_raigs = row->inf( row->first() );
	  row->del_item( row->first() );
	  delete list_raigs;
	  list_raigs = 0;
	};
	row->clear();
      }
    }
  
  if (called_from_dtor) {           // free everything, if in dtor
    while (_links->size() > 0) {
      dictionary< int , list<ig_resrc_avail_info*>* >* row 
	= _links->inf( _links->first() );
      _links->del_item( _links->first() );
      delete row; 
      row = 0;
    };
    _links->clear();

    delete _links; 
    _links = 0;
  }
}

//--------------------------------------------------------------------
ComplexRep::errcode 
ComplexRep::Set_Link(int lgport1, int lgport2,
		     service_class sc, 
		     int weight, int mcr, int acr, int ctd, int cdv, 
		     sh_int clr0, sh_int clr01) 
{
  service_class min_sc, max_sc, sc_val;

  if ( sc == ComplexRep::ALL_SERVICE_CLASSES ) {
    min_sc = (ComplexRep::service_class) 0;
    max_sc = (ComplexRep::service_class) 
      ((int)(ComplexRep::ALL_SERVICE_CLASSES)-1);
  } else {
    min_sc = max_sc = sc;
  }

  list<ig_resrc_avail_info *> * entry = 0;
  ComplexRep::errcode err = Get_RAIG( lgport1, lgport2, entry );
  assert( entry != 0 && RAIG_not_already_present(entry, min_sc, max_sc) );

  for ( sc_val = min_sc; 
	sc_val <= max_sc; 
	sc_val = (ComplexRep::service_class)((int)sc_val+1) ) {

    // remove old logical link raigs
    err = Remove_Link(lgport1, lgport2, sc_val);
    if ((err != ComplexRep::success) &&
	(err != ComplexRep::link_not_present)) {
      return err;
    }

    // create new logical link raig
    u_short req_flags = service_class_2_flags(sc_val);
    ig_resrc_avail_info* new_raig = new 
      ig_resrc_avail_info(ig_resrc_avail_info::outgoing,
			  req_flags,
			  weight,mcr,acr,ctd,cdv,clr0,clr01);

    // insert new logical link raig
    entry->append (new_raig);
  }
  return ComplexRep::success;
}

//--------------------------------------------------------------------
bool ComplexRep::RAIG_not_already_present (list<ig_resrc_avail_info *> * entry,
					   service_class min_sc,
					   service_class max_sc)
{
  bool answer = true;
  service_class sc_val;
  
  assert(entry);
  if (!entry->empty()) {
    u_int flags = 0;
    // construct flag with all the affected bits
    for ( sc_val = min_sc; 
	  sc_val <= max_sc; 
	  sc_val = (ComplexRep::service_class)((int)sc_val+1) ) 
      flags |= service_class_2_flags(sc_val);
    // check each entry in the list looking for a bit match
    list_item it;
    forall_items (it, *entry) {
      ig_resrc_avail_info *raig = entry->inf(it);
      u_int raig_flags = raig->GetFlags();

      // do a bitwise and of the two flags -- the result should be 0
      answer &= ( (raig_flags & flags) == 0);
    }
  }
  return answer;
}
 
      
//--------------------------------------------------------------------
ComplexRep::errcode 
ComplexRep::Query_Link(query_mode m,
		       int lgport1, int lgport2, 
		       service_class sc, 
		       int& weight, int& mcr, int& acr, int& ctd, int& cdv, 
		       sh_int& clr0, sh_int& clr01) const {

  // presently only explicit queries are supported!
  if ( m == ComplexRep::interpreted ) {
    return (ComplexRep::not_supported);
  }

  if (sc == ComplexRep::ALL_SERVICE_CLASSES) {
    return ComplexRep::ambiguous_service_class;
  }
    
  u_short req_flags = service_class_2_flags(sc);

  list<ig_resrc_avail_info*>* existing_raigs = 0;
  ComplexRep::errcode err = 
    Get_RAIG(lgport1,lgport2, existing_raigs);

  if (err != ComplexRep::success ) {
    return err;
  }
  else {
    assert(existing_raigs);
    list_item li;

    // search for the link
    int found = 0;
    ig_resrc_avail_info* golden;
    forall_items( li, *existing_raigs ) {
      ig_resrc_avail_info* raig_iter = existing_raigs->inf(li);
      if (raig_iter->GetFlags() == req_flags) {
	found++;
	golden = raig_iter;
      }
    }

    if (!found) {
      return ComplexRep::link_not_present;
    }
    else {
      assert (found==1);
      // obtain raig values for link of interest
      weight = golden->GetAdminWeight();
      mcr    = golden->GetMCR();
      acr    = golden->GetACR();
      ctd    = golden->GetCTD();
      cdv    = golden->GetCDV();
      clr0   = golden->GetCLR0();
      clr01  = golden->GetCLR01();
      return ComplexRep::success;
    }
  }
}


//--------------------------------------------------------------------
ComplexRep::errcode 
ComplexRep::Remove_Link(int lgport1, int lgport2,
			service_class sc) {

  list<ig_resrc_avail_info*>* existing_raigs = 0;
  ComplexRep::errcode err = 
    Get_RAIG(lgport1,lgport2, existing_raigs);

  if (err != ComplexRep::success) {
    return err;
  }
  else {
    list_item li;
    list<list_item*> kill_list;
    u_short req_flags = service_class_2_flags(sc);

    // search for link of interest,
    // make a list raigs to delete
    int found = 0;
    forall_items( li, *existing_raigs ) {
      ig_resrc_avail_info* raig_iter = existing_raigs->inf(li);
      if (raig_iter->GetFlags() == req_flags) {
	kill_list.append( &li );
	found++;
      }
    }
    if (!found) {
      return ComplexRep::link_not_present;
    }

    assert(found==1);
    // delete raigs found in previous phase
    forall_items( li, kill_list ) {
      list_item* victim = kill_list.inf(li);
      existing_raigs->del_item( *victim );
    } 
    kill_list.clear();
    return ComplexRep::success;
  }
}


//--------------------------------------------------------------------
ComplexRep::errcode 
ComplexRep::Evaluate(LogosGraph* g,
		     int lgport1, int lgport2,
		     service_class sc, 
		     double& weight, 
		     double& mcr, 
		     double& acr, 
		     double& ctd, 
		     double& cdv, 
		     double& clr0, 
		     double& clr01) const
{
  // not supported
  return (ComplexRep::not_supported);
}


//--------------------------------------------------------------------
void ComplexRep::Print_Topology(ostream& os) const {
  list<ig_resrc_avail_info*>* raig = 0;
  int i,j;

  list<int>* ports = Get_Logical_Ports_List();
  list_item li1,li2;

  os << "ComplexRep Topology of LGN: ";
  os << *_lgnode << endl << endl;

  // top row of the table
  os << "      ";
  forall_items(li1, *ports) {
    j = ports->inf(li1);
    os << "    " << j;
  }
  os << endl << "      ";
  forall_items(li1, *ports) {
    os << "----";
  }
  os << endl;


  forall_items(li1, *ports) {
    i = ports->inf(li1);
    // left hand column of the table
    os << " " << i << "  | ";
    forall_items(li2, *ports) {
      j = ports->inf(li2);
      if (Get_RAIG(i,j,raig) == ComplexRep::success) {
	// * means the link is in the complex rep
	os << " *  ";
      }
      else {
	// . means the link is not in the complex rep
	os << " .  ";
      }
    }
    os << endl;
  }
  os << endl;

  delete ports;
}


//--------------------------------------------------------------------
void ComplexRep::Print_Metrics(ostream& os) const {
  list<ig_resrc_avail_info*>* raig = 0;
  int i,j;

  os << "ComplexRep Metrics" << endl;
  list<int>* ports = Get_Logical_Ports_List();
  list_item li1,li2;

  os << "ComplexRep Metrics of LGN: ";
  os << *_lgnode << endl << endl;

  forall_items(li1, *ports) {
    i = ports->inf(li1);

    forall_items(li2, *ports) {
      j = ports->inf(li2);

      if (Get_RAIG(i,j,raig) == ComplexRep::success) {
	// * means the link is in the complex rep
	list_item ri;
	forall_items(ri, *raig) {
	  ig_resrc_avail_info* one_raig = raig->inf(ri);
	  const u_int flg = one_raig->GetFlags();
	  if (flg & 0x8000) os << "CBR ";       // Constant Bit Rate
	  if (flg & 0x4000) os << "RTVBR ";     // Realtime Variable Bit Rate
	  if (flg & 0x2000) os << "NRTVBR ";    // Non-realtime Variable Bit Rate
	  if (flg & 0x1000) os << "ABR ";       // Available Bit Rate
	  if (flg & 0x0800) os << "UBR ";       // Unspecified Bit Rate
	  if (flg & 0x0001) os << "GCAC_CLP ";  // Cell Loss Priority bit
	  os << ": ";

	  const int weight = one_raig->GetAdminWeight();
	  const int mcr = one_raig->GetMCR();
	  const int acr = one_raig->GetACR();
	  const int ctd = one_raig->GetCTD();
	  const int cdv = one_raig->GetCDV();
	  const int clr0 = one_raig->GetCLR0();
	  const int clr01 = one_raig->GetCLR01();

	  os << "weight=" << weight << ", ";
	  os << "mcr=" << mcr << ", ";
	  os << "acr=" << acr << ", ";
	  os << "ctd=" << ctd << ", ";
	  os << "cdv=" << cdv << ", ";
	  os << "clr0=" << clr0 << ", ";
	  os << "clr01=" << clr01 << ", ";
	  os << endl;
	}
      }
      else {
	// . means the link is not in the complex rep
	os << " .  ";
      }
    }
    os << endl;
  }
}


//--------------------------------------------------------------------
// set the time before this complex node representation must be updated

void ComplexRep::Set_TTL(double seconds) {
  _ttl = seconds;
}


//--------------------------------------------------------------------
// get the time before this complex node representation must be updated

double ComplexRep::Get_TTL(void) const {
  return _ttl;
}


//--------------------------------------------------------------------
// get the peergroup leader node address

NodeID * ComplexRep::Get_PGL(void) const 
{
  return _pgl->copy();
}


//--------------------------------------------------------------------
// get the logical group node address

NodeID * ComplexRep::Get_LGN(void) const
{
  return _lgnode->copy();
}

//--------------------------------------------------------------------
// get a list of logical ports for this complex node representation

list<int>* ComplexRep::Get_Logical_Ports_List(void) const {
  list<int>* ports = new list<int>;
  dic_item di;
  forall_items( di, *_links ) {
    int id = _links->key(di);
    ports->append(id);
  }
  return ports;
}

//--------------------------------------------------------------------
// get the list of RAIGS of a link in the complex node representation
ComplexRep::errcode 
ComplexRep::Get_RAIG(int lgport1,int lgport2,
		     list<ig_resrc_avail_info*>*& raig) const {

  dic_item d1 = _links->lookup(lgport1);
  if ( !d1 )
    return ComplexRep::lgport1_out_of_range;

  dictionary< int , 
              list<ig_resrc_avail_info*>* >* row 
    = _links->inf(d1);

  dic_item d2 = row->lookup(lgport2);
  if ( !d2 )
    return ComplexRep::lgport2_out_of_range;
  
  list<ig_resrc_avail_info*>* entry = row->inf(d2);
  raig = entry;
  if (entry->size() > 0)
    return ComplexRep::success;
  else
    return ComplexRep::link_not_present;
}


//--------------------------------------------------------------------
// determine if this is a valid floodable object
bool ComplexRep::Validate(void) const {

  list<ig_resrc_avail_info*>* existing_raigs = 0;
  ComplexRep::errcode err = 
    Get_RAIG( 0, 0, existing_raigs);
  
  return ((err==ComplexRep::success) && 
	  (existing_raigs->size() == 
	   (int)(ComplexRep::ALL_SERVICE_CLASSES)));
}


//--------------------------------------------------------------------
// add a logical port to the complex node representation
bool ComplexRep::Add_LogicalPort(int lgportnumber) {
  list<int>* ports = Get_Logical_Ports_List();
  list_item li;

  dic_item exists = _links->lookup(lgportnumber);
  if (exists) {
    delete ports;
    return false;
  }

  forall_items(li,*ports) {
    int id = ports->inf(li);
    
    dic_item di1 = _links->lookup(id);
    assert(di1);
    dictionary< int ,list<ig_resrc_avail_info*> *> * row
      = _links->inf(di1);

    dic_item di2 = row->lookup(lgportnumber);
    assert( !di2 );
    row->insert(lgportnumber, new list<ig_resrc_avail_info*>); 
  }

  dictionary< int,list<ig_resrc_avail_info*>* > * row 
    = new dictionary< int,list<ig_resrc_avail_info*>* >;

  _links->insert(lgportnumber, row);
  forall_items(li,*ports) {
    int id = ports->inf(li);
    row->insert(id,new list<ig_resrc_avail_info*>);
  }
  row->insert(lgportnumber,new list<ig_resrc_avail_info*>);
  
  delete ports;
  return true;
}


//--------------------------------------------------------------------
// remove a logical port from the complex node representation
bool ComplexRep::Remove_LogicalPort(int lgportnumber) {
  list<int>* ports = Get_Logical_Ports_List();
  list_item li;

  dic_item exists = _links->lookup(lgportnumber);
  if (!exists) {
    delete ports;
    return false;
  }

  forall_items(li,*ports) {
    int id = ports->inf(li);
    if (id==lgportnumber) continue;

    dic_item di1 = _links->lookup(id);
    assert(di1);
    dictionary< int ,list<ig_resrc_avail_info*> *> * row
      = _links->inf(di1);

    dic_item di2 = row->lookup(lgportnumber);
    assert( di2 );
    list<ig_resrc_avail_info*>* list_raigs = 
      row->inf(di2);
    while (list_raigs->size() > 0) {
      ig_resrc_avail_info* raig = list_raigs->inf( list_raigs->first() );
      list_raigs->del_item( list_raigs->first() );
      delete raig;
      raig = 0;
    };
    list_raigs->clear();
  }

  dictionary< int ,list<ig_resrc_avail_info*> *> * row
    = _links->inf(exists);

  while (row->size() > 0) {
    list<ig_resrc_avail_info*>* list_raigs = row->inf( row->first() );
    row->del_item( row->first() );
    delete list_raigs;
    list_raigs = 0;
  };
  row->clear();
  delete row;

  _links->del_item(exists);

  delete ports;
  return true;
}


//--------------------------------------------------------------------
// this complex node's TTL has expired, time to update
void ComplexRep::Callback(void) 
{
  Cancel(this);

  DIAG("sim.agg", DIAG_INFO,
       cout << "time is " << theKernel().CurrentTime() << endl
            << "callback occurring in ComplexRep " << (long)this << endl;);

  if ( _lgnode != 0 ) {
    LogosGraph * current_topology = 
      _logos->GetGraph( _lgnode );     
		       // _level, Logos::aggregation );
    _aggr->Update_Complex_Node( *current_topology, 
				*this );

    if (Validate()) {
      Reoriginate_NSP();
    }
    else {
      DIAG("sim.agg", DIAG_ERROR,
	   cout << "ComplexRep of " << _lgnode->Print() << " was not originated \n";
	   cout << " at time " << theKernel().CurrentElapsedTime() << " because it did not provide a default radius 0,0 link.\n");
    }
    
    delete current_topology;
  }
  ExpiresIn( Get_TTL() );
  Register(this);
}


//--------------------------------------------------------------------
// insert NSP into the DB
void ComplexRep::Reoriginate_NSP(void) 
{
  list<ig_nodal_state_params *> * nsplist = new list<ig_nodal_state_params *>;
  list<ig_resrc_avail_info *> * raig = 0;
  int i,j;

  list<int>* ports = Get_Logical_Ports_List();
  list_item li1,li2;

  forall_items(li1, *ports) {
    i = ports->inf(li1);
    forall_items(li2, *ports) {
      j = ports->inf(li2);
      if (Get_RAIG(i,j,raig) == ComplexRep::success) {

	if (!raig->empty()) {
	  assert((i==0) || (i!=j));
	  ig_nodal_state_params * nsp = 
	    new ig_nodal_state_params(0, i, j);  // Default radius

	  list_item rit;
	  forall_items(rit, *raig) {
	    ig_resrc_avail_info* one_raig = raig->inf(rit);
	    nsp->AddRAIG( (ig_resrc_avail_info*)( one_raig->copy() ));
	  }

	  nsplist->append(nsp);
	}
      }
      else {
	// no raig between port i and port j, keep going...
      }
    }
  }

  assert( ! nsplist->empty() );

  // ----------------------------------------------
  NodeID * phys = _leadership->PhysicalNodeID();
  NodeID * lgn  = phys->copy();
  lgn->SetLevel(_level);

  char str[128];
  sprintf(str, "%d", ports->size() - 1 );
  theNetStatsCollector().ReportNetEvent("NSP_Aggr",
					lgn->Print(), // OwnerName(),
					str, lgn);
  delete lgn;
  delete phys;
  // ----------------------------------------------
  _leadership->GenerateComplexRep(_level, nsplist);

  delete ports;
}

//--------------------------------------------------------------------
// compute the RAIG flags, based on service class
u_short ComplexRep::service_class_2_flags(service_class sc) const {
  u_short flags;

  switch (sc) {
  case ComplexRep::CBR:
    flags = RAIG_FLAG_CBR;
    break;
  case ComplexRep::RTVBR:
    flags = RAIG_FLAG_RTVBR;
    break;
  case ComplexRep::NRTVBR:
    flags = RAIG_FLAG_NRTVBR;
    break;
  case ComplexRep::ABR:
    flags = RAIG_FLAG_ABR;
    break;
  case ComplexRep::UBR:
    flags = RAIG_FLAG_UBR;
    break;
  default:
  case ComplexRep::ALL_SERVICE_CLASSES:
    flags = 0;
    break;
  }

  return flags;
}


//--------------------------------------------------------------------
void ComplexRep::ReAggregate_PortCountChanged(void) {

  if ( _lgnode != 0 ) {
    LogosGraph * current_topology = 
      _logos->GetGraph( _lgnode );     
		       // _level, Logos::aggregation );
    _aggr->Update_Complex_Node( *current_topology, 
				*this );

    if (Validate()) {
      Reoriginate_NSP();
    }
    else {
      DIAG("sim.agg", DIAG_ERROR,
	   cout << "ComplexRep of " << _lgnode->Print() << " was not originated \n";
	   cout << " at time " << theKernel().CurrentElapsedTime() << " because it did not provide a default radius 0,0 link.\n");
    }
    
    delete current_topology;
  }
}

//--------------------------------------------------------------------
// print the error value
void Print_ComplexRep_Error(ComplexRep::errcode e, ostream& os) {
  switch (e) {
  case ComplexRep::success:
    os << "success";
    break;
  case ComplexRep::lgport1_out_of_range:
    os << "lgport1_out_of_range";
    break;
  case ComplexRep::lgport2_out_of_range:
    os << "lgport2_out_of_range";
    break;
  case ComplexRep::link_not_present:
    os << "link_not_present";
    break;
  case ComplexRep::ambiguous_service_class:
    os << "ambiguous_service_class";
    break;
  case ComplexRep::not_supported:
  default:
    os << "not_supported";
    break;
  };
}

#endif
