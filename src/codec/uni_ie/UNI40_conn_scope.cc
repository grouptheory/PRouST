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
static char const _UNI40_conn_scope_cc_rcsid_[] =
"$Id: UNI40_conn_scope.cc,v 1.6 1998/08/06 23:14:53 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/UNI40_conn_scope.h>

UNI40_conn_scope_sel & UNI40_conn_scope_sel::operator = (const UNI40_conn_scope_sel & rhs)
{
  _id        = rhs._id;
  _conn_type = rhs._conn_type;
  _conn_sel  = rhs._conn_sel;
  return *this;
}

int UNI40_conn_scope_sel::operator ==(const UNI40_conn_scope_sel & rhs) const
{
  return equals(&rhs);
}

int UNI40_conn_scope_sel::equals(const InfoElem * ieptr) const
{
  return ieptr->equals(this);
}

int UNI40_conn_scope_sel::equals(const UNI40_conn_scope_sel * rhs) const
{
  return ((_conn_type == rhs->_conn_type) && (_conn_sel == rhs->_conn_sel));
}

int UNI40_conn_scope_sel::Length()
{
  return (ie_header_len + 2);
}

int UNI40_conn_scope_sel::encode(u_char * buffer)
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
  put_coding(buffer,0x60);
  put_8(temp,buflen,((_conn_type & 0x0F) | 0x80));
  put_8(temp,buflen,_conn_sel);
  put_len(buffer,buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return buflen;
}

InfoElem::ie_status UNI40_conn_scope_sel::decode(u_char *buffer, int & id)
{
       id = buffer[0];
  short len = get_len(buffer);
  // buffer[1]: bit8 must be 1 and coding must be 11
  if ((buffer[1] & 0xE0) != 0xE0)
    return InfoElem::invalid_contents;
  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);
  if (len < 2)
    return (InfoElem::incomplete);
  if (len != 2)
    return InfoElem::invalid_contents;
  u_char *temp = buffer + ie_header_len;
  get_8tc(temp, _conn_type, enum UNI40_conn_scope_sel::connection_scope_types);
  _conn_type = (enum UNI40_conn_scope_sel::connection_scope_types)
    ((int)_conn_type & 0x0F);
  if (_conn_type != 1)
    return InfoElem::invalid_contents;
  get_8tc(temp, _conn_sel, enum UNI40_conn_scope_sel::connection_scope_sels);
  if (_conn_sel < 0x00 || _conn_sel > 0x0f)
    return InfoElem::invalid_contents;
  return (InfoElem::ok);
}

char UNI40_conn_scope_sel::PrintSpecific(ostream & os)
{
  os << "   Connection Scope Type::";
  switch (_conn_type) {
    case organizational: os << "organizational\n"; break;
    default: os << "reserved\n"; break;
  }
  os << "   Connection Scope Selection::";
  switch (_conn_sel) {
    case reserved:    os << "Reserved\n"; break;
    case local_net:   os << "Local Network\n"; break;
    case local_netp1: os << "Local Network plus one\n"; break;
    case local_netp2: os << "Local Network plus two\n"; break;
    case sitem1:      os << "Site minus one\n"; break;
    case intra_site:  os << "Intra-site\n"; break;
    case sitep1:      os << "Site plus one\n"; break;
    case orgm1:       os << "Organization minus one\n"; break;
    case intra_org:   os << "Intra-organization\n"; break;
    case orgp1:       os << "Organization plus one\n"; break;
    case communitym1: os << "Community minus one\n"; break;
    case regional:    os << "Regional\n"; break;
    case inter_regl:  os << "Inter-regional\n"; break;
    case global:      os << "Global\n"; break;
    default:          os << "Unknown\n"; break;
  }
  os << endl;
  return 0;
}

ostream & operator << (ostream & os, UNI40_conn_scope_sel & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}
