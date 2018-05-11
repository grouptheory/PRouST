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

//-*-C++-*-

#ifndef __XTI_ADAPTER_H__
#define __XTI_ADAPTER_H__
#ifndef LINT
static char const _xti_adapter_h_rcsid_[] =
"$Id: xti_adapter.h,v 1.14 1999/03/05 19:25:27 battou Exp $";
#endif

#include <FW/kernel/Handlers.h>
#include <FW/actors/State.h>
#include <sys/time.h>

class PNNIVisitor;

class XTIadapter : public Terminal {
  friend class XTIreader;
public:
  XTIadapter(int port, int vpi, int vci);
  virtual ~XTIadapter();
  void Absorb(Visitor* v);
  void Interrupt(class SimEvent *e);
  virtual Visitor *DecodePDU(u_char *pdu, int size) = 0;
  virtual void DumpPDU(char *prefix, u_char *pdu, int size) = 0;
  void SpecialInject( Visitor * v );
protected:
  void Read(void);
  int _port;
  int _vpi;
  int _vci;
  InputHandler *_reader;
  int _fd;
  struct timeval _start;
  static const VisitorType * _packet_visitor;
};


class ILMI_XTIadapter : public XTIadapter {
public:
  ILMI_XTIadapter(int port);
  virtual ~ILMI_XTIadapter();
  Visitor *DecodePDU(u_char *pdu, int size);
  void DumpPDU(char *prefix, u_char *pdu, int size);
};

class SIG_XTIadapter : public XTIadapter {
public:
  SIG_XTIadapter(int port);
  virtual ~SIG_XTIadapter();
  Visitor *DecodePDU(u_char *pdu, int size);
  void DumpPDU(char *prefix, u_char *pdu, int size);
};

class PNNI_XTIadapter : public XTIadapter {
public:
  PNNI_XTIadapter(int port);
  virtual ~PNNI_XTIadapter();
  Visitor *DecodePDU(u_char *pdu, int size);
  void DumpPDU(char *prefix, u_char *pdu, int len);
private:
  struct timeval _hello_time;
  struct timeval _ptsp_time;
  struct timeval _db_time;
  struct timeval _ptse_ack_time;
  struct timeval _ptse_req_time;
};



class XTIreader : public InputHandler {
public:
  XTIreader(XTIadapter *owner, int fd);
  virtual ~XTIreader();
  void Callback(void);
private:
  XTIadapter *_owner;
};

#endif
