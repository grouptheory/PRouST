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
static char const _end_pt_state_cc_rcsid_[] =
"$Id: end_pt_state.cc,v 1.7 1998/08/06 04:03:17 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/end_pt_state.h>
#include <codec/uni_ie/errmsg.h>

//***************************************************************************
// see 5.4.8.2 Endpoint State, page 240 UNI-3.1 book
// This information element has 1 byte length.
//***************************************************************************
ie_end_pt_state::ie_end_pt_state(party_state state) : 
                        InfoElem(ie_end_pt_state_id),
                        _epr_party_state(state) 
{ }

ie_end_pt_state::ie_end_pt_state(const ie_end_pt_state & rhs) : InfoElem(ie_end_pt_state_id),
  _epr_party_state(rhs._epr_party_state) { }

ie_end_pt_state::~ie_end_pt_state() { }


InfoElem* ie_end_pt_state::copy(void) const
{ return new ie_end_pt_state(_epr_party_state); }

int ie_end_pt_state::equals(const ie_end_pt_state *himptr) const
{
  return (himptr->_epr_party_state == _epr_party_state);
}


int ie_end_pt_state::equals(const InfoElem * himptr) const
{
  return (himptr->equals(this));
}

int ie_end_pt_state::operator == (const ie_end_pt_state & rie) const
    { return (rie._epr_party_state == _epr_party_state); }

enum ie_end_pt_state::party_state  ie_end_pt_state::get_epr_state(void) 
{ return _epr_party_state; }

void ie_end_pt_state::set_epr_state(party_state state) 
{ 
  _epr_party_state = state;
  MakeStale();
}
  
ie_end_pt_state::ie_end_pt_state(void) : InfoElem(ie_end_pt_state_id),
  _epr_party_state(p_null) {}

int ie_end_pt_state::Length()
{
  return (ie_header_len + 1);
}


int ie_end_pt_state::encode(u_char * buffer)
{
  u_char * temp;
  int buflen = 0;

  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
    buffer += buflen;
    return(buflen);
  }
  temp = buffer + ie_header_len;
  put_id(buffer,_id);	
  put_coding(buffer,_coding);
  put_8(temp,buflen,_epr_party_state);
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}

InfoElem::ie_status ie_end_pt_state::decode(u_char * buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);
  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len != 1)
    return (InfoElem::incomplete);
  buffer += ie_header_len;
  int epr_party_state;
  get_8(buffer,epr_party_state);
  switch(epr_party_state) {
    case p_null:
    case add_party_initiated:
    case add_party_received:
    case drop_party_initiated:
    case drop_party_received:
    case p_active:
      _epr_party_state = (party_state )epr_party_state;
      return(InfoElem::ok);
      break;

    default:
      return (InfoElem::invalid_contents);
      break;
  }
  return (InfoElem::ok);
}


char ie_end_pt_state::PrintSpecific(ostream & os)
{
  os << endl << "   party state::";
  switch (_epr_party_state) {
  case p_null: os << "null " << endl; break;
  case add_party_initiated: os << "add party initiated " << endl; break;
  case add_party_received: os << "add party received " << endl; break;
  case drop_party_initiated: os << "drop party initiated " << endl; break;
  case drop_party_received: os << "drop party received " << endl; break;
  case p_active: os << "active " << endl; break;
  default: os << "unknown " << endl; break;
  }
  return 0;
}

ostream & operator << (ostream & os, ie_end_pt_state & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}
