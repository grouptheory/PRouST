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
#ifndef __IG_H__
#define __IG_H__

#ifndef LINT
static char const _ig_h_rcsid_[] =
"$Id: ig.h,v 1.9 1998/06/08 19:21:45 mountcas Exp $";
#endif

#include <iostream.h>
#include <codec/pnni_ig/pnni_errmsg.h>
class ig_errmsg;

typedef short int sh_int;

extern "C" {
#include <stdlib.h>
#include <sys/types.h>
#include <assert.h>
#include <common/cprototypes.h>
}

/** Abstract base class for all Information Groups.
 */
class InfoGroup {
  friend class body_parser;
  friend InfoGroup * Read_IG(istream& is);
  friend ostream & operator << (ostream & os, InfoGroup & ig);
  friend istream & operator >> (istream &, InfoGroup &);
  friend int compare(InfoGroup * const &, InfoGroup * const &);
public:

  /**@name Enumeration of ID's for all of the IGs.
   */
  //@{
  enum ig_id {
    /// Aggregation Token.
    ig_aggregation_token_id           = 32,
    /// Nodal Hierarchy List.
    ig_nodal_hierarchy_list_id        = 33,
    /// Uplink Information Attribute.
    ig_uplink_info_attr_id            = 34,
    /// LGN Horiztonal Link.
    ig_lgn_horizontal_link_ext_id     = 35,
    /// PTSE.
    ig_ptse_id                        = 64,
    /// Nodal State Parameters.
    ig_nodal_state_params_id          = 96,
    /// Nodal Information Group.
    ig_nodal_info_group_id            = 97,
    /// Outgoing resource availability.
    ig_outgoing_resource_avail_id     = 128,
    /// Incoming resource availability.
    ig_incoming_resource_avail_id     = 129,
    /// Optional GCAC Parameters.
    ig_optional_gcac_params_id        = 160,
    /// Next Higher level Binding Information.
    ig_next_hi_level_binding_info_id  = 192,
    /// Internal reachable ATM addresses.
    ig_internal_reachable_atm_addr_id = 224,
    /// External reachable ATM addresses.
    ig_exterior_reachable_atm_addr_id = 256,
    /// Horizontal Links.
    ig_horizontal_links_id            = 288,
    /// Uplinks.
    ig_uplinks_id                     = 289,
    /// Transit net ID.
    ig_transit_net_id_id              = 304,
    /// Nodal PTSE Acknowledgement.
    ig_nodal_ptse_ack_id              = 384,
    /// Nodal PTSE summary.
    ig_nodal_ptse_summary_id          = 512,
    /// Request PTSE header.
    ig_req_ptse_header_id             = 513,
    /// System capabilities.
    ig_system_capabilities_id         = 640,
    /// Unknown IG.
    ig_unknown_id                     = 0xFF
  };
  //@}

  // Internal.
  enum ig_ix {
    ig_aggregation_token_ix           = 0,
    ig_nodal_hierarchy_list_ix        = 1,
    ig_uplink_info_attr_ix            = 2,
    ig_lgn_horizontal_link_ext_ix     = 3,
    ig_ptse_ix                        = 4,
    ig_nodal_state_params_ix          = 5,
    ig_nodal_info_group_ix            = 6,
    ig_outgoing_resource_avail_ix     = 7,
    ig_incoming_resource_avail_ix     = 8,
    ig_optional_gcac_params_ix        = 9,
    ig_next_hi_level_binding_info_ix  = 10,
    ig_internal_reachable_atm_addr_ix = 11,
    ig_exterior_reachable_atm_addr_ix = 12,
    ig_horizontal_links_ix            = 13,
    ig_uplinks_ix                     = 14,
    ig_transit_net_id_ix              = 15,
    ig_nodal_ptse_ack_ix              = 16,
    ig_nodal_ptse_summary_ix          = 17,
    ig_req_ptse_header_ix             = 18,
    ig_system_capabilities_ix         = 19,
    ig_unknown_ix                     = -1
  };

  /// Constructor.
  InfoGroup(ig_id id);

  /// Destructor.
  virtual ~InfoGroup();

  /**@name Pure virtual methods for encoding/decoding.
   */
  //@{
    /** Used to encode the IG into a buffer so it can be sent out on the link.
        Pure virtual function which must be redefined in all derived classes.
      @param buffer unsigned character array in which the encoded IG is placed.
      @param buflen integer in which the size of the buffer is stored.
     */
  virtual u_char *encode(u_char *buffer, int & buflen) = 0;

    /** Decodes a packed received from the link into the IG.
        Pure virtual function which must be redefined in all derived classes.
      @param buffer unsigned character array which contains the encoded IG.
     */
  virtual errmsg *decode(u_char *buffer) = 0;
  //@}

  /**@name Accessor methods.
   */
  //@{
    /// Get ID.
  ig_id   GetId(void) const { return _id; }
    /// Set ID.
  void    SetId(ig_id id) { _id = id; }
    /// Get Length.
  int     GetLength(void) const { return _length; }
    /// Set Length.
  void    SetLength(int length) { _length = length; }
  //@}

  static const int ig_header_len;

  virtual InfoGroup * copy(void) = 0;

protected:

  ig_id  _id;
  int    _length;

          bool        PrintGeneral(ostream& os);
          bool        ReadGeneral(istream & is);
    /** Prints the data specific to the IG to the ostream.
        Pure virtual function which must be redefined in all derived classes.
      @param os ostream where the data will be printed.
     */
  virtual bool        PrintSpecific(ostream& os) = 0;

    /** Reads data into the IG from the passed istream.
        Pure virtual function which must be redefined in all derived classes.
      @param buffer unsigned character array which contains the encoded IG.
     */
  virtual bool        ReadSpecific(istream& is) = 0;
          void        encode_ig_header(u_char * buffer, int len);

  friend  void        Print_IG_name(InfoGroup::ig_ix ix, ostream& os);
  friend  const char *IGTypeToName(InfoGroup::ig_id id);
  friend  InfoGroup::ig_ix igid_to_index(InfoGroup::ig_id id);
};

#endif // __IG_BASE_H__
