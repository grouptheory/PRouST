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

#ifndef LINT
static char const _xti_adapter_cc_rcsid_[] =
"$Id: xti_adapter.cc,v 1.31 1999/03/05 19:25:26 battou Exp $";
#endif
#include <iostream.h>
#include <FW/basics/diag.h>
#include <codec/pnni_pkt/pkt_incl.h>
#include <codec/pnni_ig/ig_incl.h>
#include <fsm/hello/HelloVisitor.h>
#include <fsm/nodepeer/NPVisitors.h>

#include <FW/actors/State.h>

extern "C" {
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include <sys/stream.h>
#include <tiuser.h>

#include <fore_xti/xti_user_types.h>
#include <fore_xti/xti.h>
#include <fore_xti/xti_atm.h>
#include <fore_xti/ans.h>
}

#include "xti_adapter.h"

#define GET_TIME(x)     (gettimeofday(&x, (struct timezone *) NULL))
#define SUB_TIME(s,e)   ((double) ((e.tv_sec - s.tv_sec) + \
                          ((e.tv_usec - s.tv_usec) / 1000000.0)))

#define DEVICE          "/dev/xtipvc0"

static int hello_bytecnt = 0;
static int ptsp_bytecnt = 0;
static int db_bytecnt = 0;
static int ptse_req_bytecnt = 0;
static int ptse_ack_bytecnt = 0;
static int total_bytecnt = 0;

XTIreader::XTIreader(XTIadapter *owner, int fd):InputHandler(owner,fd),_owner(owner) { }

XTIreader::~XTIreader(){ }

void XTIreader::Callback() {_owner->Read();}


const VisitorType * XTIadapter::_packet_visitor = 0;

XTIadapter::XTIadapter(int port, int vpi, int vci)  
  : _port(port),_vpi(vpi),_vci(vci)
{
  if (! _packet_visitor )
    _packet_visitor = QueryRegistry( PACKET_VISITOR_NAME );

  int qos = 0;
  char *options = NULL;
  long optlen = 0;
  struct t_info info;
  _fd = t_open(DEVICE, O_RDWR, &info);
  if(_fd < 0)
    {
      cout << "t_open failed\n";
      exit(1);
    }
  if(t_bind(_fd, (struct t_bind *) NULL, (struct t_bind *) NULL) < 0)
    {
      cout << "t_bind failed\n";
      exit(1);
    }
  // no QoS, don't send TRAFFIC option 
  struct t_call call_req;
  PVCAddress addr_req;
  addr_req.addressType=AF_ATM_PVC;
  addr_req.vpi = vpi;
  addr_req.vci = vci;
  memset(&call_req, 0, sizeof(call_req));
  call_req.addr.len = sizeof(addr_req);
  call_req.addr.buf = (char *) &addr_req;

  options = (char *) NULL;
  if(t_connect(_fd, &call_req, (struct t_call *) NULL) < 0)
    {
      cout << "t_connect failed " << endl;
      exit(1);
    }
  cout << "connected " << endl;
  _reader = new XTIreader(this,_fd);
  assert(_reader);
  Register(_reader);
  // record the starting time
  GET_TIME(_start);
}

XTIadapter::~XTIadapter() { }


void XTIadapter::Read(void)
{
  int bytes_read;
  int flags;
  Visitor *v;
  u_char pdu[64000];
  bytes_read = t_rcv(_fd, (char *)pdu, 64000, &flags);
  //  cout << "READ " << bytes_read << " bytes on vpi = " << _vpi << " vci = " << _vci << endl;
  if((v = DecodePDU(pdu,bytes_read)))
    {
      Inject(v);
    }
}


void XTIadapter::Interrupt(class SimEvent *e) { }


void XTIadapter::SpecialInject( Visitor * v )
{
  assert( v != 0 );
  Inject(v);
}

void XTIadapter::Absorb(Visitor * v)
{
  if ( v->GetType().Is_A( _packet_visitor ) ) {
    PNNIVisitor * pv = (PNNIVisitor *)v;
    u_char *pdu = new u_char[2000];
    int len = 0;
    pv->encode(pdu,len);
    int bytes_sent = t_snd(_fd,(char *)pdu,len,0);
    delete [] pdu;
  } else
    cerr << "UNKNOWN VISITOR TYPE " << v->GetType() << endl;

  v->Suicide();
}

