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
static char const _v_parser_cc_rcsid_[] =
"$Id: v_parser.cc,v 1.2 1999/02/09 17:19:39 mountcas Exp $";
#endif

#include "v_parser.h"
#include "v_conduit.h"
#include "v_visitor.h"
#include "v_main.h"
#include "v_app.h"
#include <FW/kernel/Kernel_NV_Synch.h>

#include <common/cprototypes.h>
#include <iostream.h>
#include <fstream.h>

void strip_plus(char * n);
char * ExtractType(const char * n);
char * ExtractName(const char * n);
int eventNumber(const char * event);

const int v_parser::_MAX_WORDS = 8;

v_parser::v_parser(v_data * d, int argc, char ** argv) 
 : _data(d), _inFile(0), _filename(0), _continuing(false),
   _finalTime(-1.0)
{ 
  // parse the command line looking for a possible input file
  if (argc > 1)
    restart( argv[1] );

  strcpy(_scan_str, "");
  for (int i = 0; i < _MAX_WORDS; i++) 
    strcat(_scan_str,"%s ");
}

v_parser::~v_parser() 
{ 
  delete [] _filename;
  if (_inFile) 
    _inFile->close();
  delete _inFile;
}

bool v_parser::continuing(void) const
{
  return _continuing;
}

void v_parser::continuing(bool c)
{
  _continuing = c;
}

const char * v_parser::input(void) const
{
  return _filename;
}

void v_parser::restart(const char * filename)
{
  if (filename) {
    if (_filename)
      delete [] _filename;
    _filename = new char [ strlen(filename) + 1 ];
    strcpy(_filename, filename);
  }
  if (_inFile)
    _inFile->close();

  delete _inFile;
  _inFile = new ifstream( filename );

  // find the last time
  char buf[128];
  sprintf(buf, "grep \"\\!time\" %s | tail -1 | cut -f2 -d\" \" > /tmp/%s", 
	  _filename, getenv("USER"));
  system(buf);
  sprintf(buf, "/tmp/%s", getenv("USER"));
  ifstream tmp(buf);
  assert(tmp.good());
  tmp >> _finalTime;
  theApplication().notify( _finalTime );
  unlink(buf);
}

