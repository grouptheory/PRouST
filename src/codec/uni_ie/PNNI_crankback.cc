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
static char const _PNNI_crankback_cc_rcsid_[] =
"$Id: PNNI_crankback.cc,v 1.24 1998/08/07 19:59:26 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/id.h>
#include <codec/uni_ie/PNNI_crankback.h>

PNNI_crankback::PNNI_crankback(void)
  : InfoElem(PNNI_crankback_id), _level(0), 
    _blocked_transit_type(SucceedingEndOfInterface),
    _blocked_transit_id(0), _cause(TemporaryFailure), 
    _direction(forward), _port_id(0), 
    _avcr(0), _crm(0), _vf(0) 
{
}


PNNI_crankback::PNNI_crankback(u_char   level, 
			       BlockedTransitTypes blocked_transit_type, 
			       CrankbackCauses cause,
			       const NodeID *blocked_node,
			       u_int blocked_port,
			       const NodeID *blocked_end_node)

  : InfoElem(PNNI_crankback_id), _level(level), 
    _blocked_transit_type(blocked_transit_type),
    _blocked_transit_id(0), _cause(cause), _direction(forward), _port_id(0), 
    _avcr(0), _crm(0), _vf(0) 
{
  int len = (_blocked_transit_type == SucceedingEndOfInterface ? 0 : 
	     _blocked_transit_type == BlockedNode ? 22 : 48);

  //
  // SucceedingEndOfInerface must have no additional args.
  //
  // BlockedNode has only a NodeID * (the blocked node).
  //
  // BlockedLink has two NodeID* (the blocked start and end nodes) and 
  // an int (the blocked port).
  //
  assert(((_blocked_transit_type == SucceedingEndOfInterface) &&
	  (blocked_node == 0 && blocked_port == 0 && blocked_end_node == 0)) ||
	 ((_blocked_transit_type == BlockedNode) &&
	  (blocked_node != 0 && blocked_port == 0 && blocked_end_node == 0)) ||
	 ((_blocked_transit_type == BlockedLink) &&
	  (blocked_node != 0 && blocked_end_node != 0)));

  if (len) {
    _blocked_transit_id = new u_char [len];
    if (blocked_node != 0) 
      bcopy((u_char *)(blocked_node->GetNID()), _blocked_transit_id, 22);

    if (blocked_end_node != 0) {
      bcopy((u_char *)(blocked_end_node->GetNID()), 
	    _blocked_transit_id + 26, 22);

      //
      // Copy the port number into the array in a machine-independent
      // and word-alignment independent manner.
      //
      u_char *port = _blocked_transit_id + 22;
      port[0] =  blocked_port >> 24;
      port[1] = (blocked_port >> 16) & 0xFF;
      port[2] = (blocked_port >>  8) & 0xFF;
      port[3] =  blocked_port & 0xFF;
    }
  } 
}


PNNI_crankback::PNNI_crankback(const PNNI_crankback & rhs) 
  : InfoElem(rhs), _level(rhs._level), 
    _blocked_transit_type(rhs._blocked_transit_type), 
    _blocked_transit_id(0), _cause(rhs._cause), 
    _direction(rhs._direction), _port_id(rhs._port_id), _avcr(rhs._avcr), 
    _crm(rhs._crm), _vf(rhs._vf)
{
  int len = (_blocked_transit_type == SucceedingEndOfInterface ? 0 : 
	     _blocked_transit_type == BlockedNode ? 22 : 48);

  if (len && rhs._blocked_transit_id) {
    _blocked_transit_id = new u_char [len];
    bcopy(rhs._blocked_transit_id, _blocked_transit_id, len);
  }
#if 0 // Taken care of in the initialization phase
  if (_cause == UserCellRateNotAvailable) {
    _direction = rhs._direction;
    _port_id = rhs._port_id;
    _avcr = rhs._avcr;
    _crm = rhs._crm;
    _vf = rhs._vf;
  }
#endif
}

PNNI_crankback::~PNNI_crankback( )
{
  if (_blocked_transit_id) delete [] _blocked_transit_id; 
}

