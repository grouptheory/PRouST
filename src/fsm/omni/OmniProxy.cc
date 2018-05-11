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
static char const _OmniProxy_cc_rcsid_[] =
"$Id: OmniProxy.cc,v 1.3 1999/01/13 21:32:30 mountcas Exp $";
#endif

#include "OmniProxy.h"
#include <fsm/netstats/NetStatsCollector.h>
#include <common/cprototypes.h>
#include <codec/pnni_ig/id.h>
#include <codec/pnni_ig/DBKey.h>
#include <fstream.h>

OmniProxy * OmniProxy::_instance = 0;

OmniProxy & theOmniProxy(void)
{
  if (! OmniProxy::_instance)
    OmniProxy::_instance = new OmniProxy( );
  return *(OmniProxy::_instance);
}

OmniProxy::OmniProxy(void) 
  : _filename(0), _ofs(0), _POCofs( 0 ) { }

OmniProxy::~OmniProxy() 
{ 
  theNetStatsCollector().ReportNetEvent("End_Of_Simulation",
					"omni", 0, 0);
  delete [] _filename; 
  delete    _ofs; 
  delete    _POCofs;
}

void OmniProxy::setOutput(const char * filename)
{
  if ( _filename || _ofs ) {
    delete [] _filename;
    delete    _ofs;
  }
  _filename = new char [ strlen(filename) + 1 ];
  strcpy(_filename, filename);
  _ofs = new ofstream( _filename );
}

void OmniProxy::notify(const char * string)
{
  if (! _ofs) {
    // Make a default file
    char buf[64];
    sprintf(buf, "/tmp/%d.%s", getpid(), getenv("USER"));
    setOutput( buf );
  }

  *_ofs << string << endl << flush;
}

void OmniProxy::setPOCOutput(const char * filename)
{
  if ( _POCofs != 0 )
    delete _POCofs;
  if ( filename != 0 && *filename )
    _POCofs = new ofstream( filename );
}

void OmniProxy::notify(const NodeID * n, DBKey * dbk)
{
  assert( n != 0 && dbk != 0 );
  if ( _POCofs != 0 )
    *(_POCofs) << *n << " received " << *dbk << endl << flush;
}
