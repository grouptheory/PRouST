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

/* -*- C++ -*-
 * File: InitDB.h
 * Author: battou
 * Version: $Id: InitDB.h,v 1.6 1997/10/29 14:42:40 mountcas Exp $
 * Purpose: Initialize a Database from a file.
 */

#ifndef _INIT_DB_H_
#define _INIT_DB_H_

#ifndef LINT
static char const _InitDB_h_rcsid_[] =
"$Id: InitDB.h,v 1.6 1997/10/29 14:42:40 mountcas Exp $";
#endif

extern "C" {
#include <stdio.h>
};

class Database;
class NodeID;
class PeerID;
class Conduit;

extern Conduit *InitDB(FILE *fp, const char *dbConduitName, 
		       Database *& db, NodeID *& myNodeID, 
		       NodeID *& myParentNodeID,
		       NodeID *& myPreferredPeerGroupLeader,
		       bool more = false, int level = 96);

#endif
