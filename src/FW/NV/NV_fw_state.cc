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
static char const _NV_fw_state_cc_rcsid_[] =
"$Id: NV_fw_state.cc,v 1.1 1999/01/20 22:57:21 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include "NV_fw_state.h"
#include "NV_application.h"
#include "NV_visitor.h"
#include "NV_conduit.h"
#include "graphfield.h"
#include <FW/kernel/Kernel_NV_Synch.h>

extern "C" {
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
};

void strip_plus(char * n);

NV_fw_state * NV_fw_state::_singleton = 0;

//-----------------------------------------------------
NV_fw_state * FWState(void) 
{
  if (!NV_fw_state::_singleton) 
    NV_fw_state::_singleton = new NV_fw_state();
  return NV_fw_state::_singleton;
}

int NV_fw_state::parse_and_update(char * cmd) 
{
  int rval = PARSE_NOERR;

  if (!*cmd) return rval;
  else if (GetApp().IsLive()) cout << "NV: read '" << cmd << "'" << endl;

  bool cont = GetApp().GetMainWin()->continuing();
  int numscanned = sscanf(cmd, _scan_str, _chunk[0], _chunk[1], _chunk[2],
			  _chunk[3], _chunk[4], _chunk[5], _chunk[6], _chunk[7]);
  if (numscanned < 0)
    return rval;
  
  char * patch;

  if (_chunk[0][1] == '#' || _chunk[0][0] == '#' )
    return rval; // comment;

  switch (_chunk[0][1]) {
    case '*': // token
      patch = strchr(_chunk[0],'\n');
      if (patch)
	*patch=0;

      assert (strcmp(_chunk[0],TOKEN_KERNEL_TO_NV)==0);

      cout << "NV: Got the token from the kernel...\n";
      rval = PARSE_TOKEN_FOUND;
      break;

    case 'b': // birth
      switch (_chunk[2][1]) {
        case 'V': // Visitor 
	  { // !birth ConnectorVisitor:5 !Visitor !in Router:7+
	    strip_plus( _chunk[4] );
	    // _chunk[4][ strlen( _chunk[4] ) - 1 ] = '\0';  // Gets rid of the + at the end
	    NV_conduit * location = _conduits_registry->query(_chunk[4]);
	    NV_visitor * vis = _visitor_registry->demand(ExtractName(_chunk[1]), ExtractType(_chunk[1]));
	    if (!make_visitor_at_conduit(location, vis))
	      cerr << "ERROR: Unable to place '" << _chunk[1] << "' at '" << _chunk[4] << "'" << endl;
	    break;
	  }
        default:
	  {
	    // Remove the annoying + sign at the end of the name
	    strip_plus( _chunk[1] );
	    // _chunk[1][ strlen( _chunk[1] ) - 1 ] = '\0';
	    NV_conduit * nc = _conduits_registry->demand(_chunk[1], (_chunk[2] + 1));
	    if (!_current_expanders.empty()) {
	      NV_conduit * exp = _current_expanders.pop();
	      // Add the new conduit to the current group
	      exp->add_conduit(nc);
	      if (ConduitRegistry()->GetFlags() & NV_conduit_registry::HighestVisible) {
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
	strip_plus( _chunk[1] );
	strip_plus( _chunk[4] );
	//	_chunk[1][ strlen( _chunk[1] ) - 1 ] = '\0';
	//	_chunk[4][ strlen( _chunk[4] ) - 1 ] = '\0';
	NV_conduit * left = _conduits_registry->query(_chunk[1]),
	           * right = _conduits_registry->query(_chunk[4]);
	if (!connect_conduits(left, right, 
			      (NV_conduit::WhichSide)atoi( _chunk[2] ), 
			      (NV_conduit::WhichSide)atoi( _chunk[5] )))
	  cerr << "ERROR: Unable to connect '" << _chunk[1] << "' and '" << _chunk[4] << "'" << endl;
	break;
      }
    case 'a': // add 
      {
	// Remove the annoying + sign at the end of the name
	strip_plus( _chunk[1] );
	strip_plus( _chunk[3] );
	//	_chunk[1][ strlen( _chunk[1] ) - 1 ] = '\0';
	//	_chunk[3][ strlen( _chunk[3] ) - 1 ] = '\0';
	NV_conduit * component = _conduits_registry->query(_chunk[1]),
	           * composite = _conduits_registry->query(_chunk[3]);
	// This is define'd out currently because the FW library prints !add twice
	if (!composite || !composite->add_conduit(component)) {
	  cerr << "ERROR: Unable to add '";
	  if (component) cerr << component->GetName(); else cerr << "(null)";
	  cerr << "' to '";
	  if (composite) cerr << composite->GetName(); else cerr << "(null)";
	  cerr << "'." << endl; 
	}
	break;
      }
    case 'r': // received
      {
	strip_plus( _chunk[3] );
	//	_chunk[3][ strlen( _chunk[3] ) - 1 ] = '\0';
	NV_visitor * vis  = _visitor_registry->query(ExtractName(_chunk[1]), ExtractType(_chunk[1]));
	if (vis) {
	  NV_conduit * last = vis->CurrentLocation(),
	             * dest = _conduits_registry->query(_chunk[3]);
	  if (!move_visitor_between_conduits(last, dest, vis)) {
	    cerr << "ERROR: Unable to move '" << vis->GetName() << "' from '";
	    if (last) cerr << last->GetName(); else cerr << "(null)";
	    cerr << "' to '";
	    if (dest) cerr << dest->GetName(); else cerr << "(null)";
	    cerr << "'" << endl;
	  }
	} else
	  cerr << "ERROR: Unable to locate visitor '" << _chunk[1] << "'" << endl;
	break;
      }
    case 'd': // death OR disconnect ?
      if (!strcmp("death", _chunk[0] + 1)) {
	if (_chunk[2][1] == 'V') {
	  // !death ConnectorVisitor:5 !Visitor !in PortMux:0+
	  NV_visitor * vis = _visitor_registry->query(ExtractName(_chunk[1]));
	  strip_plus( _chunk[4] );
	  //	_chunk[4][ strlen( _chunk[4] ) - 1 ] = '\0';
	  NV_conduit * loc = _conduits_registry->query(_chunk[4]);
	  if (!loc || !vis || !kill_visitor_at_conduit(loc, vis)) {
	    cerr << "ERROR: Unable to kill '";
	    if (vis) cerr << vis->GetName(); else cerr << "(null)";
	    cerr << "' in '";
	    if (loc) cerr << loc->GetName(); else cerr << "(null)";
	    cerr << "'" << endl;
	  }
	} else {
	  // Conduit death
	  strip_plus( _chunk[1] );
	  //	_chunk[1][ strlen( _chunk[1] ) - 1 ] = '\0';
	  if (!_conduits_registry->destroy(_chunk[1]))
	    cerr << "ERROR: Unable to kill '" << _chunk[1] << "'" << endl;
	}
      } else if (!strcmp("disconnect", _chunk[0] + 1)) {
	strip_plus( _chunk[1] );
	strip_plus( _chunk[3] );
	NV_conduit * n1 = _conduits_registry->query(_chunk[1]),
	           * n2 = _conduits_registry->query(_chunk[3]);
	if (!disconnect_conduits(n1, n2))
	  cerr << "Unable to disconnect '" << _chunk[1] << "' from '" << _chunk[3] << "'." << endl;
      }
      break;
    case 't': // !time
      {
	double t = atof(_chunk[1]);
	if (!push_time(t))
	  cerr << "ERROR: Unable to push time forward to '" << t << "'" << endl;
	break;
      }
    case 's': // !start
      {
	strip_plus( _chunk[1] );
	NV_conduit * exp = _conduits_registry->demand(_chunk[1], "Cluster");
	if (exp) {
	  if (!_current_expanders.empty()) {
	    NV_conduit * tmp = _current_expanders.pop();
	    tmp->add_conduit(exp);
	    if (ConduitRegistry()->GetFlags() & NV_conduit_registry::HighestVisible) {
	      exp->SetVisible(true);
	    } else
	      exp->SetVisible(false);
	    _current_expanders.push(tmp);
	  } else {
	    if (ConduitRegistry()->GetFlags() & NV_conduit_registry::HighestVisible) {
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
	if (!strcmp(_chunk[0] + 1, "end")) {
	  NV_conduit * rval = _current_expanders.pop();
	  if (strcmp(_chunk[1], rval->GetName()))
	    cerr << "ERROR:  end block '" << _chunk[1] << "' doesn't "
		 << "match Expander '" << rval->GetName() << "' on top of stack!" << endl;
	} else if (!strcmp(_chunk[0] + 1, "event")) {
	  if (eventNumber(cmd) == _next_event) {
	    _searching_for_event = false;
	    _next_event = -1;
	    cont = false;
	  }
	}
      }
      break;
    case 'n': // namechange
      strip_plus( _chunk[1] );
      strip_plus( _chunk[3] );
      //      _chunk[1][ strlen( _chunk[1] ) - 1 ] = '\0';
      //      _chunk[3][ strlen( _chunk[3] ) - 1 ] = '\0';
      if (!_conduits_registry->namechange(_chunk[1], _chunk[3]))
	cerr << "ERROR: Unable to change name from '" << _chunk[1] << "' to '" << _chunk[3] 
	     << "' since '" << _chunk[1] << "' is no longer in the registry." << endl;
      break;
    case 'x': // stop condition
      ConduitRegistry()->SetFlag( NV_conduit_registry::ViewWhileContinue );
      cont = false;
      _read_xxx = true;
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
  // No drawing until we've read in the xxx line
  if (!GetApp().IsLive() && _read_xxx && !_searching_for_event) {
    // force a repaint if stepping ...
    if (!cont || (ConduitRegistry()->GetFlags() & NV_conduit_registry::ViewWhileContinue))
      GetApp().GetMainWin()->repaint(FALSE);
  }
  return rval;
}

bool NV_fw_state::push_time(double t)  
{
  bool rval = false;
  if (_simtime <= t) {
    _simtime = t;
    rval = true;
    GetApp().GetMainWin()->emit_time(_simtime);
  }
  return rval;
}

NV_fw_state::NV_fw_state(void) 
  : _simtime(0.0), _read_xxx(false), 
    _searching_for_event(false)
{
  _visitor_registry      = VisitorRegistry();
  _visitor_colors_table  = VisitorColorsTable();
  _conduits_registry     = ConduitRegistry();
  _conduits_pixmap_table = ConduitPixmapsTable();

  strcpy(_scan_str, "");
  for (int i = 0; i < _MAX_WORDS; i++) 
    strcat(_scan_str,"%s ");
}

NV_fw_state::~NV_fw_state() 
{
  delete _visitor_registry;
  delete _visitor_colors_table;
  delete _conduits_registry;
  delete _conduits_pixmap_table;
}

bool NV_fw_state::readXXX(void) const { return _read_xxx; }

bool NV_fw_state::searchingForEvent(void) const
{
  return _searching_for_event;
}

void NV_fw_state::searchForEvent(int event)
{
  if (hasEvent(event)) {
    _searching_for_event = true;
    _next_event = event;
  } else
    cerr << "There is no event number " << event << " present in the log." << endl;
}

bool NV_fw_state::hasEvent(int event)
{
  bool rval = false;

  dic_item di;
  forall_items(di, _event_log) {
    const char * tmp = _event_log.key(di);
    if (event == eventNumber(tmp)) {
      rval = true;
      break;
    }
  }
  return rval;
}

void NV_fw_state::addEvent(const char * line)
{
  char * tmp = new char [ strlen(line) + 1 - strlen("!event ") ];
  strncpy(tmp, line + strlen("!event "), strlen(line) - strlen("!event "));
  tmp[ strlen(line) - strlen("!event ") ] = 0;
  _event_log.insert(tmp, false);
}

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
