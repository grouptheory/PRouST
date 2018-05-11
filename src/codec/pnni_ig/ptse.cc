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
static char const _ptse_cc_rcsid_[] =
"$Id: ptse.cc,v 1.96 1999/01/14 21:40:10 battou Exp $";
#endif

#include <common/cprototypes.h>
#include <FW/basics/diag.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/nodal_state_params.h>
#include <codec/pnni_ig/nodal_info_group.h>
#include <codec/pnni_ig/internal_reachable_atm_addr.h>
#include <codec/pnni_ig/external_reachable_atm_addr.h>
#include <codec/pnni_ig/horizontal_links.h>
#include <codec/pnni_ig/uplinks.h>
#include <codec/pnni_ig/system_capabilities.h>

long ig_ptse::_glob_ptses = 0;
list<long> * ig_ptse::_ptsesList = 0;

ig_ptse::ig_ptse(InfoGroup::ig_id type, int id, int sequence_number, 
		 int checksum, short int remaining_lifetime) : 
   InfoGroup(InfoGroup::ig_ptse_id), _type(type), _identifier(id), 
   _seq_num(sequence_number), _checksum(checksum), 
   _remaining_lifetime(remaining_lifetime), _ref_count(1),
   _my_death_row_entry(0), _death_row(0), _members(0)
{ 
  // The identifier 1 is reserved for the nodal ig
  //  assert( (_identifier > 1) || (_type == InfoGroup::ig_nodal_info_group_id) );

  _glob_ptses++;
  if (_ptsesList == 0)
    _ptsesList = new list<long> ;
  assert( _ptsesList->lookup((long)this) == 0 );
  _ptsesList->append((long)this);
}

ig_ptse::ig_ptse(const ig_ptse & rhs) : 
  InfoGroup(InfoGroup::ig_ptse_id), _type(rhs._type), 
  _identifier(rhs._identifier), _ref_count(1),
  _seq_num(rhs._seq_num), _checksum(rhs._checksum), 
  _remaining_lifetime(rhs._remaining_lifetime), 
  _my_death_row_entry(0), _death_row(0),
  _members(0)
{ 
  // Copy over all the members as well.
  if (rhs._members != 0 &&
      (rhs._members)->empty() == false) {
    _members = new list<InfoGroup *>;
    list_item li;
    forall_items(li, *(rhs._members)) {
      InfoGroup * ptr = rhs._members->inf(li);
      assert(ptr);
      _members->append( ptr->copy() );
    }
  }

  _glob_ptses++;
  assert( _ptsesList->lookup((long)this) == 0 );
  _ptsesList->append((long)this);
}

ig_ptse::~ig_ptse() 
{
  assert( _ref_count == 0 );

  if (_members) {
    while (_members->empty() == false) {
      list_item li = _members->first();
      delete _members->inf(li);
      _members->del_item(li);
    }
    _members->clear();
    delete _members;
    _members = 0;
  }

  if (_my_death_row_entry &&
      _death_row->empty() == false &&
      _death_row->contains( _my_death_row_entry )) {
    _death_row->del_item(_my_death_row_entry);
    _death_row = 0;
    _my_death_row_entry = 0;
  }

  _type = ig_unknown_id;
  _identifier = 0;
  _seq_num = 0;
  _checksum = 0;
  _remaining_lifetime = 0;

  assert(--_glob_ptses >= 0);
  list_item li;
  assert( (li = _ptsesList->lookup((long)this)) != 0 );
  _ptsesList->del_item(li);
  if ( _glob_ptses == 0 ) {
    delete _ptsesList;
    _ptsesList = 0;
  }
}

InfoGroup * ig_ptse::copy(void)
{
  return new ig_ptse(*this);
}

ig_ptse * ig_ptse::copy_husk(void)    
{ 
  // short int type, int id, int seq, int check, short int rem
  ig_ptse * rval = new ig_ptse(_type, _identifier, _seq_num, 
			       _checksum, _remaining_lifetime);
  return rval;
}

