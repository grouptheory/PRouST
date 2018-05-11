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
static char const _broadband_repeat_ind_cc_rcsid_[] =
"$Id: broadband_repeat_ind.cc,v 1.8 1998/08/06 23:19:41 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/broadband_repeat_ind.h>
#include <codec/uni_ie/errmsg.h>

int ie_broadband_repeat_ind::operator ==(const ie_broadband_repeat_ind & bri) const
{
  return equals(&bri);
}

/*
 * When you've seen one broadband repeat indicator, you've seen them all.
 * That is, any instance of the class ie_broadband_repeat_ind is equal
 * to any other instance of the same class.
 */
int ie_broadband_repeat_ind::equals(const ie_broadband_repeat_ind *himptr) const
{
  return 1;
}


int ie_broadband_repeat_ind::equals(const InfoElem * himptr) const
{
  return (himptr->equals(this));
}

ie_broadband_repeat_ind::ie_broadband_repeat_ind(void) : InfoElem(ie_broadband_repeat_ind_id) {}

ie_broadband_repeat_ind::ie_broadband_repeat_ind(const ie_broadband_repeat_ind & rhs) :
  InfoElem(ie_broadband_repeat_ind_id) { }

InfoElem* ie_broadband_repeat_ind::copy(void) const
{
  return new ie_broadband_repeat_ind;
}
  
ie_broadband_repeat_ind::~ie_broadband_repeat_ind() {}


int ie_broadband_repeat_ind::Length()
{
  return(ie_header_len+1);
}

int ie_broadband_repeat_ind::encode(u_char * buffer)
{
  u_char * temp;
  int buflen = 0;

  if ((temp = LookupCache()) != 0L)
    {
      bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
      buffer += buflen;
      return(buflen);
    }
  temp = buffer + ie_header_len;
  put_id(buffer,_id);
  put_coding(buffer,_coding);
  put_8(temp,buflen,(0x80 | prioritized)); 
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return (buflen);
}

/*
 * This information element has total length (4+1) bytes
 */
InfoElem::ie_status  ie_broadband_repeat_ind::decode(u_char * buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);

  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len != 1)
    return (InfoElem::invalid_contents);
  buffer += 4;
  if ((*buffer & 0x80) != 0x80)
    return (InfoElem::invalid_contents);
  int repeat_ind;
  get_8(buffer,repeat_ind);
  repeat_ind &= 0x0f;
  switch(repeat_ind)
    {
    case prioritized:
      return (InfoElem::ok);
      break;
    default:
      return (InfoElem::invalid_contents);
      break;
    }
  return (InfoElem::ok);
}

int ie_broadband_repeat_ind::max_repeats(void) { return 2; }
