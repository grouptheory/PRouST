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
#ifndef __BUFFER_H__
#define __BUFFER_H__

#ifndef LINT
static char const _Buffer_h_rcsid_[] =
"$Id: Buffer.h,v 1.3 1999/02/09 18:45:39 mountcas Exp $";
#endif

class Buffer {
public:

  Buffer(int length);
  ~Buffer();
  
  bool GrowHeader(int bytes);
  bool GrowTrailer(int bytes);
  bool ShrinkHeader(int bytes);
  bool ShrinkTrailer(int bytes);

  unsigned char* data(void) const;
  int length(void) const;

  bool set_length(int newlength);

private:

  unsigned char* _current;  
  int            _length;

  unsigned char* _storage_begin;
  unsigned char* _storage_end;

  static const int _max_header = 128;
  static const int _max_trailer = 128;
};

#endif