ig_ptse * ig_ptse::Reference(void) 
{ 
  _ref_count++; 

  // This will do list validity checking
  if (_members)
    _members->empty();

  return this; 
}

void ig_ptse::UnReference(void) 
{ 
  // This will do list validity checking
  if (_members)
    _members->empty();

  if (! --_ref_count)
    delete this;
  else
    assert(_ref_count > 0);
}

const int ig_ptse::GetRefs(void) 
{ 
  // This will do list validity checking
  if (_members)
    _members->empty();

  return _ref_count; 
}

void ig_ptse::size(int & length)
{
  length += PTSE_HEADER; // the ptse header 20
  list_item li;
  if (_members && _members->empty() == false) {
    forall_items(li, *_members) {
      (_members->inf(li))->size(length);
    }
  }
}

u_char * ig_ptse::encode(u_char * buffer, int & buflen)
{
  buflen = 0;
  u_char * temp = buffer + ig_header_len;

  *temp++ = (_type >> 8) & 0xFF;
  *temp++ = (_type & 0xFF);

  // Reserved
  *temp++;
  *temp++;

  *temp++ = (_identifier >> 24) & 0xFF;
  *temp++ = (_identifier >> 16) & 0xFF;
  *temp++ = (_identifier >>  8) & 0xFF;
  *temp++ = (_identifier & 0xFF);

  *temp++ = (_seq_num >> 24) & 0xFF;
  *temp++ = (_seq_num >> 16) & 0xFF;
  *temp++ = (_seq_num >>  8) & 0xFF;
  *temp++ = (_seq_num & 0xFF);

  *temp++ = (_checksum >> 8) & 0xFF;
  *temp++ = (_checksum & 0xFF);

  *temp++ = (_remaining_lifetime >> 8) & 0xFF;
  *temp++ = (_remaining_lifetime & 0xFF);

  buflen  += 16;

  if (_members && _members->empty() == false) {
    list_item li;
    forall_items(li, *_members) {
      InfoGroup * ptr = _members->inf(li);
      int tlen = 0;
      temp = ptr->encode(temp, tlen);
      buflen += tlen;
    }
  }

  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_ptse::decode(u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF));
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (type != InfoGroup::ig_ptse_id)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);

  u_char * temp = &buffer[4];

  int tmp  = (*temp++ << 8) & 0xFF00;
      tmp |= (*temp++ & 0xFF);
      
  _type = (InfoGroup::ig_id)tmp;
  len -= 2;

  temp += 2; // Reserved case
  len -= 2;

  _identifier  = (*temp++ << 24) & 0xFF000000;
  _identifier |= (*temp++ << 16) & 0x00FF0000;
  _identifier |= (*temp++ <<  8) & 0x0000FF00;
  _identifier |= (*temp++ & 0xFF);
  len -= 4;

  _seq_num  = (*temp++ << 24) & 0xFF000000;
  _seq_num |= (*temp++ << 16) & 0x00FF0000;
  _seq_num |= (*temp++ <<  8) & 0x0000FF00;
  _seq_num |= (*temp++ & 0xFF);
  len -= 4;

  _checksum  = (*temp++ << 8) & 0xFF00;
  _checksum |= (*temp++ & 0xFF);
  len -= 2;

  _remaining_lifetime  = (*temp++ << 8) & 0xFF00;
  _remaining_lifetime |= (*temp++ & 0xFF);
  len -= 2;

  errmsg * rval = 0;
  InfoGroup * ig = 0;

  while (len > 0) {
     int type = (((temp[0] << 8) & 0xFF00) | (temp[1] & 0xFF));
     int tlen = (((temp[2] << 8) & 0xFF00) | (temp[3] & 0xFF));

     switch (type) {
       case InfoGroup::ig_nodal_state_params_id :
         ig = (InfoGroup *)(new ig_nodal_state_params);
         rval = ig->decode(temp);
         break;
       case InfoGroup::ig_nodal_info_group_id :
         ig = (InfoGroup *)(new ig_nodal_info_group);
         rval = ig->decode(temp);
         break;
       case InfoGroup::ig_internal_reachable_atm_addr_id :
         ig = (InfoGroup *)(new ig_internal_reachable_atm_addr);
         rval = ig->decode(temp);
         break;
       case InfoGroup::ig_exterior_reachable_atm_addr_id :
         ig = (InfoGroup *)(new ig_external_reachable_atm_addr);
         rval = ig->decode(temp);
         break;
       case InfoGroup::ig_horizontal_links_id :
         ig = (InfoGroup *)(new ig_horizontal_links);
         rval = ig->decode(temp);
         break;
       case InfoGroup::ig_uplinks_id :
         ig  = (InfoGroup *)new ig_uplinks;
         rval = ig->decode(temp);
         break;
       case InfoGroup::ig_system_capabilities_id :
         ig = (InfoGroup *)new ig_system_capabilities;
         rval = ig->decode(temp);
         break;
       default :
         return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);
     }

     if (!_members)
       _members = new list<InfoGroup *>;
     _members->append(ig);

     if (rval) return rval;

     len  -= tlen + 4;
     temp += tlen + 4;

  }
   return 0;
}

