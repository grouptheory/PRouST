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
// @file CallConfiguration.cc
// @author talmage
// @version $Id: CallConfiguration.cc,v 1.4 1999/02/19 21:22:56 marsh Exp $
//
// Purpose: Describes the configuration of a switch.
//

#ifndef LINT
static char const _CallConfiguration_cc_rcsid_[] =
"$Id: CallConfiguration.cc,v 1.4 1999/02/19 21:22:56 marsh Exp $";
#endif

#include "CallConfiguration.h"
#include <DS/util/String.h>
#include <codec/uni_ie/UNI40_td.h>
#include <codec/uni_ie/UNI40_bbc.h>
#include <codec/uni_ie/qos_param.h>
#include <codec/uni_ie/UNI40_xqos.h>
#include <codec/uni_ie/UNI40_e2e_trans_delay.h>

CallConfiguration::CallConfiguration(ds_String *name, 
				     UNI40_traffic_desc *td,
				     ie_bbc *bbc, 
				     ie_qos_param *qos,
				     UNI40_xqos_param *xqos,
				     UNI40_e2e_transit_delay *e2e) :
  _name(name), _traffic_descriptor(td), _bbc(bbc), _qos(qos),
  _xqos(xqos), _e2e_transit_delay(e2e)
{
}


CallConfiguration::~CallConfiguration(void)
{
  delete _name;
  delete _traffic_descriptor;
  delete _bbc;
  delete _qos;
  delete _xqos;
  delete _e2e_transit_delay;
}


// caller owns the pointer
ds_String *CallConfiguration::Name(void)
{
  ds_String *answer = 0;
  if (_name != 0) answer = new ds_String(*_name);

  return answer;
}


// caller owns the pointer
UNI40_traffic_desc *CallConfiguration::TrafficDescriptor(void)
{
  UNI40_traffic_desc *answer = 0;
  if (_traffic_descriptor != 0) 
    answer = new UNI40_traffic_desc(*_traffic_descriptor);

  return answer;
}


// caller owns the pointer
ie_bbc *CallConfiguration::BroadbandBearerCap(void)
{
  ie_bbc *answer = 0;
  if (_bbc != 0) 
    answer = new ie_bbc( *_bbc);

  return answer;
}


// caller owns the pointer
ie_qos_param *CallConfiguration::QualityOfService(void)
{
  ie_qos_param *answer = 0;
  if (_qos != 0) answer = new ie_qos_param( *_qos);

  return answer;
}


// caller owns the pointer
UNI40_xqos_param *CallConfiguration::ExtendedQualityOfService(void)
{
  UNI40_xqos_param *answer = 0;
  if (_xqos != 0) 
    answer = new UNI40_xqos_param(*_xqos);

  return answer;
}


// caller owns the pointer
UNI40_e2e_transit_delay *CallConfiguration::EndToEndTransitDelay(void)
{
  UNI40_e2e_transit_delay *answer = 0;
  if (_e2e_transit_delay != 0)
    answer = new UNI40_e2e_transit_delay(*_e2e_transit_delay);

  return answer;
}

