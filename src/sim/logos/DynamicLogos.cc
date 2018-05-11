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
static char const _DynamicLogos_cc_rcsid_[] =
"$Id: DynamicLogos.cc,v 1.11 1999/01/06 15:27:03 mountcas Exp $";
#endif

#include <sim/logos/BaseLogos.h>
#include <sim/logos/DynamicLogos.h>
#include <DS/containers/dictionary.h>
#include <FW/basics/diag.h>
#include <common/cprototypes.h>

class Logos;
class CACRoutingFSM;

LogosRegistry * LogosRegistry::_logos_singleton=0;

void RegisterLogos(const char * name, BaseLogos *(*ptr)(Logos *))
{
  dic_item di;

  // if it's already in there remove it
  forall_items(di, theLogosRegistry()->_name_to_ptr) {
    const char * str = theLogosRegistry()->_name_to_ptr.key(di);

    if (!strcmp(str, name)) {
      delete [] (char *)str;
      theLogosRegistry()->_name_to_ptr.del_item(di);
      break;
    }
  }

  // now it's definitely not in the dictionary - so add it
  char * n = new char [strlen(name) + 1];
  strcpy(n, name);
  theLogosRegistry()->_name_to_ptr.insert(n, ptr);
}

// name should be in the form of XXXXLogosConstruct OR XXXXX
BaseLogos * AllocateLogos(const char * name, Logos * logos)
{
  dic_item di;
  if (name) {
      forall_items(di, theLogosRegistry()->_name_to_ptr) {
	  const char * str = theLogosRegistry()->_name_to_ptr.key(di);

	  if (!strcmp(str, name))
	      return (theLogosRegistry()->_name_to_ptr.inf(di))(logos);
      }
      diag( "sim.logos", DIAG_WARNING,
	    "AllocateLogos: %sLogosConstruct() not found - using default.\n",
	    name );
  }
  // otherwise
  return BaseLogosConstruct(logos);
}

LogosRegistry * theLogosRegistry(void)
{
  if (LogosRegistry::_logos_singleton == 0)
    LogosRegistry::_logos_singleton = new LogosRegistry();
  return (LogosRegistry::_logos_singleton);
}

LogosRegistry::LogosRegistry()
{
  if ( _logos_singleton == 0) {
    _logos_singleton=this;
  }
}

