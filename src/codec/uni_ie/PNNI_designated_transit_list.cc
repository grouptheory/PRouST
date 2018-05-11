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
static char const _PNNI_des_xit_list_cc_rcsid_[] =
"$Id: PNNI_designated_transit_list.cc,v 1.18 1998/10/22 13:31:37 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/PNNI_designated_transit_list.h>

PNNI_designated_transit_list::
PNNI_designated_transit_list(int trans_ptr) :
  InfoElem(PNNI_designated_transit_list_id), 
  _transit_pointer(trans_ptr) { }

PNNI_designated_transit_list::
PNNI_designated_transit_list(const PNNI_designated_transit_list & rhs) : 
  InfoElem(PNNI_designated_transit_list_id), 
  _transit_pointer(rhs._transit_pointer)
{
  if (!(rhs._dtl.empty())) { 
    list_item li;
    forall_items(li, rhs._dtl) {
      DTLContainer * cur = rhs._dtl.inf(li);
      _dtl.append(new DTLContainer( *cur ));
    }
  }
}

PNNI_designated_transit_list::~PNNI_designated_transit_list( )
{
  list_item li;
  forall_items(li, _dtl) {
    delete _dtl.inf(li);
  }
  _dtl.clear();
}

PNNI_designated_transit_list * PNNI_designated_transit_list::copy(void)
{ 
  return (new PNNI_designated_transit_list(*this)); 
}

InfoElem * PNNI_designated_transit_list::copy(void) const 
{ 
  return (InfoElem *)new PNNI_designated_transit_list(*this); 
}

int PNNI_designated_transit_list::Length(void)
{
  int buflen =  ie_header_len;
  buflen += 2; // _transit_pointer
  list_item li;
  forall_items(li, _dtl) {
    // 1 for Logical node/port indicator + 22 for nodeid + 4 for port
    buflen += 27;
  }
  return buflen;
}

int PNNI_designated_transit_list::encode(u_char *buffer)
{
  u_char * temp = 0;
  int buflen = 0;
  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
    buffer += buflen;
    return(buflen);
  }
  temp = buffer + ie_header_len;
  put_id(buffer,_id);
  put_coding(buffer,_coding);
  put_16(temp,buflen,_transit_pointer);

  list_item li;
  forall_items(li, _dtl) {
    DTLContainer * cur = _dtl.inf(li);
    put_8(temp, buflen, 1);               // Logical node/port indicator
    for (int i = 0; i < 22; i++)
      put_8(temp, buflen, (cur->GetNID()->GetNID())[i]);
    put_32(temp, buflen, cur->GetPort());
  }
  put_len(buffer, buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return buflen;
}

InfoElem::ie_status PNNI_designated_transit_list::decode(u_char *buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);
  if (id != _id)
    return InfoElem::bad_id;
  if (!len)
    return InfoElem::empty;
  u_char *temp = buffer + ie_header_len;
  get_16(temp, _transit_pointer);
  len -= 2;
  int lp;

  while (len > 26) {
    get_8(temp, lp); // Logical Node/Port indicator
    len--;
    
    u_char nid[22]; int pid;
    for (int i = 0; i < 22; i++)
      get_8(temp, nid[i]);
    len -= 22;
    get_32(temp, pid);
    len -= 4;

    AddToDTL(nid, pid);
  }
  if (len)
    return InfoElem::invalid_contents;
  return InfoElem::ok;
}

int PNNI_designated_transit_list::operator == (const PNNI_designated_transit_list & rhs) const
{
  return (_transit_pointer == rhs._transit_pointer &&
	  EqualDTLs(rhs._dtl));
}

int PNNI_designated_transit_list::equals(const InfoElem * him) const
{  return him->equals(this);  }

int PNNI_designated_transit_list::equals(const PNNI_designated_transit_list * himptr) const
{  return (*this == *himptr);  }

char PNNI_designated_transit_list::PrintSpecific(ostream & os)
{
  os << "     Transit Pointer::" << _transit_pointer << endl;
  list_item li;
  forall_items(li, _dtl) {
    DTLContainer * cur = _dtl.inf(li);
    os << "     Logical Node ID::" << *(cur->GetNID()) << endl;
    os << "     Logical Port ID::" << cur->GetPort() << endl;
  }
  return 0;
}

ostream & operator << (ostream & os, PNNI_designated_transit_list & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}

void PNNI_designated_transit_list::AddToDTL(const u_char * nid, int port)
{  _dtl.append(new DTLContainer(nid, port));  }

void PNNI_designated_transit_list::AddToDTL(const NodeID * nid, int port)
{  _dtl.append(new DTLContainer( nid, port) );  }

