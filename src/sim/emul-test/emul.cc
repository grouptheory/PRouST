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
static char const _emul_cc_rcsid_[] =
"$Id: emul.cc,v 1.16 1999/03/05 19:23:23 talmage Exp $";
#endif

#include <common/cprototypes.h>
#include <iostream.h>
#include <FW/basics/diag.h>

#include <DS/util/String.h>
#include <FW/basics/Conduit.h>
#include <FW/kernel/Kernel.h>
#include <codec/pnni_ig/id.h>
#include <fsm/config/Configurator.h>
#include <fsm/visitors/PortVisitor.h>
#include <fsm/visitors/LinkUpVisitor.h>
#include <sim/switch/SwitchFunctions.h>
#include <fsm/omni/OmniProxy.h>

#include "emulTerminal.h"
#include "xti_adapter.h"
#include "VPIVCImux.h"
#include "XTIadapterInstallerVisitor.h"

#define BUILD_VPVC(vpi, vci) (((vpi&0xff) << 16) | ((vci) & 0xffff))

vistype XTIadapterInstallerVisitor::_my_type(XTI_VISITOR_NAME);

void main(int argc, char ** argv)
{
  ds_String *switchKey = new ds_String("PRouST-Switch");
  ds_String *switchName = new ds_String("PRouST-1");
  const Configurator &globalConfigurator = theConfigurator("switch.conf");
  loadDiagPrefs();

  char nsap[80];

  DiagLevel("fsm",DIAG_DEBUG);
  DiagLevel("fsm.hello",DIAG_DEBUG);
  DiagLevel("fsm.forwarder", DIAG_DEBUG);
  DiagLevel("fsm.nodepeer", DIAG_DEBUG);

  VisPipe( "xti.log" );

  char nsc_buf[64];
  sprintf(nsc_buf, "%s.OMNI", argv[0]);
  theOmniProxy().setOutput( nsc_buf );

  NodeID * nid = globalConfigurator.BaseNodeID(switchKey);

  Conduit * ct = new Conduit("TopGun", new EmulTerminal(nid->copy()));

  Conduit * sw = MakeSwitch( switchKey, switchName, nid, ct);

  assert(sw);

  VPVCaccessor * a = new VPVCaccessor( );
  Mux * m = new Mux(a);
  Conduit *_mux = new Conduit("XTI_MUX", m);


  XTIadapter * ilmi = new ILMI_XTIadapter(1);
  Conduit  * ilmic = new Conduit("ILMI", ilmi);

  XTIadapter * sig = new SIG_XTIadapter(1);
  Conduit  * sigc = new Conduit("Q93B", sig);

  XTIadapter * pnni = new PNNI_XTIadapter(1);
  Conduit  * pnnic = new Conduit("PNNI", pnni);
  

  Visitor * vis = new PortVisitor( 1 );
  assert(vis);

  if(!Join(B_half(sw), A_half(_mux), vis, 0))
    {
      cerr << "Unable to join " << sw->GetName() << " to " << _mux->GetName() << endl;
      exit( 1 );
    }

  u_int vpvc;
  vpvc = BUILD_VPVC(0,16);
  Visitor *xv_0_16 = new XTIadapterInstallerVisitor(vpvc,ilmic);

  vpvc = BUILD_VPVC(0,5);
  Visitor *xv_0_5 = new XTIadapterInstallerVisitor(vpvc,sigc);

  vpvc = BUILD_VPVC(0,18);
  Visitor *xv_0_18 = new XTIadapterInstallerVisitor(vpvc,pnnic);


  if(!Join(B_half(_mux), A_half(ilmic), xv_0_16,0))
    {
      cerr << "Unable to join " << _mux->GetName() << " to " << ilmic->GetName() << endl;
      exit( 1 );
    }

  if(!Join(B_half(_mux), A_half(sigc), xv_0_5,0))
    {
      cerr << "Unable to join " << _mux->GetName() << " to " << sigc->GetName() << endl;
      exit( 1 );
    }

  if(!Join(B_half(_mux), A_half(pnnic), xv_0_18,0))
    {
      cerr << "Unable to join " << _mux->GetName() << " to " << pnnic->GetName() << endl;
      exit( 1 );
    }

  LinkUpVisitor * luv = new LinkUpVisitor( 1, 0, nid );
  pnni->SpecialInject( luv );

  theKernel().SetSpeed( Kernel::REAL_TIME );
  theKernel().Run();
}
