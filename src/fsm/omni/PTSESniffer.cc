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
static char const _PTSESniffer_cc_rcsid_[] =
"$Id: PTSESniffer.cc,v 1.7 1999/01/28 17:10:59 mountcas Exp $";
#endif

#include <FW/basics/diag.h>
#include <codec/pnni_ig/InfoGroup.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/DBKey.h>
#include "PTSESniffer.h"

//--------------------------------------------------------------------------------------
void PTSESniffer::Enable(void) {
  _enabled = true;
}

//--------------------------------------------------------------------------------------
void PTSESniffer::Disable(void) {
  _enabled = false;
}

//--------------------------------------------------------------------------------------
void PTSESniffer::Alarm(NodeID * location, DBKey * key) 
{
  assert( _enabled );
  cout << "***** ALARM *****  PTSESniffer reports" << endl;
  cout << "         at node:  " << location->Print() << endl;
  char buf[256];
  key->Print(buf);
  cout << "                    " << buf << endl;
}


//--------------------------------------------------------------------------------------
bool PTSESniffer::AttemptDetection(NodeID* location, DBKey* key) {
  assert( location && key );

  if ( ! _enabled)
    return false;

  bool location_test = false;
  if ( _any_loc ||
       ( (memcmp( location->GetATM(), _location->GetATM(), 19) == 0 ) // ignore selector
	 && 
	 (location->GetLevel() >= _min_loc_level)
	 &&
	 (location->GetLevel() <= _max_loc_level) )) {
    location_test=true;
  }
  
  bool originator_test = false;
  if ( _any_origin ||
       ( (memcmp( key->GetOrigin() + 2, _originator->GetATM(), 19) == 0 ) // ignore selector
	 && 
	 (key->GetOrigin()[0] >= _min_origin_level)
	 &&
	 (key->GetOrigin()[0] <= _max_origin_level) )) {
    originator_test=true;
  }
  
  bool type_test = false;
  InfoGroup::ig_id ptsetype;
  key->GetType( ptsetype );
  int type = (int) ptsetype;
  if (_any_type || _types.lookup( type )) {
    type_test = true;
  }

  if (location_test   &&
      originator_test &&
      type_test) {

    Alarm( location, key);
    return true;
  }

  return false;
}

//--------------------------------------------------------------------------------------
PTSESniffer::PTSESniffer(void) :
  _originator(0), _min_origin_level(0),
  _max_origin_level(160), _any_origin(true),
  _location(0), _min_loc_level(0), 
  _max_loc_level(160), _any_loc(true),
  _any_type(true)
{
  Disable();
}

//--------------------------------------------------------------------------------------
PTSESniffer::~PTSESniffer() {
  if (_location) {
    delete _location;
  }

  _types.clear();
  
}

//--------------------------------------------------------------------------------------
void PTSESniffer::SetLocationSpecific(const char* id, int min_level, int max_level) {
  if (_location) {
    delete _location;
  }

  if (_originator) {
    delete _originator;
  }

  _location = new NodeID(id);
  _max_loc_level = max_level;
  _min_loc_level = min_level;
  _any_loc = false;
}

//--------------------------------------------------------------------------------------
void PTSESniffer::SetLocationAny(void) {
  _any_loc = true;
}
  
//--------------------------------------------------------------------------------------
void PTSESniffer::SetOriginatorSpecific(const char* id, int min_level, int max_level) {
  if (_originator) {
    delete _originator;
  }
  _originator = new NodeID(id);
  _max_origin_level = max_level;
  _min_origin_level = min_level;
  _any_origin = false;
}

//--------------------------------------------------------------------------------------
void PTSESniffer::SetOriginatorAny(void) {
  _any_origin = true;
}

//--------------------------------------------------------------------------------------
bool PTSESniffer::AddType(int t) {
  _any_type = false;
  if (_types.lookup(t)) {
    return false;
  }
  _types.append(t);
  return true;
}

//--------------------------------------------------------------------------------------
bool PTSESniffer::AddType(InfoGroup::ig_id t) {
  AddType( (int) t );
}

//--------------------------------------------------------------------------------------
bool PTSESniffer::AddType(const char * str) {

  InfoGroup::ig_id type = name2ig( str );

  if ( type != InfoGroup::ig_unknown_id )
    AddType( (int) type );
  else
    diag("PTSESniffer", DIAG_DEBUG, "%s is not a valid IG type!\n", str);
}

//--------------------------------------------------------------------------------------
bool PTSESniffer::RemoveType(int t) {
  list_item li = _types.lookup(t);
  if ( ! li) {
    return false;
  }
  _types.del_item(li);
  return true;
}

//--------------------------------------------------------------------------------------
bool PTSESniffer::RemoveType(InfoGroup::ig_id t) {
  RemoveType( (int) t );
}

//--------------------------------------------------------------------------------------
bool PTSESniffer::RemoveType(const char * str) {

  InfoGroup::ig_id type = name2ig( str );

  if ( type != InfoGroup::ig_unknown_id )
    RemoveType( (int) type );
  else
    diag("PTSESniffer", DIAG_DEBUG, "%s is not a valid IG type!\n", str);
}

//--------------------------------------------------------------------------------------
void PTSESniffer::AllTypes(void) {
  _any_type = true;
  _types.clear();
}



