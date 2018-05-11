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
#ifndef __PTSE_H__
#define __PTSE_H__

#ifndef LINT
static char const _ptse_h_rcsid_[] =
"$Id: ptse.h,v 1.41 1998/12/14 14:33:14 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>
#include <codec/pnni_ig/DBKey.h>
#include <DS/containers/sortseq.h>
#include <DS/containers/list.h>

#define PTSE_HEADER 20 // type(2), len(2), ptsetype(2), res(2), ptseid(4)
                       // ptse seqno(4), ptse checksum(2), remlifetime(2)

/** The PNNI Topology State Elements includes its own checksum and authentication field,
    allowing for PTSEs from the same originating node to be bundled in different combinations
    upon origination and as they are propagated across a peer group.  A PTSE provides the 
    information unit that is used for purposes of retransmission, application of sequence
    numbers and checksums, and authentication.
 */
class ig_ptse : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_ptse & ig);
  friend istream & operator >> (istream & is, ig_ptse & ig);
  friend int compare(ig_ptse * const &, ig_ptse * const &);
  friend int operator == (ig_ptse & lhs, ig_ptse & rhs);
  friend int operator > (const ig_ptse &, const ig_ptse &);
public:

  /// Constructor.
  ig_ptse(InfoGroup::ig_id type, int id, int sequence_number, 
	  int checksum = 0, short int remaining_lifetime = 3600);
  /// Copy Constructor.
  ig_ptse(const ig_ptse & rhs);

  /**@name Methods for encoding/decoding the IG.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char * buffer, int & buflen);
    /// Decode.
  virtual errmsg *decode(u_char * buffer);
  //@}

  int equals(ig_ptse * rhs) const;

  int equals(ig_ptse & rhs) const;

  const short int  GetTTL(void)  const;
  const short int  GetCS(void)   const;
  const       int  GetSN(void)   const;
  const       int  GetID(void)   const;
  InfoGroup::ig_id GetType(void) const;
  
  void         SetTTL(short int t);
  void         SetCS(short int c);
  void         SetSN(int sn);
  void         SetID(int id);
  void         SetType(InfoGroup::ig_id t);
				
  bool                      AddIG(InfoGroup * p);
  const list<InfoGroup *> * ShareMembers(void) const;
  void                      RemMembers(void);
  bool                      Contains(const InfoGroup * ig) const;

  ig_ptse * copy_husk(void);

  ig_ptse * Reference(void);

  void UnReference(void);
  const int GetRefs(void);

  void Sentence(seq_item li, sortseq<DBKey *,ig_ptse *> * death_row);

  virtual InfoGroup * copy(void);
  void size(int & length);
  virtual int equals(const InfoGroup * other) const;

protected:

  virtual bool PrintSpecific(ostream & os);
  virtual bool ReadSpecific(istream & os);

  int       _ref_count;
  
  InfoGroup::ig_id _type;
  // int reserved
  int       _identifier;
  int       _seq_num;
  short int _checksum;
  short int _remaining_lifetime;

  list<InfoGroup *> * _members;

  seq_item                      _my_death_row_entry;
  sortseq<DBKey *, ig_ptse *> * _death_row;

private:

  static long         _glob_ptses;
  // ig_ptse * cannot be used because the compare method will return erroneous results
  static list<long> * _ptsesList;  

  /// Destructor.  Don't call this.  Call UnReference(void)
  virtual ~ig_ptse();
};

#endif // __PTSE_H__
