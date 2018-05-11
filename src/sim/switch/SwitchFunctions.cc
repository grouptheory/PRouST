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
 * File: SwitchFunctions.cc
 * Author: talmage
 * Version: $Id: SwitchFunctions.cc,v 1.60 1999/02/19 21:22:53 marsh Exp $
 * Purpose: Shortcuts to creating and connecting switches.
 * BUGS:
 */
#ifndef LINT
static char const rcsid[] =
"$Id: SwitchFunctions.cc,v 1.60 1999/02/19 21:22:53 marsh Exp $";
#endif
#include <common/cprototypes.h>

#include <DS/util/String.h>
#include <FW/basics/diag.h>
#include <FW/basics/Conduit.h>
#include <FW/behaviors/Cluster.h>
#include <FW/behaviors/Protocol.h>
#include <fsm/config/Configurator.h>
#include <fsm/visitors/LinkDownVisitor.h>
#include <fsm/visitors/LinkUpVisitor.h>
#include <fsm/visitors/PortVisitor.h>
#include <sim/port/LinkState.h>
#include <sim/port/LinkStateWrapper.h>
#include <sim/port/PortAccessor.h>
#include <sim/switch/PortInstallerVisitor.h>
#include <sim/switch/StrikeVisitor.h>
#include <sim/switch/SwitchExpander.h>
#include <sim/switch/SwitchFunctions.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/resrc_avail_info.h>

#define DEFAULT_SERVICE_INTERVAL 0.01
#define DEFAULT_DEPTH 100

char * first_arg(char *, char *);

const u_long OC3  = 366745;   // Cells/sec  (155.5 MB/s / 8) / 53
const u_long OC12 = 4 * OC3;  // Cells/sec
const u_long OC48 = 4 * OC12; // Cells/sec

//
// Create a switch with name 'name'.  Its characteristics come from
// the Configurator using switchKey as the key.
//
Conduit *MakeSwitch(ds_String *switchKey,
		    ds_String *switchName,
		    NodeID *& myNode,
		    Conduit *& controlTerminal)
{
  const Configurator & globalConfig = theConfigurator();
  Conduit *answer = 0;
  PortInstallerVisitor * piv = 0;
  StrikeVisitor *sv = 0;
  int physicalLevel = globalConfig.PhysicalLevel(switchKey);
  const char *name = (switchName != 0) ? switchName->chars() : "SWITCH";
  int numPorts = globalConfig.Ports(switchKey);

  Accessor     * ac = new PortAccessor;
  SwitchExpander * ne = new SwitchExpander(switchKey, switchName, myNode, 0,
					   controlTerminal);
  Cluster      * cl = new Cluster(ne, 0, ac);
  answer = new Conduit(name, cl);

  //
  // Install each one of the ports.  None of them have any virtual circuits.
  //
  if ((answer != 0) && (numPorts > 0)) {
    for (int port = 1; port < numPorts; port++) {
      NodeID * myNodeCopy = myNode->copy();
      piv = new PortInstallerVisitor(port, myNodeCopy);
      A_half(answer)->Accept(piv);
    }
    sv = new StrikeVisitor();
    A_half(answer)->Accept(sv);
  }

  return answer;
}


