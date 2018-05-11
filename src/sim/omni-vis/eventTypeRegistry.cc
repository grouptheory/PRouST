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
static char const _eventTypeRegistry_cc_rcsid_[] =
"$Id: eventTypeRegistry.cc,v 1.13 1999/02/16 19:52:39 mountcas Exp $";
#endif

#include "eventTypeRegistry.h"
#include <ostream.h>
#include <common/cprototypes.h>

//---------------------------------------------------------------

eventTypeRegistry* eventTypeRegistry::_singleton = 0;

eventTypeRegistry::eventTypeRegistry(void) 
{
  assert(!_singleton);
  _singleton = this;
  setSelectedAll( false );
  _state[ type2index(eventTypeRegistry::All_type) ] = true;
}

eventTypeRegistry::~eventTypeRegistry() 
{
  assert(_singleton);
  _singleton=0;
}

eventTypeRegistry& theEventTypeRegistry(void) 
{
  assert(eventTypeRegistry::_singleton);
  return *(eventTypeRegistry::_singleton);
}

int eventTypeRegistry::type2index(event_type t) const 
{
  return (int)t - _FIRST_EVENT_TYPE;
}

int eventTypeRegistry::isSelected(event_type t) const 
{
  if ( _state[ type2index(eventTypeRegistry::All_type) ] )
    return 1;
  if ( _state[ type2index(eventTypeRegistry::None_type) ] ) 
    return 0;
  int ind = type2index(t);
  int val = _state[ind];
  return( val );
}

void eventTypeRegistry::setSelected(event_type t, int state) 
{
  if ((t==eventTypeRegistry::All_type)||
      (t==eventTypeRegistry::None_type)) {
    setSelectedAll( 0 );
  }
  else {
    _state[ type2index(eventTypeRegistry::All_type) ]  = false;
    _state[ type2index(eventTypeRegistry::None_type) ] = false;
  }
  _state[ type2index(t) ] = state;
}

void eventTypeRegistry::setSelectedAll(int state) 
{
  for (int i=0; i<_NUM_EVENT_TYPES; i++) {
    _state[i] = state; 
  }
}

eventTypeRegistry::event_type 
eventTypeRegistry::str2type(const char * str) const 
{

  if ( !strcmp(str,"Call_Submission") )  return Call_Submission_type;
  if ( !strcmp(str,"Call_Arrival") )     return Call_Arrival_type;
  if ( !strcmp(str,"Call_Admission") )   return Call_Admission_type;
  if ( !strcmp(str,"Call_Rejection") )   return Call_Rejection_type;
  if ( !strcmp(str,"DTL_Expansion") )    return DTL_Expansion_type;
  if ( !strcmp(str,"Crankback") )        return Crankback_type;
  if ( !strcmp(str,"I_am_PGL") )         return I_am_PGL_type;
  if ( !strcmp(str,"Voted_Out") )        return Voted_Out_type;
  if ( !strcmp(str,"Lost_Election") )    return Lost_Election_type;
  if ( !strcmp(str,"Start_Elections") )  return Start_Election_type;
  if ( !strcmp(str,"Hlink_Aggr") )       return Hlink_Aggr_type;
  if ( !strcmp(str,"Hlink_Deaggr") )     return Hlink_Deaggr_type;
  if ( !strcmp(str,"Induced_Uplink") )   return Induced_Uplink_type;
  if ( !strcmp(str,"NSP_Aggr") )         return NSP_Aggr_type;
  if ( !strcmp(str,"Hello_Up") )         return Hello_Up_type;
  if ( !strcmp(str,"Hello_Down") )       return Hello_Down_type;
  if ( !strcmp(str,"NP_Exchanging") )    return NP_Exchanging_type;
  if ( !strcmp(str,"NP_Full") )          return NP_Full_type;
  if ( !strcmp(str,"NP_Down") )          return NP_Down_type;
  if ( !strcmp(str,"Switch_Up") )        return Switch_Up_type;
  if ( !strcmp(str,"Switch_Down") )      return Switch_Down_type;
  if ( !strcmp(str,"Local_Synchrony") )  return Local_Synchrony_type;
  if ( !strcmp(str,"Local_Discord") )    return Local_Discord_type;
  if ( !strcmp(str,"Global_Synchrony") ) return Global_Synchrony_type;
  if ( !strcmp(str,"Global_Discord") )   return Global_Discord_type;

  cerr << "UNKNOWN event type '" << str << "' please update eventTypeRegistry::str2type" << endl;
  abort( );
}