int v_parser::parse(void)
{
  char cmd[512], chunk[8][256];
  int  rval = NO_ERR;

  assert( _inFile && _inFile->good() );

  _inFile->getline(cmd, 512);

  int numscanned = sscanf(cmd, _scan_str, 
			  chunk[0], chunk[1], chunk[2],
                          chunk[3], chunk[4], chunk[5], 
			  chunk[6], chunk[7]);

  if (numscanned > 0 && chunk[0][1] != '#' && chunk[0][0] != '#' ) {
    switch (chunk[0][1]) 
      {
      case '*': // token
	{
	  char * patch = strchr(chunk[0],'\n');
	  if (patch)
	    *patch = 0;
	  
	  assert (strcmp(chunk[0], TOKEN_KERNEL_TO_NV) == 0);
	  
	  cout << "NV: Got the token from the kernel...\n";
	  rval = PARSE_TOKEN_FOUND;
	}
        break;

      case 'b': // birth
	switch (chunk[2][1]) 
	  {
	  case 'V': // Visitor 
	    { // !birth ConnectorVisitor:5 !Visitor !in Router:7+
	      strip_plus( chunk[4] );
	      // chunk[4][ strlen( chunk[4] ) - 1 ] = '\0';  // Gets rid of the + at the end
	      v_conduit * location = ConduitRegistry()->query(chunk[4]);
	      v_visitor * vis = VisitorRegistry()->demand(ExtractName(chunk[1]),
							  ExtractType(chunk[1]));
	      if (!make_visitor_at_conduit(location, vis))
		cerr << "ERROR: Unable to place '" << chunk[1] << "' at '" << chunk[4] << "'" << endl;
	      else
		theApplication().centralWidget()->notify(vis);
	      break;
	    }
	  default:
	    {
	      // Remove the annoying + sign at the end of the name
	      strip_plus( chunk[1] );
	      // chunk[1][ strlen( chunk[1] ) - 1 ] = '\0';
	      v_conduit * nc = ConduitRegistry()->demand(chunk[1], (chunk[2] + 1));
	      if (!_current_expanders.empty()) {
		v_conduit * exp = _current_expanders.pop();
		// Add the new conduit to the current group
		exp->add_conduit(nc);
		if (ConduitRegistry()->GetFlags() & v_conduit_registry::HighestVisible) {
		  // make the new conduit invisible since it's parent is visible
		  nc->SetVisible(false);
		} else {
                  nc->SetVisible(true);
		}
		// push the group back on the stack
		_current_expanders.push(exp);
	      }
	    } 
	    break;
	  }
	  break;
        case 'c': // connected 
	  { // !connected VC-alloc-Factory:21+ 2 !to CREF-LowerMux:22+ 2
	    // Remove the annoying + sign at the end of the name
	    strip_plus( chunk[1] );
	    strip_plus( chunk[4] );
	    //      chunk[1][ strlen( chunk[1] ) - 1 ] = '\0';
	    //      chunk[4][ strlen( chunk[4] ) - 1 ] = '\0';
	    v_conduit * left = ConduitRegistry()->query(chunk[1]),
	      * right = ConduitRegistry()->query(chunk[4]);
	    if (!connect_conduits(left, right, 
				  (v_conduit::WhichSide)atoi( chunk[2] ), 
				  (v_conduit::WhichSide)atoi( chunk[5] )))
	      cerr << "ERROR: Unable to connect '" << chunk[1] << "' and '" << chunk[4] << "'" << endl;
	    break;
	  }
        case 'a': // add 
	  {
	    // Remove the annoying + sign at the end of the name
	    strip_plus( chunk[1] );
	    strip_plus( chunk[3] );
	    
	    v_conduit * component = ConduitRegistry()->query(chunk[1]),
	              * composite = ConduitRegistry()->query(chunk[3]);
	    // This is define'd out currently because the FW library prints !add twice
	    if (!composite || !composite->add_conduit(component)) {
	      cerr << "ERROR: Unable to add '";
	      if (component) cerr << component->GetName(); else cerr << "(null)";
	      cerr << "' to '";
	      if (composite) cerr << composite->GetName(); else cerr << "(null)";
	      cerr << "'." << endl; 
	    }
	  }
	  break;
        case 'r': // received
	  {
	    strip_plus( chunk[3] );
	    //      chunk[3][ strlen( chunk[3] ) - 1 ] = '\0';
	    v_visitor * vis  = VisitorRegistry()->query(ExtractName(chunk[1]), ExtractType(chunk[1]));
	    if (vis) {
	      v_conduit * last = vis->CurrentLocation(),
		* dest = ConduitRegistry()->query(chunk[3]);
	      if (!move_visitor_between_conduits(last, dest, vis)) {
		cerr << "ERROR: Unable to move '" << vis->GetName() << "' from '";
		if (last) cerr << last->GetName(); else cerr << "(null)";
		cerr << "' to '";
		if (dest) cerr << dest->GetName(); else cerr << "(null)";
		cerr << "'" << endl;
	      }
	    } else
	      cerr << "ERROR: Unable to locate visitor '" << chunk[1] << "'" << endl;
	    break;
	  }
        case 'd': // death OR disconnect ?
	  if (!strcmp("death", chunk[0] + 1)) {
	    if (chunk[2][1] == 'V') {
	      // !death ConnectorVisitor:5 !Visitor !in PortMux:0+
	      v_visitor * vis = VisitorRegistry()->query(ExtractName(chunk[1]));
	      strip_plus( chunk[4] );
	      //    chunk[4][ strlen( chunk[4] ) - 1 ] = '\0';
	      v_conduit * loc = ConduitRegistry()->query(chunk[4]);
	      if (!loc || !vis || !kill_visitor_at_conduit(loc, vis)) {
		cerr << "ERROR: Unable to kill '";
		if (vis) cerr << vis->GetName(); else cerr << "(null)";
		cerr << "' in '";
		if (loc) cerr << loc->GetName(); else cerr << "(null)";
		cerr << "'" << endl;
	      } else
		theApplication().centralWidget()->remove(vis);
	    } else {
	      // Conduit death
	      strip_plus( chunk[1] );
	      //    chunk[1][ strlen( chunk[1] ) - 1 ] = '\0';
	      if (!ConduitRegistry()->destroy(chunk[1]))
		cerr << "ERROR: Unable to kill '" << chunk[1] << "'" << endl;
	    }
	  } else if (!strcmp("disconnect", chunk[0] + 1)) {
	    strip_plus( chunk[1] );
	    strip_plus( chunk[3] );
	    v_conduit * n1 = ConduitRegistry()->query(chunk[1]),
	      * n2 = ConduitRegistry()->query(chunk[3]);
	    if (!disconnect_conduits(n1, n2))
	      cerr << "Unable to disconnect '" << chunk[1] << "' from '" << chunk[3] << "'." << endl;
	  }
	  break;
        case 't': // !time
	  {
	    double t = atof(chunk[1]);
	    theApplication().centralWidget()->setTime(t);
	    break;
	  }
        case 's': // !start
	  {
	    strip_plus( chunk[1] );
	    v_conduit * exp = ConduitRegistry()->demand(chunk[1], "Cluster");
	    if (exp) {
	      if (!_current_expanders.empty()) {
		v_conduit * tmp = _current_expanders.pop();
		tmp->add_conduit(exp);
		if (ConduitRegistry()->GetFlags() & v_conduit_registry::HighestVisible) {
		  exp->SetVisible(true);
		} else
		  exp->SetVisible(false);
		_current_expanders.push(tmp);
	      } else {
		if (ConduitRegistry()->GetFlags() & v_conduit_registry::HighestVisible) {
		  // This exp will be visible while his children will be invisible
		  exp->SetVisible(true);
		} else
		  exp->SetVisible(false);
	      }
	      _current_expanders.push(exp);
	    }
	    break;
	  }
        case 'e': // end
	  {
	    if (!strcmp(chunk[0] + 1, "end")) {
	      v_conduit * rval = _current_expanders.pop();
	      if (strcmp(chunk[1], rval->GetName()))
		cerr << "ERROR:  end block '" << chunk[1] << "' doesn't "
		     << "match Expander '" << rval->GetName() << "' on top of stack!" << endl;
	    } else if (!strcmp(chunk[0] + 1, "event")) {
#if 0
	      if (eventNumber(cmd) == _next_event) {
		_searching_for_event = false;
		_next_event = -1;
		cont = false;
	      }
#endif
	    }
	  }
	break;
      case 'n': // namechange
	strip_plus( chunk[1] );
	strip_plus( chunk[3] );
	//      chunk[1][ strlen( chunk[1] ) - 1 ] = '\0';
	//      chunk[3][ strlen( chunk[3] ) - 1 ] = '\0';
	if (!ConduitRegistry()->namechange(chunk[1], chunk[3]))
	  cerr << "ERROR: Unable to change name from '" << chunk[1] << "' to '" << chunk[3] 
	       << "' since '" << chunk[1] << "' is no longer in the registry." << endl;
	break;
      case 'x': // stop condition
	ConduitRegistry()->SetFlag( v_conduit_registry::ViewWhileContinue );
	_continuing = false;
	rval = PARSE_XXX_LINE;
	break;
      case '#':
      case ' ':
	// Comment, ignore
	break;
      default:  // unknown
	cerr << "ERROR: Unable to parse '" << cmd << "'" << endl;
	break;
      }
  }
  return rval;
}

// -------------------------------------------------
int eventNumber(const char * event)
{
  return atoi(ExtractName(event));
}

char * ExtractName(const char * n)
{
  static char rval[1024];
  bzero(rval, 1024);

  int i = 0, j = 0;;
  while (n[i] && n[i] != ':')
    i++;

  for (j = 0; n[i] != '\0'; j++)
    rval[j] = n[++i];

  return rval;
}

char * ExtractType(const char * n)
{
  static char rval[1024];
  bzero(rval, 1024);

  int i = 0;
  while (n[i] != ':') {
    rval[i] = n[i];
    i++;
  }
  return rval;
}

void strip_plus(char * n)
{
  if (strchr(n, '+')) {
    if (strchr(n, '+') != (n + (strlen( n ) - 1)))
      cerr << "ERROR: multiple pluses within name!" << endl;
    n[ strlen( n ) - 1 ] = '\0';
  }
}