int PNNI_crankback::Length()
{
  int buflen = ie_header_len;
  buflen += 2; // for _level and _blocked_transit_type
  int j = (_blocked_transit_type == SucceedingEndOfInterface ? 0 : 
	   _blocked_transit_type == BlockedNode ? 22 : 48);
  if (j && _blocked_transit_id)
    buflen += j;
  buflen++; // cause
  if (_cause == UserCellRateNotAvailable)
    buflen += 17; // 1 for direction and 16 for port,avcr,crm and vf
  else
    if (_cause == QualityOfServiceUnavailable)
      buflen++; // just the port_id
  return buflen;
}

int PNNI_crankback::encode(u_char * buffer)
{
  u_char * temp = 0;
  int buflen = 0;

  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (buflen = CacheLength()));
    buffer += buflen;
    return (buflen);
  }
  temp = buffer + ie_header_len;
  put_id(buffer, _id);
  put_coding(buffer, _coding);
  put_8(temp, buflen, _level);
  put_8(temp, buflen, _blocked_transit_type);
  int j = (_blocked_transit_type == SucceedingEndOfInterface ? 0 : 
	   _blocked_transit_type == BlockedNode ? 22 : 48);
  if (j && _blocked_transit_id) {
    for (int i = 0; i < j; i++)
      put_8(temp, buflen, _blocked_transit_id[i]);
  }
  put_8(temp, buflen, _cause);
  if (_cause == UserCellRateNotAvailable) {
    put_8(temp,  buflen, _direction);
    put_32(temp, buflen, _port_id);
    put_32(temp, buflen, _avcr);
    put_32(temp, buflen, _crm);
    put_32(temp, buflen, _vf);
  } else if (_cause == QualityOfServiceUnavailable)
    put_8(temp, buflen, (_port_id & 0x0F));	/* _port_id serves a dual 
						 * purpose
						 */
  put_len(buffer, buflen);
  buflen += ie_header_len;
  FillCache(buffer, buflen);
  return buflen;
}

InfoElem::ie_status PNNI_crankback::decode(u_char *buffer, int & id)
{
  id = buffer[0];
  short len = get_len(buffer);
  int i;
  
  if (id != _id)
    return (InfoElem::bad_id);
  if (!len)
    return (InfoElem::empty);

  u_char *temp = buffer + ie_header_len;
  get_8(temp, _level);
  get_8tc(temp, _blocked_transit_type, BlockedTransitTypes);
  len -= 2;
  int j = (_blocked_transit_type == SucceedingEndOfInterface ? 0 : 
	   _blocked_transit_type == BlockedNode ? 22 : 48);
  if (len <= 0)
    return (InfoElem::invalid_contents);
  if (j) {
    if (_blocked_transit_id) delete [] _blocked_transit_id;
    _blocked_transit_id = new u_char [j];
    for (i = 0; i < j; i++)
      get_8(temp, _blocked_transit_id[i]);
    len -= j;
  }
  if (len <= 0)
    return (InfoElem::invalid_contents);

  get_8tc(temp, _cause, CrankbackCauses);
  len--;
  if (len <= 0)
    return (InfoElem::invalid_contents);
  switch (_cause) {
    case UserCellRateNotAvailable: {
      get_8tc(temp, _direction, Dir);
      get_32(temp,_port_id);
      get_32(temp,_avcr);
      get_32(temp,_crm);
      get_32(temp,_vf);
      len -= 17;
      break;
    }
    case QualityOfServiceUnavailable: {
      get_8(temp, _port_id);   // _port_id serves a dual purpose
      len--;
      break;
    }
    default:
      break;
  }
  if (len)
    return (InfoElem::invalid_contents);
  return (InfoElem::ok);
}

int PNNI_crankback::operator == (const PNNI_crankback & rhs) const
{
  int btdlen = (_blocked_transit_type == SucceedingEndOfInterface ? 0 : 
		_blocked_transit_type == BlockedNode ? 22 : 48);
  if (btdlen && memcmp(_blocked_transit_id, rhs._blocked_transit_id, btdlen))
    return 0;

  return (_level == rhs._level &&
	  _blocked_transit_type == rhs._blocked_transit_type &&
	  _cause == rhs._cause && _port_id == rhs._port_id &&
	  _avcr == rhs._avcr  && _crm == rhs._crm && _vf == rhs._vf);
}

