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
static char const _InterfaceRegistry_cc_rcsid_[] =
"$Id: ShareableRegistry.cc,v 1.2 1999/02/09 16:14:31 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include "ShareableRegistry.h"
#include "Shareable.h"
#include <FW/basics/diag.h>
#include <fstream.h>

bool tokens_equal(const char * s1, const char * s2);

trinode<Shareable *> * ShareableRegistry::_root = 0;
ShareableRegistry *     ShareableRegistry::_me   = 0;

ShareableRegistry::ShareableRegistry(void) 
{ 
  if (!_root) {
    _root = new trinode<Shareable *>("ROOT", 0);  // (".", 0);
    _root->set_blocking(true);
  }
}

ShareableRegistry::~ShareableRegistry() { }

void ShareableRegistry::insert(const char * name, Shareable * s)
{
  diag("FW.interface", DIAG_DEBUG,
       "Inserting %s into the hierarchy.\n", name);

  // INVARIANT:  name is a fully specified name
  Moniker n(name);
  char * tmp = n.reverseToken(1);
  s->SetName( tmp );
  delete [] tmp;

  trinode<Shareable *> * current = _root;
  bool found = false;

  while ( 1 ) {
    char * token = n.currentToken();

    if (!token)
      break;

    bool found = false;
    dic_item di;
    const dictionary<const char *, trinode<Shareable *> *> & children = current->children();
    forall_items(di, children) {
      Moniker loc = children.key(di);

      if (tokens_equal(token, loc.asString())) {
	//      if (!strcmp(token, loc.asString())) {
	current = children.inf(di);
	found = true;
	break;
      }
    }
    if (!found) {
      // This must be a new leaf ... so add it
      if (!*(s->GetName().asString())) {
	DIAG("FW.interface", DIAG_FATAL, "Attempting to insert a trienode without a name!\n");
      }
      trinode<Shareable *> * tri = 0;
      dic_item di = 0;
      if (di = _s2t.lookup(s)) {
	tri = _s2t.inf(di);
	_s2t.del_item(di);
      } else
	tri = new trinode<Shareable *>(s->GetName().asString(), s);

      if (s->IsBlocking())
	tri->set_blocking(true);

      current->add_child(tri);
      _s2t.insert(s, tri);
    }

    delete [] token;
    delete [] n.nextToken();
  }
}

void ShareableRegistry::modify(const char * name, Shareable * s)
{
  diag("FW.interface", DIAG_DEBUG,
       "Updating %s's position within the hierarchy.\n", name);
  trinode<Shareable *> * loc = share_to_trie(s);
  Moniker fs = s->FullySpecifiedName();

  trinode<Shareable *> * parent = loc->parent();
  // remove it from the parent
  if (parent) parent->rem_child(loc);
  // now we need to place ourself in the new location
  insert(name, s);
}

trinode<Shareable *> * ShareableRegistry::locate_below(Moniker requested, 
						       trinode<Shareable *> * requestor, 
						       int & num_found)
{
  trinode<Shareable *> * result = 0;
  const char * token  = requested.currentToken();
  
  trinode<Shareable *> * tmp = 0;
  // search requesting node's immediate peers
  dic_item di;

  if (!requestor || !*(requestor->name())) {
    diag("FW.interface", DIAG_FATAL, "Trienode (%x) with no name!\n", requestor);
  }

  const dictionary<char const *, trinode<Shareable *> *> chs = 
    requestor->children();

  if (!chs.empty()) {
    forall_items(di, chs) {
      tmp = chs.inf(di);
      
      if (tokens_equal(tmp->name(), token)) {
	//      if (!strcmp(tmp->name(), token)) {
	// this node matches the token
	delete [] token;
	if ((token = requested.nextToken())) {
	  // If there is another token ...
	  Moniker req = requested.remainder();
	  // search all of tmp's children for the remaining tokens
	  trinode<Shareable *> * tmp_res = locate_below(req, tmp, num_found);
	  if (tmp_res)
	    result = tmp_res;
	} else {
	  // no tokens left, so this must be what we're looking for
	  result = tmp;
	  num_found++;
	} 
      }	  
      // else name didn't match token, so continue
    }
    
    // we should probably search the requesting node's children for
    // completeness, but I suppose this should be an invariant:
    // any requesting node MUST be a leaf (unless Expanders want to
    // start requesting Interfaces ...)
    
    // now search the children of my children
    forall_items(di, chs) {
      tmp = chs.inf(di);
      // we do not want to traverse upward toward the root just yet
      trinode<Shareable *> * tmp_res = locate_below(requested, tmp, num_found);
      if (tmp_res)
	result = tmp_res;
    }
  }
  if (token) delete [] token;
  return result;
}

