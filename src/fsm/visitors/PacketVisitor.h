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
#ifndef __PACKETVISITOR_H__
#define __PACKETVISITOR_H__

#ifndef LINT
static char const _PacketVisitor_h_rcsid_[] =
"$Id: PacketVisitor.h,v 1.11 1998/12/18 14:28:06 mountcas Exp $";
#endif

#include <FW/basics/VisitorType.h>

#define PACKET_VISITOR_NAME "PacketVisitor"

// PacketVisitor has an essential requirement, that you define the
// encode and decode methods which serialize the Visitor such that it
// can be encoded, deleted, and a new Visitor constructed with the
// encoded byte stream that is EXACTLY identical to the original
// Visitor.
class PacketVisitor {
  friend PacketVisitor * decode(const unsigned char *& buffer, int & length);
public:

  // returns true if the encoding was successful, false otherwise
  // buffer is a pointer to an already allocated array of u_chars
  // length is a reference to an integer representing the length of the encoded stream.
  virtual bool encode(      unsigned char *& buffer, int & length) const = 0;
  // returns true if the decoding was successful, false otherwise
  // buffer is a pointer to an encoded stream of u_chars
  // length contains the length of the specified buffer
  virtual bool decode(const unsigned char *& buffer, int & length)       = 0;

protected:

  PacketVisitor(vistype & child_type);

  virtual ~PacketVisitor();

  static vistype _my_type;
};

#endif
