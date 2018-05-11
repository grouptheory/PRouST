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

#if !defined(LINT)
static char const _VisitorType_cc_rcsid_[] =
"$Id: VisitorType.cc,v 1.1 1999/01/21 14:56:25 mountcas Exp $";
#endif
#include <common/cprototypes.h>


#include <FW/basics/VisitorType.h>
#include <FW/basics/Registry.h>

Registry* vistype::_registry=0;

vistype::vistype(const char * const name) 
{ 
  _parents = new list<vistype*>;

  char * stripws(const char *);

  if (!_registry) 
    InitFW();

  // The registry will delete the memory alloc'ed by stripws.
  char * tmp = (char *)name; // stripws(name);
  _registry->Insert(tmp, this);
  // delete [] tmp;
}

vistype::~vistype()
{
  if (_parents) {
    // _parents->clear();
    delete _parents;
    _parents = 0;
  }
}

const char * const vistype::Name(void) const
{ 
  if (!_registry) InitFW();
  return _registry->GetName(this);
}

bool vistype::Is_A(const vistype * other) const 
{
  if (!other)
    return (bool)false;

  if (this == other) 
    return (bool)true;

  if (_parents->size() > 0) {
    list_item li;
    forall_items(li, *_parents) {
      vistype * father = _parents->inf(li);
      if (father->Is_A(other)) 
	return (bool)true;
    }
  }
  // Anything else is bad ...
  return (bool)false;
}

bool vistype::Is_Exactly(const vistype* other) const 
{
  // This should work even if other == 0
  if (this == other) 
    return (bool)true;
  else 
    return (bool)false;
}

ostream& operator << (ostream & os, const vistype & v)
{
  os << v.Name() << flush;
  return os;
}

ostream& operator << (ostream & os, const VisitorType & v) 
{
  os << *(v._type);
  return os;
}

// -------------------- VisitorType -----------------------
VisitorType::VisitorType(const vistype& vt) : _type(&vt) {}

VisitorType::~VisitorType() { }
  
bool VisitorType::Is_Exactly(const VisitorType* other) const 
{
  assert( other != 0 );
  return ( _type->Is_Exactly( other->_type ) );
}

bool VisitorType::Is_A(const VisitorType* other) const 
{
  assert( other != 0 );
  return ( _type->Is_A( other->_type ) );
}

const char * const VisitorType::Name (void) const 
{
  return _type->Name();
}

