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
#ifndef __LEAF_H__
#define __LEAF_H__
#if !defined(LINT)
static char const _UNI40_leaf_h_rcsid_[] =
"$Id: UNI40_leaf.h,v 1.8 1998/09/01 20:43:45 battou Exp $";
#endif

#include <codec/uni_ie/ie_base.h>
#include <codec/uni_ie/cause.h>
#include <codec/uni_ie/errmsg.h>

/** Leaf Initiated Join Call Identifier
    uniquely identifies a point-to-multipoint call at a Root's
    interface.  UNI 4.0 sec. 6.1.2.1
    */
class UNI40_lij_call_id : public InfoElem {
  friend ostream & operator << (ostream & os, UNI40_lij_call_id & x);
public:
  ///
  UNI40_lij_call_id(u_char type = 0, int value = 0); 

  ///
  UNI40_lij_call_id(const UNI40_lij_call_id & lij);

  ///
  virtual ~UNI40_lij_call_id( ) { }

  ///
  UNI40_lij_call_id & operator = (const UNI40_lij_call_id & right);
  ///
  int                 encode(u_char *buffer);
  ///
  InfoElem::ie_status decode(u_char *buffer, int & id);

  ///
  UNI40_lij_call_id * copy(void);
  ///
  InfoElem *copy(void) const { return (InfoElem *)new UNI40_lij_call_id(this->_type, this->_value); }

  ///
  int operator == (const UNI40_lij_call_id & lp) const;
  ///
  virtual int equals(const InfoElem * him) const;
  ///
  virtual int equals(const UNI40_lij_call_id * himptr) const;
  int Length();
  bool IsRootAssigned() {return (_type == (u_char )0);}
  int GetValue() {return _value;}
protected:
  ///
  virtual char PrintSpecific(ostream & os);

private:
  ///
  u_char   _type;           /// 4 bits
  int      _value;          // 32 bit integer
};


// Leaf Initiated Join Parameters
//   used by the Root to associate options with the call when the
///   call is created.  UNI 4.0  sec. 6.1.2.2

class UNI40_lij_params : public InfoElem {
  ///
  friend ostream & operator << (ostream & os, UNI40_lij_params & x);
public:
  ///
  UNI40_lij_params(u_char screening = 0) : 
                InfoElem(UNI40_leaf_params_id), _screening_ind(screening) { }

  ///
  UNI40_lij_params(const UNI40_lij_params & lij) : 
                InfoElem(UNI40_leaf_params_id), _screening_ind(lij._screening_ind) { }

  ///
  virtual ~UNI40_lij_params( ) { }

  ///
  int encode(u_char *buffer);
  ///
  InfoElem::ie_status decode(u_char *buffer, int & id);

  ///
  UNI40_lij_params * copy(void);
  ///
  InfoElem * copy(void) const { return (InfoElem *)new UNI40_lij_params(this->_screening_ind); }

  ///
  int operator == (const UNI40_lij_params & lp) const;
  ///
  virtual int equals(const InfoElem * him) const;
  ///
  virtual int equals(const UNI40_lij_params * himptr) const;

  int Length();
  bool IsNetJoinNoRootNotify() {return (_screening_ind == (u_char )0);}
protected:
  ///
  virtual char PrintSpecific(ostream & os);

private:
  ///
  u_char   _screening_ind;   // 4 bits
};


// Leaf Sequence Number
//   used by a joining Leaf to associate a SETUP, ADD PARTY or
//   LEAF SETUP FAILURE response message with the corresponding
//   LEAF SETUP REQUEST message that triggered the response.
///   UNI 4.0  sec. 6.1.2.3

class UNI40_leaf_sequence_num : public InfoElem {
  ///
  friend ostream & operator << (ostream & os, UNI40_leaf_sequence_num & x);
public:
  ///
  UNI40_leaf_sequence_num(int sequence = 0) : 
                       InfoElem(UNI40_leaf_sequence_num_id), _seq_num(sequence) { }

  ///
  UNI40_leaf_sequence_num(const UNI40_leaf_sequence_num & ls) : 
                       InfoElem(UNI40_leaf_sequence_num_id), _seq_num(ls._seq_num) { }

  ///
  virtual ~UNI40_leaf_sequence_num( ) { }

  ///
  int encode(u_char *buffer);
  ///
  InfoElem::ie_status decode(u_char *buffer, int & id);

  ///
  UNI40_leaf_sequence_num * copy(void);
  ///
  InfoElem *copy(void) const { return (InfoElem *) new UNI40_leaf_sequence_num(this->_seq_num); }

  ///
  int operator == (const UNI40_leaf_sequence_num & lp) const;
  ///
  virtual int equals(const InfoElem * him) const;
  ///
  virtual int equals(const UNI40_leaf_sequence_num * himptr) const;
  int Length();
  int getSeqNum() {return _seq_num;}
  void setSeqNum(int seq_num) {_seq_num = seq_num;}
protected:
  ///
  virtual char PrintSpecific(ostream & os);

private:
  ///
  int     _seq_num;       // 32 bit integer
};


#endif

