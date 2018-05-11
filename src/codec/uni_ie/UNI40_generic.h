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
#ifndef __GENERIC_ID_H__
#define __GENERIC_ID_H__

#ifndef LINT
static char const _UNI40_generic_h_rcsid_[] =
"$Id: UNI40_generic.h,v 1.6 1998/09/01 20:43:56 battou Exp $";
#endif

extern "C" {
#include <string.h>
};
#include <codec/uni_ie/ie_base.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/errmsg.h>

/**  The Generic Identifier transport IE is used to carry an
     identifier between two users.  UNI 4.0 sec. 2.1.1
 */
class UNI40_generic_id : public InfoElem {
  friend ostream & operator << (ostream & os, UNI40_generic_id & x);
public:
 
  enum id_related_std_app {
    DSMCC = 0x01,
    H245  = 0x02
    // All other values are reserved
  };

  enum id_types {
    Session  = 0x01,
    Resource = 0x02
    // All other values are reserved
  };

  UNI40_generic_id(id_related_std_app type = DSMCC, id_types id = Session, 
                u_char *val = 0L, int n = 1);
  UNI40_generic_id(const UNI40_generic_id & rhs);
  virtual ~UNI40_generic_id( );

  UNI40_generic_id & operator = (const UNI40_generic_id & rhs);
  int  operator == (const UNI40_generic_id & rhs) const;
  int  equals(const UNI40_generic_id * rhs) const;
  int  equals(const InfoElem * rhs) const;

  UNI40_generic_id *copy(void);
  InfoElem *copy(void) const;

  int                 encode(u_char *buffer);
  InfoElem::ie_status decode(u_char *buffer, int & id);

  id_related_std_app getRelated(void) const;
  id_types           getType(void) const;
  u_char            *getValue(void) const;

  void               setRelated(id_related_std_app rel);
  void               setType(id_types type);
  void               setValue(u_char * val);
  void               setRepeats(int n);
  int                max_repeats(void);
  int                Length();
protected:

  char PrintSpecific(ostream & os);

private:

  id_related_std_app    _idr;
  id_types              _id_type;
  u_char               *_id_value;
  // the octet group 6 can be repeated N times
  //  see UNI 4.0 2.1.1
  int                   _repeats;
};

#endif // __GENERIC_ID_H__