//
// Link port1 of switch1 to port2 of switch2.
// Port2 defaults to port1.
//
// Tells each switch that the link is up.
//
// Assumes that switch1 and switch2 were created by MakeSwitch().
//
LinkStateWrapper * LinkSwitches(Conduit *switch1, int port1, 
				Conduit *switch2, int port2,
				list<ig_resrc_avail_info *> * raigs1,
				list<ig_resrc_avail_info *> * raigs2,
				u_int aggregation_token)
{
  LinkStateWrapper * answer = 0;
  bool usedFakeRAIGs1 = false;
  bool usedFakeRAIGs2 = false;

  if (port2 < 0) 
    port2 = port1;

  int bw = OC3;

  if (raigs1 != 0 || raigs2 != 0) {
    ig_resrc_avail_info * rai = raigs1 ? raigs1->head() : raigs2->head();
    if (rai != 0)
      bw = rai->GetACR();
  }

  // Create a simple Link
  LinkState * st = // new DefaultLinkState(port1, port2);
    new QueuingLinkState( DEFAULT_SERVICE_INTERVAL, port1, port2, bw );
  char buf[Conduit::MAX_LOG_SIZE];
  sprintf(buf, "Link-%s-%s", switch1->GetName(), switch2->GetName());
  Conduit  * ln = new Conduit(buf, st);

  PortVisitor * v1 = new PortVisitor(port1);
  PortVisitor * v2 = new PortVisitor(port2);

  if ((Join(B_half(switch1), A_half(ln), v1, 0) && 
       Join(B_half(switch2), B_half(ln), v2, 0))) {

    ig_resrc_avail_info * rai = 0;
    if (!raigs1) {
      usedFakeRAIGs1 = true;
      raigs1 = new list<ig_resrc_avail_info *>;
      rai = new 
	ig_resrc_avail_info(ig_resrc_avail_info::outgoing, 
			    (u_short)(RAIG_FLAG_CBR | RAIG_FLAG_RTVBR |
				      RAIG_FLAG_NRTVBR | RAIG_FLAG_ABR |
				      RAIG_FLAG_UBR | RAIG_FLAG_GCAC_CLP), 
			    5040, OC3, OC3, 202, 182, 8, 8);

      raigs1->append(rai);
    }

    if (!raigs2) {
      usedFakeRAIGs2 = true;
      raigs2 = new list<ig_resrc_avail_info *>;
      rai = new 
	ig_resrc_avail_info(ig_resrc_avail_info::outgoing, 
			    (u_short)(RAIG_FLAG_CBR | RAIG_FLAG_RTVBR |
				      RAIG_FLAG_NRTVBR | RAIG_FLAG_ABR |
				      RAIG_FLAG_UBR | RAIG_FLAG_GCAC_CLP), 
			    5040, OC3, OC3, 202, 182, 8, 8);


      raigs2->append(rai);
    }

    LinkUpVisitor * luv1 = new LinkUpVisitor(port1);
    luv1->SetRAIGs(raigs1);
    luv1->SetAgg(aggregation_token);
    LinkUpVisitor * luv2 = new LinkUpVisitor(port2);
    luv2->SetRAIGs(raigs2);
    luv2->SetAgg(aggregation_token);

    luv1->SetLoggingOn();
    luv2->SetLoggingOn();

    answer = new LinkStateWrapper(ln, st, switch1, switch2, port1, port2);

    B_half(switch1)->Accept(luv1);
    B_half(switch2)->Accept(luv2);
  } else
    delete ln;

  v1->Suicide();
  v2->Suicide();

  //
  // Reclaim the space in raigs1 if we allocated it ourselves.
  //
  if (usedFakeRAIGs1) {
    while (raigs1->size() > 0) {
      ig_resrc_avail_info *rai_copy = raigs1->pop();
      delete rai_copy;
    }
    //    raigs1->clear();
    delete raigs1;
  }

  //
  // Reclaim the space in raigs2 if we allocated it ourselves.
  //
  if (usedFakeRAIGs2) {
    while (raigs2->size() > 0) {
      ig_resrc_avail_info *rai_copy = raigs2->pop();
      delete rai_copy;
    }
    //    raigs2->clear();
    delete raigs2;
  }

  return answer;
}