trinode<Shareable *> * ShareableRegistry::locate(Moniker requested, 
						 trinode<Shareable *> * requestor, 
						 int & num_found)
{
  diag("FW.interface", DIAG_DEBUG,
       "Searching for %s starting at %s.\n", 
       requested.asString(), requestor->name());
  // Invariant: the Moniker requested must contain no wildcards, it
  // may contain incomplete hierarchy information, and must contain
  // the identifier of the Shareable it is looking for.
  // Invariant: the Moniker at each node must contain only the
  // identifier for that node (i.e. only one token)
  trinode<Shareable *> * result = 0;
  const char * token = requested.currentToken();
  
  trinode<Shareable *> * iterator = requestor;

  while (iterator && (num_found < 2)) {
    trinode<Shareable *> * tmp = 0;
    // search requesting node's immediate peers
    dic_item di;

    if (! iterator->parent()) {
      diag("FW.interface", DIAG_FATAL, 
	   "%s is making a request, but has no parent.\n"
	   "Expanders are not allowed to request interfaces.\n", iterator->name());
    }

    const dictionary<char const *, trinode<Shareable *> *> children = 
      iterator->parent()->children();
    forall_items(di, children) {
      tmp = children.inf(di);

      if (tokens_equal(tmp->name(), token)) {
	//      if (!strcmp(tmp->name(), token)) {
        // this node matches the token
	delete [] token;
        if ((token = requested.nextToken())) {
          // If there is another token ...
          Moniker req = requested.remainder();
          // search all of tmp's children for the remaining tokens
	  trinode<Shareable *> * tmp_res = locate_below(req, tmp, num_found);
	  if (tmp_res)
	    result = tmp_res;
        } else {
          // no tokens left, so this must be what we're looking for
          result = tmp;
          num_found++;
        } 
      }	  
      // else name didn't match token, so continue
    }
    
    // we should probably search the requesting node's children for
    // completeness, but I suppose this should be an invariant:
    // any requesting node MUST be a leaf (unless Expanders want to
    // start requesting Interfaces ...)
    
    if (num_found > 1)
      break;  // no point in continuing if we've failed
    
    // now search the children of the requestor's peers    
    forall_items(di, children) {
      tmp = children.inf(di);
      // we do not want to traverse upward toward the root just yet
      trinode<Shareable *> * tmp_res = locate_below(requested, tmp, num_found);
      if (tmp_res) {
	if ( tmp_res == result )
	  num_found--;
	else
	  result = tmp_res;
      }
    }
    
    if (num_found > 1)
      break;  // no point in continuing if we've failed
    
    iterator = iterator->parent();
    
    if ( iterator->blocking() )
      break;  // we've reached the end of this partition
  }
  
  if (num_found != 1)
    result = 0;
  
  if (token) delete [] token;
  return result;
}

void ShareableRegistry::save(const char * filename) const
{
  ofstream ofs(filename);

  print(ofs);
}

void ShareableRegistry::print(ostream & os) const
{
  os << "!birth " << _root->name() << " !Protocol" << endl;
  write(_root, os);
  os << "xxxxxxxxxxx" << endl;
}

void ShareableRegistry::write(trinode<Shareable *> * current, ostream & os) const
{
  const dictionary<const char *, trinode<Shareable *> *> & children = current->children();

  if (children.size() > 0) {
    dic_item di;
    forall_items(di, current->children()) {
      trinode<Shareable *> * child = current->children().inf(di);
      
      if (current == child) {
	diag("FW.interface", DIAG_FATAL, 
	     "Something's amiss, trinode %s has itself for a child!\n", 
	     current->name());
      }
      
      os << "!birth " << child->name();
      if (child->blocking()) 
	os << " !Protocol" << endl;
      else
	os << " !Factory" << endl;
      os << "!connected " << current->name() << " 3 !to " 
	 << child->name() << " 3" << endl;
    }
    forall_items(di, current->children()) {
      trinode<Shareable *> * child = current->children().inf(di);
      write(child, os);
    }
  }
}

ShareableRegistry & theShareableRegistry(void)
{
  if (!ShareableRegistry::_me)
    ShareableRegistry::_me = new ShareableRegistry();
  return *(ShareableRegistry::_me);
}

trinode<Shareable *> * share_to_trie(const Shareable * s)
{
  dic_item di;
  if (di = theShareableRegistry()._s2t.lookup((Shareable *)s))
    return theShareableRegistry()._s2t.inf(di);
  return 0;
}

Shareable * trie_to_share(const trinode<Shareable *> * t)
{
  return t->data();
}

void ShareableRegistry::update(const char * name, Shareable * s)
{
  if (!share_to_trie(s))
    insert(name, s);
  else
    modify(name, s);
}

// check only upto ':'
bool tokens_equal(const char * s1, const char * s2)
{
  char * stripaddr(const char * name);
  bool rval = false;

  if ((s1 && !s2) ||
      (s2 && !s1))
    return rval;

  char * new_s1 = stripaddr(s1);
  char * new_s2 = stripaddr(s2);

  if (!strcmp(new_s1, new_s2))
    rval = true;

  delete [] new_s1;
  delete [] new_s2;

  return rval;
}