bool ig_ptse::PrintSpecific(ostream & os)
{
  os << "     Type::" << _type << endl;
  os << "     Identifier::" << _identifier << endl;
  os << "     Sequence Number::" << _seq_num << endl;
  os << "     Checksum::" << _checksum << endl;
  os << "     Remaining Lifetime::" << _remaining_lifetime << endl;

  if (_members && _members->empty() == false) {
    list_item li;
    forall_items(li, *_members) {
      InfoGroup * ig = _members->inf(li);
      os << *ig << endl;
    }
  }
  return true;
}

bool ig_ptse::ReadSpecific(istream & os)
{
  return false;
}

ostream & operator << (ostream & os, ig_ptse & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return (os);
}

istream & operator >> (istream & is, ig_ptse & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return is;
}

int operator == (ig_ptse & lhs, ig_ptse & rhs)
{
  return (lhs._type == rhs._type &&
          lhs._identifier == rhs._identifier &&
          lhs._seq_num == rhs._seq_num &&
          (((lhs._remaining_lifetime == DBKey::ExpiredAge) &&
           (rhs._remaining_lifetime == DBKey::ExpiredAge)) ||
           ((lhs._remaining_lifetime != DBKey::ExpiredAge) &&
           (rhs._remaining_lifetime != DBKey::ExpiredAge))));
}

int ig_ptse::equals(ig_ptse * rhs) const { return equals(*(rhs)); }

int ig_ptse::equals(ig_ptse & rhs) const
{
  return (_type == rhs._type && _identifier == rhs._identifier &&
	  _seq_num == rhs._seq_num && _checksum == rhs._checksum &&
	  _remaining_lifetime == rhs._remaining_lifetime); 
}

const short int  ig_ptse::GetTTL(void)  const 
{ 
  // This will do list validity checking
  if (_members)
    _members->empty();

  return _remaining_lifetime; 
}

const short int  ig_ptse::GetCS(void)   const 
{ 
  // This will do list validity checking
  if (_members)
    _members->empty();

  return _checksum; 
}

const       int  ig_ptse::GetSN(void)   const 
{ 
  // This will do list validity checking
  if (_members)
    _members->empty();

  return _seq_num; 
}

const       int  ig_ptse::GetID(void)   const 
{ 
  // This will do list validity checking
  if (_members)
    _members->empty();

  return _identifier; 
}

InfoGroup::ig_id ig_ptse::GetType(void) const 
{ 
  // This will do list validity checking
  if (_members)
    _members->empty();

  return _type; 
}

void  ig_ptse::SetTTL(short int t) 
{ 
  _remaining_lifetime = t; 

  // This will do list validity checking
  if (_members)
    _members->empty();
}

void  ig_ptse::SetCS(short int c) 
{ 
  _checksum = c;
  
  // This will do list validity checking
  if (_members)
    _members->empty();
}

void  ig_ptse::SetSN(int sn) 
{ 
  _seq_num = sn; 

  // This will do list validity checking
  if (_members)
    _members->empty();
}