void eventTypeRegistry::type2str(event_type t, char * str) const 
{
  switch (t) {
  case All_type:              strcpy(str,"All");              break;
  case None_type:             strcpy(str,"None");             break;
  case Call_Submission_type:  strcpy(str,"Call_Submission");  break;
  case Call_Arrival_type:     strcpy(str,"Call_Arrival");     break;
  case Call_Admission_type:   strcpy(str,"Call_Admission");   break;
  case Call_Rejection_type:   strcpy(str,"Call_Rejection");   break;
  case DTL_Expansion_type:    strcpy(str,"DTL_Expansion");    break;
  case Crankback_type:        strcpy(str,"Crankback");        break;
  case I_am_PGL_type:         strcpy(str,"I_am_PGL");         break;
  case Voted_Out_type:        strcpy(str,"Voted_Out");        break;
  case Lost_Election_type:    strcpy(str,"Lost_Election");    break;
  case Start_Election_type:   strcpy(str,"Start_Elections");  break;
  case Hlink_Aggr_type:       strcpy(str,"Hlink_Aggr");       break;
  case Hlink_Deaggr_type:     strcpy(str,"Hlink_Deaggr");     break;
  case Induced_Uplink_type:   strcpy(str,"Induced_Uplink");   break;
  case NSP_Aggr_type:         strcpy(str,"NSP_Aggr");         break;
  case Hello_Up_type:         strcpy(str,"Hello_Up");         break;
  case Hello_Down_type:       strcpy(str,"Hello_Down");       break;
  case NP_Full_type:          strcpy(str,"NP_Full");          break;
  case NP_Exchanging_type:    strcpy(str,"NP_Exchanging");    break;
  case NP_Down_type:          strcpy(str,"NP_Down");          break;
  case Switch_Up_type:        strcpy(str,"Switch_Up");        break;
  case Switch_Down_type:      strcpy(str,"Switch_Down");      break;
  case Local_Synchrony_type:  strcpy(str,"Local_Synchrony");  break;
  case Local_Discord_type:    strcpy(str,"Local_Discord");    break;
  case Global_Synchrony_type: strcpy(str,"Global_Synchrony"); break;
  case Global_Discord_type:   strcpy(str,"Global_Discord");   break;
  case Unknown_type:
  default:
    strcpy(str, "Unknown");
    cerr << "UNKNOWN event type!  eventTypeRegistry::type2str" << endl;
    break;
  }
}

void eventTypeRegistry::dumpSelections(void) const 
{
  for (int i = 0; i < _NUM_EVENT_TYPES; i++) {
    cout << "state["<<i<<"] = " << _state[i] << endl;
  }
}

void eventTypeRegistry::event2color(char * str, event_type t) const 
{
  switch (t) {
  case Call_Submission_type:  strcpy(str,"orange");           break;
  case Call_Arrival_type:     strcpy(str,"orange");           break;

  case Call_Admission_type:   strcpy(str,"green");            break;
  case Call_Rejection_type:   strcpy(str,"red");              break;

  case DTL_Expansion_type:    strcpy(str,"blue");             break;
  case Crankback_type:        strcpy(str,"magenta");          break;

  case I_am_PGL_type:         strcpy(str,"purple");           break;
  case Voted_Out_type:        strcpy(str,"brown");            break;
  case Lost_Election_type:    strcpy(str,"brown");            break;
  case Start_Election_type:   strcpy(str,"blue");             break;

  case Hlink_Aggr_type:       strcpy(str,"lightyellow");      break;
  case Hlink_Deaggr_type:     strcpy(str,"darkyellow");       break;
  case Induced_Uplink_type:   strcpy(str,"cyan");             break;
  case NSP_Aggr_type:         strcpy(str,"yellow");           break;

  case Hello_Up_type:         strcpy(str,"lightgreen");       break;
  case Hello_Down_type:       strcpy(str,"turquoise");        break;

  case NP_Exchanging_type:    strcpy(str,"lightblue");        break;
  case NP_Full_type:          strcpy(str,"cyan");             break;
  case NP_Down_type:          strcpy(str,"darkblue");         break;

  case Switch_Up_type:        strcpy(str,"white");            break;
  case Switch_Down_type:      strcpy(str,"white");            break;

    // for local/global synchrony and discord it would be nice 
    // to change the color of ALL the nodes affected ...
  case Local_Synchrony_type:  strcpy(str, "blue");            break;
  case Local_Discord_type:    strcpy(str, "yellow");          break;
  case Global_Synchrony_type: strcpy(str, "green");           break;
  case Global_Discord_type:   strcpy(str, "red");             break;

  default:
    cerr << "UNKNOWN event type!  eventTypeRegistry::event2color" << endl;
    abort();
    break;
  }  
}
