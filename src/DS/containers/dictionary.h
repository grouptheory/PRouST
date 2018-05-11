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
#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#ifndef LINT
static char const _dictionary_h_rcsid_[] =
"$Id: dictionary.h,v 1.26 1999/01/26 22:38:34 mountcas Exp $";
#endif

#include <iostream.h>

// #define __USE_RB_TREE__

#ifndef __USE_RB_TREE__
#include <DS/containers/list.h>
#else
#include <DS/containers/rb_tree.h>
#endif

class association {
public:
  
  association(void) 
    : _owner(0), _key(0), _info(0), _sizek(-1), _sizei(-1) { }

  association(void * owner, void * key, void * info, int k, int i) :
    _owner(owner), _key(key), _info(info), _sizek(k), _sizei(i) { }

  association(const association & rhs) :
    _owner(0), _key(0), _info(0), _sizek(rhs._sizek), _sizei(rhs._sizei)
  { 
    _key  = new char [ _sizek ];
    memcpy(_key, rhs._key, _sizek);
    _info = new char [ _sizei ];
    memcpy(_info, rhs._info, _sizei);
  }

  ~association( ) { _owner = 0; }

  void * _owner;
  void * _key;
  void * _info;
  int    _sizek;
  int    _sizei;
};

typedef association * dic_item;

template <class K, class I> class dictionary {
public:

  static int compare(const association * & x, const association * & y);

  dictionary(void);
  dictionary(const dictionary & rhs);
  virtual ~dictionary();

  dictionary & operator = (const dictionary & rhs);

  K   key(dic_item it) const;
  I & inf(dic_item it) const;
  I   access(K & key)  const;

  dic_item insert(K keyi, I info);
  dic_item lookup(K keyi) const;

  void del(K & key);
  void del_item(dic_item it);

  void change_inf(dic_item it, I info);
  void clear(void);

  int  size(void) const;
  int  rank(const K & key) const;

  bool empty(void) const;

  // allows 'forall_items' to work
  dic_item first(void) const;
  dic_item last(void) const;
  dic_item next(dic_item it) const;
  dic_item prev(dic_item it) const;
  dic_item succ(dic_item it) const;
  dic_item pred(dic_item it) const;

  I & operator [] (K & key) const;

  dictionary & operator &= (const dictionary & rhs);
  dictionary & operator += (const dictionary & rhs);

  bool contains(dic_item it) const;

protected:

  void copy(const dictionary & rhs);

#ifndef __USE_RB_TREE__
  list<association *> _list;
#else
  rb_tree<association *> _list;
#endif
};

#endif // __DICTIONARY_H__
