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
#ifndef __PNNI_PKT_H__
#define __PNNI_PKT_H__

#ifndef LINT
static char const _pnni_pkt_h_rcsid_[] =
"$Id: pnni_pkt.h,v 1.11 1999/01/06 17:19:08 mountcas Exp $";
#endif

#include <iostream.h>
#include <codec/pnni_ig/pnni_errmsg.h>

extern "C" {
#include <sys/types.h>
};

typedef short int sh_int;

//@see Information Group Documentation.
class errmsg;

/// Base class for all PNNI packets.
class PNNIPkt {
public:

  /// Enumeration of the different types of PNNI Packets.
  enum PktTypes {

    /// Hello Packet.
    hello = 1,

    /// PTSP Packet.
    ptsp,

    /// PTSE Acknowledgement.
    ptse_ack,

    /// Database Summary.
    database_sum,

    /// PTSE Request.
    ptse_req,
  };

  enum versions {
    NewVer = 1,
    OldVer = 1,
    CurrentVer = 1,
  };

  /// Constructor takes the type of packet, the newest version and oldest version supported.
  PNNIPkt(PktTypes type, versions newv = NewVer, versions oldv = OldVer);

  /// Destructor.
  virtual ~PNNIPkt();

  /**@name Pure virtual methods for encoding/decoding which must be redefined in derived classes. */
  //@{
    /// Encode takes a buffer to store the packet in, and an int in which the length will be stored.
  virtual u_char * encode(u_char *, int &) = 0;

    /// Decode takes a buffer containing the encoded packet.
  virtual errmsg * decode(const u_char *) = 0;
  //@}

  const u_char GetCurrentVer(void) const { return _protocol_ver;}
  const u_char GetNewVer(void) const { return _new_ver; }
  const u_char GetOldVer(void) const { return _old_ver; }

  PktTypes GetType(void) const { return _type; }

  virtual PNNIPkt * copy(void) const = 0;

  virtual void size(int & length) = 0;

protected:

  /// Pure virtual print method.
  virtual bool PrintSpecific(ostream & os) const = 0;

  /// Method for printing data which all packets share.
  bool PrintGeneral(ostream &) const;
  const char * printType(void) const;

  /// Method for encoding the packet header.
  void encode_pkt_header(u_char *, int & len, versions ver = CurrentVer);

  /// Method for encoding the packet header.
  errmsg * decode_pkt_header(const u_char * buffer);

  PktTypes    _type;
  // Provided via encode_pkt_header
  sh_int   _length;
  u_char   _protocol_ver;
  u_char      _new_ver;
  u_char      _old_ver;
  // u_char reserved

  static const int pkt_header_len;
};

#endif // __PNNI_MSG_H__

