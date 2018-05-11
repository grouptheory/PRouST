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
// @file CallConfiguration.h
// @author talmage
// @version $Id: CallConfiguration.h,v 1.3 1999/02/19 21:22:57 marsh Exp $
//
// Purpose: Describes the configuration of a switch.
//

#ifndef __CALLConfiguration_H__
#define __CALLConfiguration_H__

#ifndef LINT
static char const _CallConfiguration_h_rcsid_[] =
"$Id: CallConfiguration.h,v 1.3 1999/02/19 21:22:57 marsh Exp $";
#endif

class ds_String;
class ie_bbc;
class ie_qos_param;
class UNI40_e2e_transit_delay;
class UNI40_traffic_desc;
class UNI40_xqos_param;


class CallConfiguration {
public:

  // Owns all pointers
  CallConfiguration(ds_String *name, 
		    UNI40_traffic_desc *td,
		    ie_bbc *bbc, 
		    ie_qos_param *qos,
		    UNI40_xqos_param *xqos,
		    UNI40_e2e_transit_delay *e2e);

  // Deletes all pointers
  ~CallConfiguration(void);

  // caller owns the pointer
  ds_String *Name(void);

  // caller owns the pointer
  UNI40_traffic_desc *TrafficDescriptor(void);

  // caller owns the pointer
  ie_bbc *BroadbandBearerCap(void);

  // caller owns the pointer
  ie_qos_param *QualityOfService(void);

  // caller owns the pointer
  UNI40_xqos_param *ExtendedQualityOfService(void);

  // caller owns the pointer
  UNI40_e2e_transit_delay *EndToEndTransitDelay(void);

private:
  ds_String *_name;
  UNI40_traffic_desc *_traffic_descriptor;
  ie_bbc *_bbc;
  ie_qos_param *_qos;
  UNI40_xqos_param *_xqos;
  UNI40_e2e_transit_delay *_e2e_transit_delay;
};

#endif
