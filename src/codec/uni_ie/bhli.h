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

#ifndef _BHLI_H_
#define _BHLI_H_
#if !defined(LINT)
static char const _bhli_h_rcsid_[] =
"$Id: bhli.h,v 1.7 1998/04/08 15:02:55 mountcas Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

/*
 * Broadband Higher Layer Information:
 * see 5.4.5.8 page 218 in UNI-3.1 book
 * provides a means to be used by the addressed entity to do compatibility
 * checking. Mandatory at the private UNI.
 */
class ie_errmsg;
class iso_bhli;
class user_bhli;
class vendor_bhli;

class ie_bhli: public InfoElem {
public:

  enum bhli_type {
    unspecified_or_unknow           = 0x0,
    iso                             = 0x80,
    user_specific                   = 0x81,
    vendor_specific_application_id  = 0x83
  };

  ie_bhli(bhli_type t);
  virtual ~ie_bhli();
  virtual InfoElem* copy(void) const=0;
  virtual int operator == (const ie_bhli &) const=0;

  bhli_type get_bhli_type(void) const;

  virtual u_char * get_bhli_buffer(void)=0;
  virtual void     set_bhli(u_char * buffer, int buflen)=0;
  virtual int      get_bhli_buflen(void) const=0;

  virtual int                 encode(u_char * buffer) = 0;
  virtual InfoElem::ie_status decode(u_char *, int &) = 0;
  virtual int Length() = 0;

  ie_bhli(void);

protected:

  char PrintSpecific(ostream & os);

  bhli_type _bhli_type;
};

class iso_bhli : public ie_bhli{
 public:

  int equals(const InfoElem* him) const;
  int equals(const iso_bhli* himptr) const;

  iso_bhli(u_char *buffer, int buflen);
  iso_bhli(const iso_bhli & rhs);
  iso_bhli(void);
  virtual ~iso_bhli();

  InfoElem* copy(void) const;
  int operator == (const ie_bhli &) const;

  void set_bhli(u_char * buffer, int buflen);
  int  get_bhli_buflen(void) const;
  u_char* get_bhli_buffer(void);

  const iso_bhli * get_iso_bhli(void) const;

  int                 encode(u_char * buffer);
  InfoElem::ie_status decode(u_char *, int &);
  int Length();
protected:

  char PrintSpecific(ostream & os);
  friend ostream & operator << (ostream & os, iso_bhli & x);

  u_char _iso_bhli[8];
  int _iso_bhli_len;
};

class user_bhli : public ie_bhli{
 public:

  int equals(const InfoElem* him) const;
  int equals(const user_bhli* himptr) const;

  user_bhli(u_char *buffer, int buflen);
  user_bhli(const user_bhli & rhs);
  user_bhli(void);
  virtual ~user_bhli();
  InfoElem* copy(void) const;
  int operator == (const ie_bhli &) const;

  u_char* get_bhli_buffer(void);
  int get_bhli_buflen(void) const;
  void set_bhli(u_char * buffer, int buflen);

  const user_bhli * get_user_bhli(void) const;

  int                 encode(u_char * buffer);
  InfoElem::ie_status decode(u_char *, int &);
  int Length();

protected:

  char PrintSpecific(ostream & os);
  friend ostream & operator << (ostream & os, user_bhli & x);

  u_char _user_bhli[8];
  int _user_bhli_len;
};

class vendor_bhli : public ie_bhli{
 public:

  int equals(const InfoElem* him) const;
  int equals(const vendor_bhli* himptr) const;

  vendor_bhli(int oui, int app_id);
  vendor_bhli(const vendor_bhli & rhs);
  vendor_bhli(void);

  virtual ~vendor_bhli();
  InfoElem* copy(void) const;
  int operator == (const ie_bhli &) const;

  const vendor_bhli * get_vendor_bhli(void) const;

  void set_bhli(u_char * buffer, int buflen);
  int  get_bhli_buflen(void) const;
  u_char* get_bhli_buffer(void);

  int                 encode(u_char * buffer);
  InfoElem::ie_status decode(u_char *, int &);
  int Length();

protected:

  char PrintSpecific(ostream & os);
  friend ostream & operator <<(ostream & os, vendor_bhli & x);

  int _oui;
  int _app_id;
};

#endif

