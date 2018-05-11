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
#ifndef __NODAL_STATE_H__
#define __NODAL_STATE_H__

#ifndef LINT
static char const _nodal_state_h_rcsid_[] =
"$Id: nodal_state_params.h,v 1.7 1998/09/23 18:08:30 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>
#include <DS/containers/list.h>

#define NODAL_STATE_HEADER 16 //type(2), len(2), flags(2), Res(2), In pid(4)
                              // Out Pid(4)

/** Nodal State Parameter IGs are used to advertise elements of this node's
  complex node representation.  All metrics and attributes for each
  input-output port pair must be included in the same nodal state parameters
  information group.  This information group can appear multiple times and
  in multiple different PTSEs, with different pairs of input and output
  ports each time.
 */
class ig_resrc_avail_info;

class ig_nodal_state_params : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_nodal_state_params & ig);
  friend istream & operator >> (istream & is, ig_nodal_state_params & ig);

public:

  enum flags {
    vp_cap_flag = 0x8000, // 1000 0000 0000 0000
  };

  /// Constructor.
  ig_nodal_state_params(sh_int flags = 0, int ip = 0, int op = 0);

  /// Constructor, call when decoding a pkt
  ig_nodal_state_params(const ig_nodal_state_params & rhs);

  /// Destructor.
  virtual ~ig_nodal_state_params();

  /**@name Methods for encoding/decoding parameters.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char * buffer, int & buflen);
    /// Decode.
  virtual errmsg *decode(u_char * buffer);
  //@}

  /**@name Bit manipulation methods.
   */
  //@{
    /// Set the specified bit.
  void SetFlag(flags flag) { _flags |= flag; }
    /// Check if the specified bit is set.
  bool IsSet(flags flag) { return (_flags & flag); }
    /// Un-set the specified bit.
  void RemFlag(flags flag) { _flags &= ~flag; }
  //@}

  const int GetInputPID(void) const;
  const int GetOutputPID(void) const;
  void  AddRAIG(ig_resrc_avail_info *p);
  const list<ig_resrc_avail_info *> & GetRAIGS(void) const;
  virtual InfoGroup * copy(void);
  void size(int & length);
  
protected:

  virtual bool PrintSpecific(ostream & os);
  virtual bool ReadSpecific(istream & os);

  sh_int _flags;
  int    _input_pid;
  int    _output_pid;
  list<ig_resrc_avail_info *> _raigs;
};

#endif // __NODAL_STATE_H__
