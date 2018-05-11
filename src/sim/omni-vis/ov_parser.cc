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
static char const _ov_parser_cc_rcsid_[] =
"$Id: ov_parser.cc,v 1.15 1999/02/08 15:25:17 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include "ov_main.h"
#include "ov_parser.h"
#include "ov_data.h"
#include "OVNode.h"

#include <codec/pnni_ig/id.h>
#include <fstream.h>

//----------------------------------------
// Methods of class 'ov_parser'
// ---------------------------------------

ov_parser::ov_parser( ov_main * m, ov_data * d,
		      int argc, char ** argv ) 
  : _parent(m), _data(d), _continuing(false), 
    _filename(0), _input(0), _finalTime(0)
{ 
  if (argc > 1) {
    // in the below case argv[1] is -f[ont], argv[2] is filename and argv[3] is what I want
    if ( !strncmp(argv[1], "-f", 2) && argc > 3 )
      restart( argv[3] );
    else
      restart( argv[1] );
  } else
    cout << "usage: " << argv[0] << " [-font filename] [file.OMNI]" << endl;
}

ov_parser::~ov_parser() 
{ 
  delete [] _filename;
  delete    _input;
}

// stopTime defaults to -1, so we only go thru the loop once
int ov_parser::parse( int stopTime )
{
  int rval = -1;
  double time = 0.0;

  assert( _input != 0 && _input->good() );

  do {
    char line[512]; line[0] = '#';
    while (_input->eof() == false && 
	   (line[0] == '#' || !*line))  // Skip comments and empty lines
      _input->getline(line, 512);

    if ( _input->eof() )
      break;
      
    //           Time   Type       Loc              Misc  NodeID
    // FORMAT:  double string     string           string string
    //          double add|delete node|edge|leader NodeID [NodeID]
    int numScanned = sscanf( line, "%lf %s %s %s %s", 
			     &time, _chunk[0], _chunk[1], 
			     _chunk[2], _chunk[3] );
    
    //  cout << "parser: " << line << endl;
    
    if ( !strcmp("add",    _chunk[0]) ||
	 !strcmp("delete", _chunk[0]) ) {
      switch ( _chunk[1][0]) {
      case 'n': // node
	if (!strcmp( _chunk[0], "add" ))
	  _data->notify( _chunk[2] );   // Notify of new Node
	else // delete
	  _data->remove( _chunk[2] );   // Remove Node
	break;
      case 'e': // edge
	if (!strcmp( _chunk[0], "add" )) {
	  OVNode * source = (OVNode *)_data->find( _chunk[2] );
	  OVNode * dest   = (OVNode *)_data->find( _chunk[3] );
	  assert( source && dest );
	  source->addEdge( dest );
	  // Is this bidirectional notification?
	  // dest->addEdge( source );
	} else { // delete
	  OVNode * source = (OVNode *)_data->find( _chunk[2] );
	  OVNode * dest   = (OVNode *)_data->find( _chunk[3] );
	  assert( source && dest );
	  source->remEdge( dest );
	  // Is this bidirectional notification?
	  // dest->remEdge( source );
	}
	break;
      case 'l': // leader
	if (!strcmp( _chunk[0], "add" )) {
	  OVNode * source = (OVNode *)_data->find( _chunk[2] );
	  OVNode * dest   = (OVNode *)_data->find( _chunk[3] );

	  if ( dest == 0 ) // first time seeing the leader
	    dest = _data->notify( _chunk[3], true );

	  assert( source && dest );
	  source->setLeader( dest );
	} else { // delete
	  OVNode * source = (OVNode *)_data->find( _chunk[2] );
	  OVNode * dest   = (OVNode *)_data->find( _chunk[3] );
	  assert( source && dest );
	  source->setLeader( 0 );
	}
	break;
      default:
	// I'll tolerate no insolence!
	abort();
	break;
      }
      rval = 0;
    } else if (numScanned == 5) {
      NodeID * n = 0;
      if (strcmp(_chunk[3], "(null)"))
	n = new NodeID( _chunk[3] );
    
      Event * e = _data->notify( time, _chunk[0], 
				 _chunk[1], _chunk[2], n );

      if ( e != 0 ) {
	_parent->addEvent( e );
	_parent->updateTime( (int)time );
      } else {
	cerr << "WARNING: Unable to insert Event " << _chunk[0] 
	     << " " << _chunk[1] << " " << _chunk[2] << endl;
      }

      rval = 0;
    }
  } while ( time < stopTime );

  if ( _input->eof() )
    _continuing = false;

  return rval;
}

const char * ov_parser::input(void) const
{
  return _filename;
}

bool ov_parser::continuing(void) const
{
  return _continuing;
}

void ov_parser::continuing(bool b)
{
  _continuing = b;
}

void ov_parser::restart(const char * filename)
{
  delete [] _filename; _filename = 0;
  if (_input) {
    _input->close();
    delete _input;
  }

  if (filename) {
    _filename = new char [ strlen(filename) + 1 ];
    strcpy(_filename, filename);
    _input = new ifstream( _filename );
    assert( _input->good() );

    char buf[128];
    sprintf(buf, "tail -1 %s | awk '{ print($1); }' > /tmp/.%s",
	    _filename, getenv("USER"));
    system(buf);
    sprintf(buf, "/tmp/.%s", getenv("USER"));
    ifstream lf(buf);
    lf >> _finalTime;
    // _parent->sliderMax( (int)_finalTime + 1 );
    unlink(buf);
  }
}

double ov_parser::finalTime(void) const
{
  return _finalTime;
}
