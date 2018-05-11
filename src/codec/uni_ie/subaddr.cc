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
static char const _subaddr_cc_rcsid_[] =
"$Id: subaddr.cc,v 1.9 1998/08/06 23:22:35 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/addr.h>
#include <codec/uni_ie/subaddr.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/errmsg.h>

subaddress::subaddress(ieid  id, Addr *aptr) : 
   InfoElem(id), _subaddr_type(nsap_even), _subaddr_ptr(0) 
{
   if (aptr != 0L) {
     switch (aptr->get_addressing_type()) {
     case Addr::Unknown: // NSAP
       switch (((NSAP_addr *)aptr)->get_afi_type()) {
       case NSAP_addr::DCC_ATM:
       case NSAP_addr::ICD_ATM:
	 _subaddr_ptr = new NSAP_DCC_ICD_addr((NSAP_DCC_ICD_addr *)aptr);
	 break;
       case NSAP_addr::E164_ATM:
	 _subaddr_ptr = new NSAP_E164_addr((NSAP_E164_addr *)aptr);
	 break;
       default:
	 _subaddr_ptr = 0L;
	 break;
       }
       break;
     case Addr::International: // E164
       _subaddr_ptr = new E164_addr((E164_addr *)aptr);
       break;
     default:
       // Error!
       _subaddr_ptr = 0L;
       break;
     }
   }
}

subaddress::subaddress(const subaddress &him) : InfoElem(him),
  _subaddr_type(him._subaddr_type), _subaddr_ptr(0)
{
  if (him._subaddr_ptr)
    _subaddr_ptr = him._subaddr_ptr->copy(); 
}

subaddress::subaddress(ieid id) :
  InfoElem(id),_subaddr_ptr(0L),
  _subaddr_type(nsap_even) {}


subaddress::~subaddress(void) 
{ 
  delete _subaddr_ptr;
  _subaddr_ptr=0L;
}

enum subaddress::subaddr_type subaddress::get_subaddr_type(void) 
{return _subaddr_type;}

void subaddress::set_subaddr_type(subaddr_type atype) 
{ 
  _subaddr_type = atype;
  MakeStale();
}

int subaddress::operator == (const subaddress & sub) const
{
  if ((_subaddr_type == sub._subaddr_type) &&
      (_subaddr_ptr == _subaddr_ptr))
    return 1;
  else
    return 0;
}

u_char subaddress::IsVariableLen(void) { return ((u_char)1); }

InfoElem::ie_status subaddress::decode(u_char *buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);

  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len > 21)
    return (InfoElem::invalid_contents);
  if (len != 21)
    return (InfoElem::incomplete);
  buffer += ie_header_len;
  int temp = 0xf8 & (*buffer++);
  if ((temp != nsap_even) && (temp != nsap_odd))
    return (InfoElem::invalid_contents);
  _subaddr_type = (subaddress::subaddr_type) temp;

  // what type of NSAP address do we have
  u_char c = *buffer;
  switch(c) {
    case NSAP_addr::DCC_ATM:
    case NSAP_addr::ICD_ATM:
      _subaddr_ptr = new NSAP_DCC_ICD_addr((NSAP_addr::afi_type) c);
      break;
    case NSAP_addr::E164_ATM:
      _subaddr_ptr = new NSAP_E164_addr((NSAP_addr::afi_type) c);
      break;
    default:
      return (InfoElem::invalid_contents);
      break;
  }
  int nid;
  return (_subaddr_ptr->decode(buffer,nid));
}

int subaddress::Length()
{


  if(LookupCache())
    return(CacheLength());
  int buflen = ie_header_len;
  buflen++; //  _subaddr_type
  if (_subaddr_ptr) // sanity check
    buflen += _subaddr_ptr->Length();
  return buflen;
}

int subaddress::encode(u_char *buffer)
{
  u_char * temp = 0L;
  int buflen = 0;

  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
    buffer += buflen;
    return(buflen);
  }
  temp = buffer + ie_header_len;
  put_id(buffer,_id);	
  put_coding(buffer,_coding);
  put_8(temp,buflen,_subaddr_type);
  if (_subaddr_ptr) // sanity check
    buflen += _subaddr_ptr->encode(temp);
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return buflen;
}

ostream & operator << (ostream & os, subaddress & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}

char subaddress::PrintSpecific(ostream & os)
{
  os << endl << "   subaddress type::";
  switch (_subaddr_type) {
  case nsap_even: os << "NSAP even " << endl; break;
  case nsap_odd:  os << "NSAP odd " << endl; break;
  case atm_even:  os << "ATM even " << endl; break;
  case atm_odd:   os << "ATM odd " << endl; break;
  default:        os << "unknown " << endl; break;
  }
  os << "   subaddress::" << *(_subaddr_ptr) << endl;
    
  return 0;
}

