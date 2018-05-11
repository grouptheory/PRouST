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
static char const _restart_ind_cc_rcsid_[] =
"$Id: restart_ind.cc,v 1.7 1998/08/06 04:03:19 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/errmsg.h>
#include <codec/uni_ie/restart_ind.h>

ie_restart_ind::ie_restart_ind(restart_indicator cind) : 
  InfoElem(ie_restart_ind_id),
  _restart_ind(cind) { }

ie_restart_ind::ie_restart_ind(const ie_restart_ind & rhs) :
  InfoElem(ie_restart_ind_id), _restart_ind(rhs._restart_ind) { }

ie_restart_ind::~ie_restart_ind() { }


InfoElem* ie_restart_ind::copy(void) const
{ return (new ie_restart_ind(_restart_ind)); }


int ie_restart_ind::equals(const ie_restart_ind *himptr) const
{
  return (himptr->_restart_ind == _restart_ind);
}


int ie_restart_ind::equals(const InfoElem * himptr) const
{
  return (himptr->equals(this));
}


int ie_restart_ind::operator == (const ie_restart_ind & rs) const
{ return (rs._restart_ind == _restart_ind);}
  
int ie_restart_ind::Length()
{
  return (ie_header_len + 1);
}


int ie_restart_ind::encode(u_char * buffer)
{
  u_char * temp;
  int buflen = 0;

  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
    buffer += buflen;
    return(buflen);
  }
  put_id(buffer,_id);	
  put_coding(buffer,_coding);
  temp = buffer + ie_header_len;
  put_8(temp,buflen,(0x80 | _restart_ind)); 
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}

/*
 * This information element has length 5 bytes
 */
InfoElem::ie_status ie_restart_ind::decode(u_char * buffer, int & id)
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
  if ((*buffer & 0x80) != 0x80)
    return (InfoElem::invalid_contents);
  int rc = (*buffer & 0x07);
  switch(rc) {
    case specific_vc:
    case all_vc:
      _restart_ind = (ie_restart_ind::restart_indicator) rc;
      return (InfoElem::ok);
      break;
    default:
      return (InfoElem::invalid_contents);
      break;
  }
  return (InfoElem::ok);
}


enum ie_restart_ind::restart_indicator ie_restart_ind::get_class(void) 
{ return _restart_ind; }

void ie_restart_ind::set_class(restart_indicator cind)
{ 
  _restart_ind = cind;
  MakeStale();
}

ie_restart_ind::ie_restart_ind(void) : InfoElem(ie_restart_ind_id),
  _restart_ind(specific_vc) {}

char ie_restart_ind::PrintSpecific(ostream & os)
{
  os << endl << "   restart indicator::";
  switch (_restart_ind) {
  case specific_vc: os << "specific vc "; break;
  case all_vc:      os << "all vc "; break;
  default:          os << "unknown "; break;
  }
  os << endl;
  return 0;
}

ostream & operator << (ostream & os, ie_restart_ind & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}