void PNNI_designated_transit_list::RemFromDTL(const u_char * nid, int port)
{
  DTLContainer rem(nid, port);

  list_item li;
  forall_items(li, _dtl) {
    DTLContainer * cur = _dtl.inf(li);
    if (rem.equals(cur)) {
      _dtl.del_item(li);
      delete cur;
      break;
    }
  }
}

void PNNI_designated_transit_list::RemFromDTL(const NodeID * nid, int port)
{
  DTLContainer rem( nid, port );

  list_item li;
  forall_items(li, _dtl) {
    DTLContainer * cur = _dtl.inf(li);
    if (rem.equals(cur)) {
      _dtl.del_item(li);
      delete cur;
      break;
    }
  }
}

bool PNNI_designated_transit_list::IsInDTL(const u_char * nid, int port) const
{
  DTLContainer query(nid, port);

  list_item li;
  forall_items(li, _dtl) {
    DTLContainer * cur = _dtl.inf(li);
    if (query.equals(cur))
      return true;
  }
  return false;
}

bool PNNI_designated_transit_list::IsInDTL(const NodeID * nid, int port) const
{
  DTLContainer query( nid, port );

  list_item li;
  forall_items(li, _dtl) {
    DTLContainer * cur = _dtl.inf(li);
    if (query.equals(cur))
      return true;
  }
  return false;
}

bool PNNI_designated_transit_list::EqualDTLs(const list<DTLContainer *> & rhs) const
{
  list_item li;
  forall_items(li, rhs) {
    DTLContainer * cur = rhs.inf(li);
    if (!IsInDTL(cur->GetNID(), cur->GetPort()))
      return false;
  }
  forall_items(li, _dtl) {
    DTLContainer * cur = _dtl.inf(li);
    if (!IsInDTL(cur->GetNID(), cur->GetPort()))
      return false;
  }
  return true;
}

const list<DTLContainer *> & PNNI_designated_transit_list::GetDTL(void) const
{  return _dtl;  }

// --------------------------- DTLContainer ---------------------------------
DTLContainer::DTLContainer(const NodeID * n, int port) 
  : _nid( new NodeID(*n) ), 
    _port(port) 
{
  // The NodeID is still yours, I made a copy
}

DTLContainer::DTLContainer(const u_char * n, int port) 
  : _nid(0), _port(port) 
{  _nid = new NodeID(n);  }

DTLContainer::DTLContainer(const DTLContainer & rhs)
  : _nid(0), _port(rhs._port)
{
 if (rhs._nid) _nid = new NodeID(*rhs._nid);
}

DTLContainer::~DTLContainer( ) { delete _nid; }

// Casting operator to NodeID for ACAC
DTLContainer::operator NodeID * () const
{ return _nid; }

const NodeID * DTLContainer::GetNID(void) const
{ return _nid; }

NodeID * DTLContainer::TakeNID(void)
{ 
  NodeID * rval = _nid;
  _nid = 0x0;
  return _nid; 
}

const int      DTLContainer::GetPort(void) const
{ return _port; }

DTLContainer * DTLContainer::copy(void) const
{ return new DTLContainer( *this ); }

bool DTLContainer::equals(const DTLContainer * rhs) const
{ 
  if (_port == rhs->_port &&
      _nid->equals(rhs->_nid))
    return true; 
  return false; 
}

bool DTLContainer::equals(const DTLContainer & rhs) const
{ return equals(&rhs); }

int DTLContainer ::GetLevel()
{
  int level=0;
  if (_nid) level = _nid->GetLevel();
  return level;
}
//  friends of DTLContainer 
ostream & operator << (ostream & os, const DTLContainer rhs)
{
  return os; // not implemented but required by LEDA
}

int compare ( DTLContainer const &lhs, DTLContainer const  &rhs)
{
  int result;
  // compare the containers - no need to die on dereferencing NULL
  if ( lhs == 0 && rhs == 0) return 0;
  if ( lhs == 0 || rhs == 0) 
    return (&lhs > &rhs ? 1 : &rhs > &lhs ? -1 : 0);

  // it is necessary to compare the contents of the NodeID pointers.
  // do not change this code to compare pointers
  const u_char * lnid = lhs._nid->GetNID();
  const u_char * rnid = rhs._nid->GetNID();
  int len = 16 + (lnid[1]<104 ? lnid[1] : 104);
  result = bitcmp(lnid, rnid, len);

  if (result == 0) 
    result = (lhs._port > rhs._port ? 1 : rhs._port > lhs._port ? -1 : 0);

  return result;
}