LinkStateWrapper * LinkSwitches(Conduit * switch1, int port1, 
				Conduit * switch2, int port2,
				long fwd_bandwidth, 
				long bwd_bandwidth,
				u_int aggregation_token)
{
  LinkStateWrapper * rval = 0;

  ig_resrc_avail_info * frai = 
    new ig_resrc_avail_info(ig_resrc_avail_info::outgoing, 
			    (u_short)(RAIG_FLAG_CBR | RAIG_FLAG_RTVBR |
			    RAIG_FLAG_NRTVBR | RAIG_FLAG_ABR |
			    RAIG_FLAG_UBR | RAIG_FLAG_GCAC_CLP), 
			    5040, fwd_bandwidth, fwd_bandwidth, 202, 182, 8, 8);

  ig_resrc_avail_info * brai = 
    new ig_resrc_avail_info(ig_resrc_avail_info::outgoing, 
			    (u_short)(RAIG_FLAG_CBR | RAIG_FLAG_RTVBR |
			    RAIG_FLAG_NRTVBR | RAIG_FLAG_ABR |
			    RAIG_FLAG_UBR | RAIG_FLAG_GCAC_CLP), 
			    5040, bwd_bandwidth, bwd_bandwidth, 202, 182, 8, 8);


  list<ig_resrc_avail_info *> * fwd_raigs = new list<ig_resrc_avail_info *>;
  fwd_raigs->append( frai );

  list<ig_resrc_avail_info *> * bwd_raigs = new list<ig_resrc_avail_info *>;
  bwd_raigs->append( brai );

  rval = LinkSwitches(switch1, port1, switch2, port2, 
		      fwd_raigs, bwd_raigs, aggregation_token);

  // cleanup
  while (fwd_raigs->size() > 0) {
    ig_resrc_avail_info * rai_copy = fwd_raigs->pop();
    delete rai_copy;
  }
  //  fwd_raigs->clear();
  delete fwd_raigs;

  while (bwd_raigs->size() > 0) {
    ig_resrc_avail_info * rai_copy = bwd_raigs->pop();
    delete rai_copy;
  }
  //  bwd_raigs->clear();
  delete bwd_raigs;

  return rval;
}


//
// The link is aggregated using the forward raigs aggregation token.
//
LinkStateWrapper * LinkSwitches(Conduit *switch1, int port1, 
				Conduit *switch2, int port2,
				ds_String *fwd_raigs_key, 
				ds_String *bwd_raigs_key)
{
  const Configurator & globalConfig = theConfigurator();
  LinkStateWrapper *answer = 0;
  int aggregationToken = 0;

  list<ig_resrc_avail_info *> *forward = 0;
  if (fwd_raigs_key != 0) {
    forward = globalConfig.RAIGs(fwd_raigs_key);
    aggregationToken = globalConfig.AggregationToken(fwd_raigs_key);
  }

  list<ig_resrc_avail_info *> *backward = 0;
  if (bwd_raigs_key != 0) {
    backward = globalConfig.RAIGs(bwd_raigs_key);
  }
    

  answer = LinkSwitches(switch1, port1, switch2, port2, 
			forward, backward, aggregationToken);


  // cleanup
  if (forward != 0) {
    while (forward->size() > 0) {
      ig_resrc_avail_info * rai_copy = forward->pop();
      delete rai_copy;
    }
    delete forward;
  }

  if (backward != 0) {
    while (backward->size() > 0) {
      ig_resrc_avail_info * rai_copy = backward->pop();
      delete rai_copy;
    }
    delete backward;
  }

  return answer;
}


