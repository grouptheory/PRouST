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
static char const _Shareable_cc_rcsid_[] =
"$Id: Shareable.cc,v 1.3 1999/03/05 17:30:30 marsh Exp $";
#endif

#include <common/cprototypes.h>

#include "Shareable.h"
#include "ShareableRegistry.h"
#include "Interface.h"
#include <FW/basics/diag.h>

Shareable::Shareable(void) : _name(0), _blocking(false) { }

Shareable::~Shareable() 
{ 
  dic_item di;
  forall_items(di, _permissions)
    delete _permissions.key(di);
  _permissions.clear(); 

  if (di = theShareableRegistry()._s2t.lookup( (Shareable *)this ))
    theShareableRegistry()._s2t.del_item(di);
}

fw_Interface * Shareable::QueryInterface(const char * requested) const
{
  // Don't bother looking if we haven't set our Moniker yet
  if (!_name.asString())
    return 0;

  fw_Interface * rval = 0;
  Moniker req(requested);
  trinode<Shareable *> * tri = share_to_trie(this), * targ = 0;
  Shareable * shar = 0;

  int nf = 0;
  targ = theShareableRegistry().locate(req, tri, nf);

  if (nf == 1) {
    // if we found one, we had better have targ set properly
    assert(targ);

    shar = targ->data();
    rval = shar->HasPermission(this);
  }

  if (rval) {
    diag("FW", DIAG_DEBUG, "%s requested fw_Interface from %s and obtained fw_Interface %lx from %s.\n",
	 GetName().asString(), requested, rval, rval->NameOfShareable());
  }
  return rval;
}

void Shareable::SetMoniker(const char * name)
{
  _name = name;
}

fw_Interface * Shareable::HasPermission(const Shareable * requestor) const
{
  fw_Interface * rval = 0;
  Moniker req = requestor->GetName(); 	
  int highscore = -(MAXINT), depth = -(MAXINT);

  dictionary< Moniker *, fw_Interface * > matches;

  dic_item di;
  forall_items(di, _permissions) {
    Moniker * iter = _permissions.key(di);
    
    if (iter->satisfied_by(req)) {
      int dp = compute_depth((char *)((Shareable *)requestor)->FullySpecifiedName().asString(), 
			     iter->asString());
      
      if (dp > depth) {
	depth = dp;
	matches.insert( new Moniker( *iter ), _permissions.inf(di) );
      }
    }
  }

  while (matches.empty() == false &&
	 matches.size() != 1) {

    // Find out if there are permissions w/o any wildcards
    bool rem_wilds = false;
    
    forall_items(di, matches) {
      Moniker * iter = matches.key(di);

      if ( ! strchr( iter->identifier(), '*' ) )
	rem_wilds = true;
    }

    // If there are permissions w/o wildcards remove all permissions w/ wildcards
    if (rem_wilds) {
      dic_item ndi;
      for ( di = matches.first(); di; di = ndi) {
	ndi = matches.next(di);

	Moniker * iter = matches.key(di);
	if ( strchr( iter->identifier(), '*' ) ) {
	  delete matches.key(di);
	  matches.del_item(di);
	}
      }
    }
    
    // Now the matches list should contain either all wildcards or no wildcards
    if (matches.size() != 1) {
      // prune all of the identifiers and continue;
      dic_item ndi;
      for ( di = matches.first(); di; di = ndi) {
	ndi = matches.next(di);

	Moniker * iter = matches.key(di);
	iter->removeIdentifier();
	if (iter->isNull()) {
	  delete iter;
	  matches.del_item(di);
	}
      }
    }
  }    

  if ( matches.empty() == false ) {
    assert( matches.size() == 1 );
    rval = matches.inf( matches.first() );
  }
  return rval;
}

// INVARIANT: token is present in iter
int compute_depth(char * iter, const char * token)
{
  int rval = 0;
  char * tmp = strstr(iter, token);

  if (tmp) {
    while ( iter != tmp ) {
      if (*iter == '.')
	rval++;
      iter++;
    }
  }
  return rval;
}

// INVARIANT: requestor is the fully specified name of the requesting Shareable
int compute_score(Moniker & requestor, Moniker & permission)
{
  int rval = -(MAXINT);

  // the requestor should already satisfy the permission, but just in case ...
  if (! permission.satisfied_by(requestor))
    return rval;

  // iterate over the tokens in the permission searching for them in the requestor
  char * token = permission.currentToken(), * ptr = (char *)requestor.asString();
  rval = 0;
  while ( token && *token ) {
    // if token has an * suffix, remove it
    if ( token [ strlen(token) - 1 ] == '*' )
      token [ strlen(token) - 1 ] = 0;

    rval += compute_depth(ptr, token);

    ptr = strstr(ptr, token);
    
    delete [] token;
    token = permission.nextToken();
  }
  return rval;
}

Moniker & Shareable::FullySpecifiedName(void)
{
  _full_name.clear();

  trinode<Shareable *> * current = share_to_trie(this);

  while (current && !(current->blocking())) {
    Shareable * s = current->data();
    _full_name.prepend( s->GetName() );
    current = current->parent();
  }

  return _full_name;
}

const Moniker & Shareable::GetName(void) const
{
  return _name;
}

void Shareable::SetName(const Moniker & name)
{
  _name = name;
}

void Shareable::AddPermission(const char * key, fw_Interface * val)
{
  Moniker * k = new Moniker(key);
  AddPermission(k, val);
}

void Shareable::AddPermission(Moniker * key, fw_Interface * val)
{
  if (_permissions.lookup(key)) {
    cerr << key << " was already inserted into the Database previously!" << endl;
    abort();
  }
  _permissions.insert(key, val);
}

void Shareable::SetBlocking(bool b)
{
  _blocking = b;
}

bool Shareable::IsBlocking(void) const
{ return _blocking; }
