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
static char const _pnni_pkt_cc_rcsid_[] =
"$Id: pnni_pkt.cc,v 1.13 1999/01/06 18:45:22 bilal Exp $";
#endif
#include <common/cprototypes.h>
#include <FW/basics/diag.h>

#define __PNNI_PKT_CC__
#include <codec/pnni_pkt/pnni_pkt.h>

const int PNNIPkt::pkt_header_len = 8;

PNNIPkt::PNNIPkt(PNNIPkt::PktTypes type, PNNIPkt::versions newest, PNNIPkt::versions oldest) :
  _type(type), _new_ver(newest), _old_ver(oldest) { }

PNNIPkt::~PNNIPkt() {}

bool PNNIPkt::PrintGeneral(ostream & os) const
{
  switch (_type) {
    case PNNIPkt::hello:
      os << "!  Hello Packet:" << endl;
      break;
    case PNNIPkt::ptsp:
      os << "!  PTSP Packet:" << endl;
      break;
    case PNNIPkt::ptse_ack:
      os << "!  PTSE Acnowledgement Packet:" << endl;
      break;
    case PNNIPkt::database_sum:
      os << "!  Database Summary:" << endl;
      break;
    case PNNIPkt::ptse_req:
      os << "!  PTSE Requests:" << endl;
      break;
  }
  return true;
}

const char * PNNIPkt::printType(void) const
{
  switch (_type) {
    case PNNIPkt::hello:
      return "Hello"; break;
    case PNNIPkt::ptsp:
      return "PTSP"; break;
    case PNNIPkt::ptse_ack:
      return "PTSE Ack"; break;
    case PNNIPkt::database_sum:
      return "Database Summary"; break;
    case PNNIPkt::ptse_req:
      return "PTSE Req"; break;
  }
  return 0;
}

void PNNIPkt::encode_pkt_header(u_char * buffer, int & len  , PNNIPkt::versions ver)
{
  DIAG("codec.debugging", DIAG_DEBUG, 
       cout << "PNNIPkt::encode_pkt_header " << printType() << " -- len = " << len << endl; );

  len += 8; // Which accounts for 4 bytes in pkt header  sans type & length
  buffer[0] = (_type >> 8) & 0xFF;
  buffer[1] = (_type & 0xFF);
  buffer[2] = (len >> 8) & 0xFF;
  buffer[3] = (len & 0xFF);
  buffer[4] = ver;
  buffer[5] = _new_ver;
  buffer[6] = _old_ver;
  buffer[7] = 0;  // Reserved
}

errmsg * PNNIPkt::decode_pkt_header(const u_char * buffer)
{
  errmsg * rval = 0;

  _length  = (((buffer[2] << 8) & 0xFF00 ) | (buffer[3] & 0xFF));
  _protocol_ver      = buffer[4];
  _new_ver = buffer[5];
  _old_ver = buffer[6];

  return rval;
}


void PktHexDump(const u_char * buffer, int len, ostream & os)
{
  const u_char * buf = buffer;

  //  if (prefix) 
  //  os << prefix;
  if (buf) {
    for (int i = 0; i < len; i++) {
      int val = (int)(buf[i]);
      if (val < 16) 
	os << "0";
      os << hex << val;
      if ((i   ) == 1) os << " ";
    }
  } else
    os << "Invalid message" << endl;
  os << dec << endl;
}
