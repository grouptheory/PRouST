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
#ifndef __RESRC_AVAIL_INFO_H__
#define __RESRC_AVAIL_INFO_H__

#ifndef LINT
static char const _resrc_avail_info_h_rcsid_[] =
"$Id: resrc_avail_info.h,v 1.15 1998/09/23 18:20:07 mountcas Exp $";
#endif

#include <codec/pnni_ig/InfoGroup.h>

class ig_optional_gcac_params;

#define RAIG_FLAG_UNSUPPORTED 0x0000	/* No such RAIG.  Use this as
					 * an indication of error when
					 * determining the service class 
					 * required by a call setup.
					 */
#define RAIG_FLAG_CBR      0x8000       // Constant Bit Rate
#define RAIG_FLAG_RTVBR    0x4000       // Realtime Variable Bit Rate
#define RAIG_FLAG_NRTVBR   0x2000       // Non-realtime Variable Bit Rate
#define RAIG_FLAG_ABR      0x1000       // Available Bit Rate
#define RAIG_FLAG_UBR      0x0800       // Unspecified Bit Rate
#define RAIG_FLAG_GCAC_CLP 0x0001       // Cell Loss Priority bit

#define RAIG_CONTENT 32 // type(2), len(2), flags(2), reserved(2),
                        // addwt(4), MaxCR(4), ACR(4), CTD(4), CDV(4)
                        // CLR_0(2), CLR_01(2)

/** The Resource Availability IG is used to attach values of routing metrics and
    attributes to nodes, links, and reachable addresses.  For node, only the outgoing
    resource availability IG is used.  For links and reachable addresses, the outgoing
    resource availability IG is used to specify routing metrics and attributes from
    this node to the neighbor node or reachable address.  The incoming resource
    availability IG is used to specify routing metrics and attributes from the
    neighbor node or reachable address to this node.
  */
class ig_resrc_avail_info : public InfoGroup {
  friend ostream & operator << (ostream & os, ig_resrc_avail_info & ig);
  friend istream & operator >> (istream & os, ig_resrc_avail_info & ig);
public:

  /**@name Enumerated types of resources.
   */
  //@{
  enum resrc_type {
    /// Outgoing Resource Availability IG.
    outgoing = 0,
    /// Incoming Resource Availability IG.
    incoming = 1,
  };
  //@}

  /// Constructor.
  ig_resrc_avail_info(resrc_type type = outgoing);

  ig_resrc_avail_info(resrc_type type, u_short flags,
		      int weight, int mcr, int acr,
		      int ctd, int cdv, sh_int clr0, sh_int clr01);
  ig_resrc_avail_info(const ig_resrc_avail_info & rhs);

  /// Destructor.
  virtual ~ig_resrc_avail_info();

  /**@name Methods for encoding/decoding the IG.
   */
  //@{
    /// Encode.
  virtual u_char *encode(u_char *buffer, int & buflen);

    /// Decode.
  virtual errmsg *decode(u_char *buf);
  //@}

  const u_int GetFlags(void) const;
  const int GetAdminWeight(void) const;
  const int GetMCR(void) const;		// Maximum Cell Rate, cells/second
  const int GetACR(void) const;		// Available Cell Rate, cells/second
  const int GetCTD(void) const;		// Cell Transfer Delay, microseconds
  const int GetCDV(void) const;		// Cell Delay Variation, microseconds
  const int GetCLR0(void) const;	// Cell Loss Ratio (CLP=0)
  const int GetCLR01(void) const;	// Cell Loss Ratio (CLP=0+1)

  void  SetFlags(u_int f);
  void  SetAdminWeight(int a);
  void  SetMCR(int m);
  void  SetACR(int a);
  void  SetCTD(int c);
  void  SetCDV(int c);
  void  SetCLR0(int c);
  void  SetCLR01(int c);

  const ig_optional_gcac_params * GetGCAC(void) const;

  virtual InfoGroup * copy(void);
  void size(int & length);

protected:

  virtual bool        PrintSpecific(ostream& os);
  virtual bool        ReadSpecific(istream& is);

  resrc_type     _type;
  // RAIG Flags
  //  16    15      14      13   12   11 - 2    1 (LSB)
  // CBR  rt-VBR  nrt-VBR  ABR  UBR  Reserved  GCAC CLP
  u_short       _RAIG_Flags;
  int           _Admin_weight;
  int           _max_cell_rate;
  int           _avail_cell_rate;
  int           _cell_xfer_delay;
  int           _cell_delay_variation;
  sh_int        _cell_loss_ratio_0;
  sh_int        _cell_loss_ratio_01;

  ig_optional_gcac_params * _gcac;
};

#endif // __RESRC_AVAIL_INFO_H__
