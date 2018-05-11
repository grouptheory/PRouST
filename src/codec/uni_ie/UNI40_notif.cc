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
static char const _UNI40_notif_cc_rcsid_[] =
"$Id: UNI40_notif.cc,v 1.9 1998/08/06 04:03:08 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/UNI40_notif.h>

UNI40_notification_indicator::
UNI40_notification_indicator(const UNI40_notification_indicator & rhs) 
  : InfoElem(UNI40_notification_ind_id) { }  

UNI40_notification_indicator & UNI40_notification_indicator::operator = 
(const UNI40_notification_indicator & rhs)
{ return *this; }

int  UNI40_notification_indicator::operator == (const UNI40_notification_indicator & rhs) const
{
  return 0;
}

int  UNI40_notification_indicator::equals(const UNI40_notification_indicator * rhs) const
{
  return 0;
}

int  UNI40_notification_indicator::equals(const InfoElem * rhs) const
{
  return 0;
}

int UNI40_notification_indicator::Length()
{
  // Body is always NULL.
  return (ie_header_len);
}

int UNI40_notification_indicator::encode(u_char *buffer)
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
  put_len(buffer,buflen);
  buflen += ie_header_len;
  // Body is always NULL.
  FillCache(buffer, buflen);
  return buflen;
}

InfoElem::ie_status UNI40_notification_indicator::decode(u_char *buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);

  if (id != _id)
    return (InfoElem::bad_id);
  // Not supported currently.
  //  if (!len)
  //  return (InfoElem::empty);
  if (len != 0)
    return (InfoElem::incomplete);
  
  return (InfoElem::ok);
}


char UNI40_notification_indicator::PrintSpecific(ostream & os)
{
  return 0;
}

ostream & operator << (ostream & os, UNI40_notification_indicator & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}