ILMI_XTIadapter::ILMI_XTIadapter(int port) : XTIadapter(port,0,16) { }

ILMI_XTIadapter::~ILMI_XTIadapter( ) { }


Visitor *ILMI_XTIadapter::DecodePDU(u_char *pdu, int len)
{
  //  cout << "Received an ILMI  packet -- ignore it for now !! " << endl;
  return 0;
}

void ILMI_XTIadapter::DumpPDU(char *prefix, u_char *pdu, int len)
{
}


SIG_XTIadapter::SIG_XTIadapter(int port) : XTIadapter(port,0,5) { }

SIG_XTIadapter::~SIG_XTIadapter( ) { }

Visitor *SIG_XTIadapter::DecodePDU(u_char *pdu, int len)
{
  //  cout << "Received a Q93B  packet -- ignore it for now !! " << endl;
  return 0;
}

void SIG_XTIadapter::DumpPDU(char *prefix, u_char *pdu, int len)
{
}


PNNI_XTIadapter::PNNI_XTIadapter(int port) : XTIadapter(port,0,18) { }

PNNI_XTIadapter::~PNNI_XTIadapter( ) 
{
  GET_TIME(_hello_time);
  GET_TIME(_ptsp_time);
  GET_TIME(_db_time);
  GET_TIME(_ptse_ack_time);
  GET_TIME(_ptse_req_time);
}


Visitor *PNNI_XTIadapter::DecodePDU(u_char *pdu, int len)
{
  PNNIVisitor *pv = 0;
  PNNIPkt::PktTypes pdu_type = (PNNIPkt::PktTypes ) (pdu[0] << 8 | pdu[1]);
  struct timeval t2;
  GET_TIME(t2);
  total_bytecnt += len;
  switch (pdu_type)
    {
    case PNNIPkt::hello:
      hello_bytecnt += len;
      cout << "TIMING: hello Time = " << SUB_TIME(_hello_time,t2) << " hello bytes = " << len << " cumul = " << hello_bytecnt << endl;
      GET_TIME(_hello_time);
      pv = new HelloVisitor();
      break;

    case PNNIPkt::ptsp:
      ptsp_bytecnt += len;
      cout << "TIMING: ptsp Time = " << SUB_TIME(_ptsp_time,t2) << " ptsp bytes = " << len << " cumul = " << ptsp_bytecnt << endl;
      GET_TIME(_ptsp_time);
      pv = new PTSPVisitor();
      break;

    case PNNIPkt::ptse_ack:
      ptse_ack_bytecnt += len;
      cout << "TIMING: ptse_ack Time = " << SUB_TIME(_ptse_ack_time,t2) << " ptse_ack bytes = " << len << " cumul = " << ptse_ack_bytecnt << endl;
      GET_TIME(_ptse_ack_time);
      pv = new AckVisitor();
      break;

    case PNNIPkt::database_sum:
      db_bytecnt += len;
      cout << "TIMING: db Time = " << SUB_TIME(_db_time,t2) << " DB bytes = " << len << " cumul = " << db_bytecnt << endl;
      GET_TIME(_db_time);
      pv = new DSVisitor();
      break;

    case PNNIPkt::ptse_req:
      ptse_req_bytecnt += len;
      cout << "TIMING: ptse_req Time = " << SUB_TIME(_ptse_req_time,t2) << " ptse_req bytes = " << len << " cumul = " << ptse_req_bytecnt << endl;
      GET_TIME(_ptse_req_time);
      pv = new ReqVisitor();
      break;

    default:
      cout << "UNKNOWN PACKET TYPE !!! " << endl;
      HexDump( pdu, len, cout );
      break;
    }
  cout << "TIMING: total bytes = " << total_bytecnt << endl;

  if ( pv != 0 ) {
    pv->SetOutPort(_port);
    pv->SetInPort(_port);
    pv->SetInVP(0);
    pv->SetInVC(18);
    pv->decode(pdu,len);
  }
  return pv;
}


void PNNI_XTIadapter::DumpPDU(char *prefix, u_char *pdu, int len)
{
  printf("%s ",prefix);
  for(int i = 0; i < len; i++)
    printf("%02x ",(int )pdu[i]);
  printf("\n");
}



