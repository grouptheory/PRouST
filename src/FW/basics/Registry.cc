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
static char const _Registry_cc_rcsid_[] =
"$Id: Registry.cc,v 1.1 1999/01/21 14:56:25 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/basics/Registry.h>
#include <FW/basics/Visitor.h>
#include <FW/basics/Conduit.h>
#include <FW/basics/VisitorType.h>
#include <FW/basics/diag.h>
#include <FW/kernel/Kernel.h>
#include <FW/kernel/SimEvent.h>
#include <FW/actors/Actor.h>

#include <iostream.h>
#include <new.h>

Registry::Registry(void) 
{
  _map    = new dictionary<const char *, const vistype *> ;
  _invmap = new dictionary<const vistype *, const char *> ;
}

Registry::~Registry() 
{ 
  dic_item di;
  forall_items(di, *_map) {
    // This removes the char * from _invmap as well
    delete [] _map->key(di);
    _map->change_inf(di,(const vistype*)0);
  }
  _map->clear();

  forall_items(di,*_invmap) {
    // delete [] _invmap->inf(di);
    _invmap->change_inf(di,(char*)0);
  }
  _invmap->clear();
  delete _map;
  delete _invmap;
  _map = 0;
  _invmap = 0;
}

void Registry::Insert(const char * const name, const vistype* v)
{
  const vistype* rval=0;
  if ((rval = Lookup(name)) != 0) return;
  _Insert(name,v);
}

const vistype* Registry::_Insert(const char * const name, const vistype* v)
{
  char * tmp = new char [ strlen(name) + 1 ];
  strcpy(tmp, name);
  _map->insert(tmp, v);
  _invmap->insert(v, tmp);
  return v;
}

const vistype* Registry::Lookup(const char * const name)
{
  dic_item di;

  forall_items(di, *_map) {
    if (!strcmp(_map->key(di), name)) 
      return _map->inf(di);
  }
  return 0;
}

const char * const Registry::GetName(const vistype* v)
{
  dic_item d;
  if (d = _invmap->lookup(v))
    return _invmap->inf(d);

  return "<class-not-registered>";
}

void DeleteFW(void) 
{
  // Free the memory alloc'ed for the Registry
  delete Conduit::_registry;

  // Set all ptrs to 0
  Conduit::_registry = Visitor::_registry =
  Actor::_registry   = vistype::_registry = 0;

  if (Conduit::_disconnect_type) {
    delete (VisitorType *)Conduit::_disconnect_type;
    Conduit::_disconnect_type=0;
  }
  if (Conduit::_clusterkiller_type) {
    delete (VisitorType *)Conduit::_clusterkiller_type;
    Conduit::_clusterkiller_type = 0;
  }

  FreeDiag();
}

void InitFW(void)
{
  // This is always called right?
  set_new_handler(mallocErrHandler);

  Registry * r = new Registry;
  // Be sure memory was alloc'ed for the Registry.
  assert(r);

  Visitor::_registry =
    Actor::_registry =
    vistype::_registry =
    Conduit::_registry = r;
  
  InitDiag();
}

int memex(int size = 12)
{
  char * block[size]; 
  int    sz = 1, i;  

  for (i = 0; i < size; i++) {  
    block[i] = new char[sz];
    for (int j = 0; j < sz; j++) 
      block[i][j] = (char)-1; 
    sz *= 2;
  } 
  for (i =  0; i < size; i++)
    delete [] block[i]; 

  return size;
}

extern "C" {
  void __pure_virtual( ) { 
    cerr << "SEAN detects that a pure virtual method was called, dumping core ..." << endl;
    abort();
  }
  void _pure_virtual( ) { 
    cerr << "Sean detects that a pure virtual method was called, dumping core ..." << endl;
    abort();
  }
  void mallocErrHandler(void) {
    cerr << "Unable to allocate requested memory, dumping core ..." << endl;
    abort();
  }
};

