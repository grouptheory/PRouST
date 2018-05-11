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

#ifndef LINT
static char const _uplink_cc_rcsid_[] =
"$Id: uplink.cc,v 1.6 1998/08/06 04:04:44 bilal Exp $";
#endif
#include <common/cprototypes.h>
/*
 * File: loaddb.cc
 * Author: battou
 * Version: $Id: uplink.cc,v 1.6 1998/08/06 04:04:44 bilal Exp $
 * Purpose: Test the reading of a database from a file.
 */
#include <iostream.h>
#include <fsm/database/Database.h>
#include <FW/basics/Conduit.h>
#include <FW/actors/Terminal.h>
#include <FW/behaviors/Adapter.h>
#include <FW/behaviors/Protocol.h>
#include <codec/pnni_ig/ptse.h>
#include <codec/pnni_ig/uplinks.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_pkt/ptsp.h>
#include <fsm/visitors/FastUNIVisitor.h>

class TempTerm : public Terminal {
public:

  TempTerm(void) : Terminal() { }

  virtual void Absorb(Visitor * v) 
  { cout << OwnerName() << " Received " << v->GetType() << " " << v << endl; 
  if (v->GetType().Is_A(QueryRegistry("FastUNIVisitor"))) 
    // don't do this in production code, I'm just hacking.
    cout << "*** Type is " << ((FastUNIVisitor *)v)->GetMSGType() 
	 << ": Look for type 3 (Uplink Response)" << endl;
  v->Suicide();
  }
  virtual void Interrupt(SimEvent * event) { } // Nah

protected:

  virtual ~TempTerm() { }
};


unsigned char uc_nid[] = { 96, 120, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 };
unsigned char rm_nid[] = { 96, 120, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 };

void main(int argc, char **argv)
{
  if (argc < 2) {
    cout << "usage: " << argv[0] << " filename" << endl;
    cout << "Otherwise you won't see any output." << endl;
    exit(1);
  }
  cout << "Doing something ..." << endl;

  VisPipe(argv[1]);

  NodeID  nid(uc_nid);
  NodeID rnid(rm_nid);
  PeerID rpid(rm_nid+1);

  // Allocate a Database and two terminals
  Database * db = new Database(&nid);
  Protocol * pr = new Protocol(db);
  Conduit  * co = new Conduit("The Database", pr);

  Terminal * te = new TempTerm();
  Adapter  * ad = new Adapter(te);
  Conduit  * t1 = new Conduit("Top Terminal", ad);

             te = new TempTerm();
             ad = new Adapter(te);
  Conduit  * t2 = new Conduit("Bot Terminal", ad);

  Join(A_half(t1), A_half(co));
  Join(A_half(t2), B_half(co));

  // Construct the PTSE containing the uplink
  ig_uplinks * uplink = new ig_uplinks(0, rm_nid, rm_nid+1, 0, 0, rm_nid+2);

  ig_ptse * ptse = new ig_ptse(InfoGroup::ig_uplinks_id, 1, 1, 0, 120);
  ptse->AddIG(uplink);
  // Insert it
  db->Insert(&rnid, ptse, &rpid, true, &nid, true);

  cout << "Finished doing something ... go look at " << argv[1] << endl;
}