int PNNI_crankback::equals(const InfoElem * him) const
{
  return (him->equals(this));
}

int PNNI_crankback::equals(const PNNI_crankback * himptr) const
{
  return (*this == *himptr);
}

char PNNI_crankback::PrintSpecific(ostream & os)
{
  int len = (_blocked_transit_type == 2 ? 
	     0 : _blocked_transit_type == 3 ? 22 : 48);

  os << "    Level::";
  os.setf(ios::hex);
  os << (int)_level << endl;
  os.setf(ios::dec);
  os << "    Blocked Transit Type::" << _blocked_transit_type << endl;
  os << "    Blocked Transit ID::";
  special_print(_blocked_transit_id, len, os);
  os << "    Crankback cause::" << _cause << endl;
  if (_cause == UserCellRateNotAvailable) {
    os << "     Direction::" << _direction << endl;
    os << "     Port ID::" << _port_id << endl;
    os << "     AvCR::" << _avcr << endl;
    os << "     CRM::" << _crm << endl;
    os << "     VF::" << _vf << endl;
  } else if (_cause == QualityOfServiceUnavailable) {
    os << ((_port_id & 0x08) ? 
	   "     CTD unavailable" : "     CTD available") << endl;
    os << ((_port_id & 0x04) ? 
	   "     CDV unavailable" : "     CDV available") << endl;
    os << ((_port_id & 0x02) ? 
	   "     CLR unavailable" : "     CLR available") << endl;
    os << ((_port_id & 0x01) ? "     Other QoS paramters unavailable" 
	   : "     Other QoS paramters available") << endl;
  }
  return 0;
}

ostream & operator << (ostream & os, PNNI_crankback & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return os;
}

PNNI_crankback * PNNI_crankback::copy(void) 
{ 
  return (new PNNI_crankback(*this)); 
}

InfoElem * PNNI_crankback::copy(void) const 
{
  return (InfoElem *)new PNNI_crankback(*this); 
}

void PNNI_crankback::SetCauseDiagnostic(Dir direction, int port_id, int AvCR, 
					int CRM, int VF)
{
  _direction = direction;
  _port_id = port_id;
  _avcr = AvCR;
  _crm = CRM;
  _vf = VF;
}

void PNNI_crankback::SetCauseDiagnostic(int CTD, int CDV, int CLR, int QoS)
{
  if (CTD) CTD = 0x08;
  if (CDV) CDV = 0x04;
  if (CLR) CLR = 0x02;
  if (QoS) QoS = 0x01;

  _port_id = ((CTD | CDV | CLR | QoS) & 0x0F);
}

PNNI_crankback::BlockedTransitTypes 
PNNI_crankback::GetBlockedTransitType(void) const
{  return _blocked_transit_type;  }

PNNI_crankback::CrankbackCauses PNNI_crankback::GetCrankbackCause(void) const
{  return _cause;  }

const u_char      * PNNI_crankback::GetBlockedTransitID(void) const
{  return _blocked_transit_id;  }

const u_char        PNNI_crankback::GetBlockedLevel(void) const
{  return _level;  }

  /** Returns a new NodeID that must be deleted. */
NodeID * PNNI_crankback::GetBlockedNode(void)
{
  NodeID *answer = 0;

  if (_blocked_transit_type != SucceedingEndOfInterface) {
    answer = new NodeID(_blocked_transit_id);
  }

  return answer;
}

  /** Returns a new NodeID that must be deleted. */
NodeID * PNNI_crankback::GetStartNode(void)
{
  return GetBlockedNode();
}

  /** Returns a new NodeID that must be deleted. */
NodeID * PNNI_crankback::GetEndNode(void)
{
  NodeID *answer = 0;

  if (_blocked_transit_type == BlockedLink) {
    answer = new NodeID(_blocked_transit_id + 26);
  }

  return answer;
}

u_int PNNI_crankback::GetBlockedPort(void)
{
  u_int answer = 0;

  if (_blocked_transit_type == BlockedLink) {
    u_char *p = _blocked_transit_id + 22;

    answer = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
  }

  return answer;
}