//
// Unlink Link port1 of switch1 from port2 of switch2.
// Port2 defaults to port1.
//
// Tells each switch that the link is down.
//
// Assumes that switch1 and switch2 were created by MakeSwitch().
//
bool UnlinkSwitches(LinkStateWrapper *link)
{
  bool answer = false;

  if (link != 0) {
    Conduit *left = link->LeftNeighbor();
    Conduit *right = link->RightNeighbor();
    Conduit *theLink = link->Container();
    int leftPort = link->LeftPort();
    int rightPort = link->RightPort();

    PortVisitor *v1 = new PortVisitor(leftPort);
    PortVisitor *v2 = new PortVisitor(rightPort);

    if (answer = (Sever(B_half(left), A_half(theLink), v1, 0) &&
		  Sever(B_half(right), B_half(theLink), v2, 0))) {
      LinkDownVisitor *ldv1 = new LinkDownVisitor(leftPort);
      LinkDownVisitor *ldv2 = new LinkDownVisitor(rightPort);

      ldv1->SetLoggingOn();
      ldv2->SetLoggingOn();

      B_half(left)->Accept(ldv1);
      B_half(right)->Accept(ldv2);

      delete theLink;
    }

    v1->Suicide();
    v2->Suicide();
    delete link;
  }

  return answer;
}

#if 0
const list<Conduit *> * ParseNetwork(const char * filename)
{
  Conduit * controlTerminal = 0;
  ifstream in(filename);

  if (in.bad()) {
    cerr << "Unable to open '" << filename << "'." << endl;
    return 0;
  }
  
  list<Conduit *> * lptr = new list<Conduit *>;
  dictionary<const char *, Conduit *> mapping;
  // Parse the data until there is no data left to parse.
  while (! in.eof()) {
    Conduit * current, * lhs = 0, * rhs = 0;
    LinkStateWrapper * link;
    Addr * addr; NodeID * nid, * pnid, * ppgl;
    char buf[160], word[80], port[20], port2[20], address1[80], address2[80], filename[80], * cptr;

    in.getline(buf, 160);
    cptr = first_arg(buf, word);

    switch (word[0]) {
      case 'H':
      case 'h':
	// new Host
	// H <addr> <filename>
	cptr = first_arg(cptr, word);
	cptr = first_arg(cptr, filename);
	strncpy(buf, filename, strlen(filename) - 3);
	buf[strlen(filename) - 2] = '\0';

	DIAG(SIM, DIAG_DEBUG, 
	     cout << "Creating Host " << buf << " with NodeID " 
	     << word << " from file " << filename << endl;)

	current = MakeHost(buf, DEFAULT_SERVICE_INTERVAL, DEFAULT_DEPTH, 
			   addr, filename);
	mapping.insert(strdup(word), current);
	lptr->append(current);
	break;
      case 'S':
      case 's':
	// new Switch
	// S <addr> <# ports> <filename>
	cptr = first_arg(cptr, word);
	cptr = first_arg(cptr, port);
	cptr = first_arg(cptr, filename);
	strncpy(buf, filename, strlen(filename) - 3);
	buf[strlen(filename) - 2] = '\0';

	DIAG(SIM, DIAG_DEBUG, 
	     cout << "Creating Switch " << buf << " with NodeID " 
	     << word << " from file " << filename << endl;)

	current = MakeSwitch(buf, atoi(port), 
			     nid, pnid, ppgl, controlTerminal, filename);
	mapping.insert(strdup(word), current);
	lptr->append(current);
	break;
      case 'L':
      case 'l':
	// new Connection
	// L <addr> <port #> <addr2> <port #>
	cptr = first_arg(cptr, address1);
	cptr = first_arg(cptr, port);
	cptr = first_arg(cptr, address2);
	cptr = first_arg(cptr, port2);

	dic_item di;
	forall_items(di, mapping) {
	  const char * str = mapping.key(di);

	  if (!strcmp(str, address1))
	    lhs = mapping.inf(di);
	  if (!strcmp(str, address2))
	    rhs = mapping.inf(di);

	  if (lhs && rhs)
	    break;
	}

	if (!lhs || ! rhs) {
	  cerr << "Unable to link " << address1 << " and " << address2 << endl;
	  continue;
	} else {
	  // these are leaked for now.
	  link = LinkSwitches(lhs, atoi(port), rhs, atoi(port2));
	}
	break;
      default:
	// line is a comment or blank.
        continue;
        break;
    }
  }
  return lptr;
}
#endif