void  ig_ptse::SetID(int id) 
{ 
  _identifier = id; 

  // This will do list validity checking
  if (_members)
    _members->empty();
}

void  ig_ptse::SetType(InfoGroup::ig_id t) 
{ 
  _type = t; 

  // This will do list validity checking
  if (_members)
    _members->empty();
}
				
bool  ig_ptse::AddIG(InfoGroup * p)            
{
  if (!_members)
    _members = new list<InfoGroup *>;

  if (!_members->search(p)) {
    _members->append(p); 
    return true;
  }
  return false;
}

const list<InfoGroup *> * ig_ptse::ShareMembers(void) const
{
  // This will do list validity checking
  if (_members)
    _members->empty();

  return _members;
}

void ig_ptse::RemMembers(void) 
{ 
  if (_members && _members->empty() == false) {
    list_item li;
    InfoGroup * ig;
    forall_items(li, *_members) {
      if (ig = _members->inf(li))
	delete ig;
    }
    _members->clear(); 
    delete _members;
    _members = 0;
  }
}

bool ig_ptse::Contains(const InfoGroup * ig) const
{
  bool rval = false;

  if (_members && _members->empty() == false) {
    list_item li;
    forall_items(li, *_members) {
      InfoGroup * tmp = _members->inf(li);
      if (tmp->equals(ig)) {
	rval = true;
	break;
      }
    }
  }
  return rval;
}

int ig_ptse::equals(const InfoGroup * other) const
{
  // This will do list validity checking
  if (_members) _members->empty();

  int rval = InfoGroup::equals(other);
  if (!rval) {
    ig_ptse * rhs = (ig_ptse *)other;
    if ((_type < rhs->_type) ||
	((_type == rhs->_type) && (_identifier < rhs->_identifier)) ||
	((_type == rhs->_type) && (_identifier == rhs->_identifier) && (_seq_num < rhs->_seq_num)) ||
	((_type == rhs->_type) && (_identifier == rhs->_identifier) && (_seq_num == rhs->_seq_num) &&
	 (_checksum < rhs->_checksum)) ||
	((_type == rhs->_type) && (_identifier == rhs->_identifier) && (_seq_num == rhs->_seq_num) &&
	 (_checksum == rhs->_checksum) && (_remaining_lifetime < rhs->_remaining_lifetime)))
      return -1;
    else if ((_type > rhs->_type) ||
	((_type == rhs->_type) && (_identifier > rhs->_identifier)) ||
	((_type == rhs->_type) && (_identifier == rhs->_identifier) && (_seq_num > rhs->_seq_num)) ||
	((_type == rhs->_type) && (_identifier == rhs->_identifier) && (_seq_num == rhs->_seq_num) &&
	 (_checksum > rhs->_checksum)) ||
	((_type == rhs->_type) && (_identifier == rhs->_identifier) && (_seq_num == rhs->_seq_num) &&
	 (_checksum == rhs->_checksum) && (_remaining_lifetime > rhs->_remaining_lifetime)))
      return 1;
  }
  return rval;
}

void ig_ptse::Sentence(seq_item li, sortseq<DBKey*,ig_ptse*>* death_row)
{
  // This will do list validity checking
  if (_members) _members->empty();

  _my_death_row_entry = li;
  _death_row = death_row;
  // Added  aug 2 -Sandeep below
  // _ref_count = 0;
  // Added aug 2 -Sandeep above
}

int operator > (const ig_ptse & lhs, const ig_ptse & rhs)
{
  // This will do list validity checking
  if (lhs._members) lhs._members->empty();
  if (rhs._members) rhs._members->empty();

  return (lhs._type == rhs._type &&
	  lhs._identifier == rhs._identifier &&
	  ((lhs._seq_num > rhs._seq_num) ||
	   ((lhs._seq_num == rhs._seq_num) &&
	    ((lhs._remaining_lifetime == DBKey::ExpiredAge) &&
	     (rhs._remaining_lifetime != DBKey::ExpiredAge)))));
}
