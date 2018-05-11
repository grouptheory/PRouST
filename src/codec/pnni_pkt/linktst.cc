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
#ifndef LINT
static char const _linktst_cc_rcsid_[] =
"$Id: linktst.cc,v 1.7 1998/08/06 04:02:55 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <iostream.h>
#include <codec/pnni_pkt/pkt_incl.h>

void main(int argc, char ** argv)
{
  u_char    buf[255];
  int       len;
  PNNIPkt * pkt;
  errmsg  * err = 0;

  pkt = new DatabaseSumPkt();
  pkt->encode(buf, len);
  cout << "Encoded DatabaseSumPkt in " << len << " bytes." << endl;
  if (err = pkt->decode(buf))
    delete err;
  delete pkt;

  pkt = new HelloPkt((u_char *)"0000000000000000000000", (u_char *)"00000000000000000000", 
		     (u_char *)"00000000000000", (u_char *)"1111111111111111111111", 0, 1, 15);
  pkt->encode(buf, len);
  cout << "Encoded HelloPkt in " << len << " bytes." << endl;
  if (err = pkt->decode(buf))
    delete err;
  delete pkt;
  
  pkt = new PTSEAckPkt();
  pkt->encode(buf, len);
  cout << "Encoded PTSEAckPkt in " << len << " bytes." << endl;
  if (err = pkt->decode(buf))
    delete err;
  delete pkt;

  pkt = new PTSEReqPkt();
  pkt->encode(buf, len);
  cout << "Encoded PTSEReqPkt in " << len << " bytes." << endl;
  if (err = pkt->decode(buf))
    delete err;
  delete pkt;

  pkt = new PTSPPkt((u_char *)"0000000000000000000000", (u_char *)"00000000000000");
  pkt->encode(buf, len);
  cout << "Encoded PTSPPkt in " << len << " bytes." << endl;
  if (err = pkt->decode(buf))
    delete err;
  delete pkt;

  // done.
}
