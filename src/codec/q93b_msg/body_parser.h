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
#ifndef _BODY_PARSER_H_
#define _BODY_PARSER_H_

#ifndef LINT
static char const _body_parser_h_rcsid_[] =
"$Id: body_parser.h,v 1.9 1998/12/15 15:19:31 mountcas Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

//-------------------------------------------------------
/*
 * this is where IE parsing errors get reported.
 *
 */

class body_parser_errmsg {
  friend class body_parser;
  friend class generic_q93b_msg;
public:
  
  char successful(void);

  void Print(ostream& os);

protected:

  body_parser_errmsg(void);
  ~body_parser_errmsg();

  char _completed;

  int * _ie_mand_missing;
  int * _ie_opt_missing;
  int * _ie_mand_corrupted;
  int * _ie_opt_corrupted;

};


//-------------------------------------------------------

/*
 * this is where IE's get parsed.
 *
 */

class Buffer;
class q93b_tp;

class body_parser {
  friend class q93b_tp;
public:

  body_parser(const u_char * buffer, int buflen);
  body_parser(InfoElem ** ie_array);
  body_parser(InfoElem ** ie_array, Buffer* buf);
  
  virtual ~body_parser();
  
  body_parser_errmsg * process(void);
  
  InfoElem ** get_ie_array(void) { return _ie_array; }
  
protected:

  void Print(ostream& os);

  virtual int  min_len(void) const = 0;
  virtual int  max_len(void) const = 0;

  virtual int   msg_header_buflen(void) = 0;
  virtual int & msg_body_buflen(void)   = 0;
  virtual int & msg_total_buflen(void)  = 0;

  body_parser_errmsg * process_decode(void);
  body_parser_errmsg * process_encode(void);

  u_char               * _body_buffer;
  int                    _body_buflen;
  int                    _body_valid;

  int                    _encode;
  char                   _duplicated_body;

  InfoElem             * _ie_array[num_ie];
  u_char               * _ie_encoded[num_ie];
  int                    _ie_len[num_ie];

  int                  * _ie_legal;
  int                  * _ie_mandatory;
  int                  * _ie_hits;

  InfoElem             * _current_ie;
  InfoElem::ieid         _current_id;
  int                    _current_len;

  body_parser_errmsg   * _errors;

  void list_errors(ostream & os);

  //----------------------------------
  virtual void SetupLegal(void)     = 0;        // define in subclasses
  virtual void SetupMandatory(void) = 0;
  //-----------------------------------

  void zero(void);
  int parse_ie_header(u_char *buffer, int buflen);

  body_parser(body_parser& him, u_char* buf, int len);
};

#endif





