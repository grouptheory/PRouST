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
#ifndef __EVENTTYPEREGISTRY_H__
#define __EVENTTYPEREGISTRY_H__

#ifndef LINT
static char const _ov_eventtyperegistry_h_rcsid_[] =
"$Id: eventTypeRegistry.h,v 1.11 1999/02/24 21:55:55 mountcas Exp $";
#endif

class Event;

//---------------------------------------------------------------
class eventTypeRegistry {
  friend class ov_main;
  friend eventTypeRegistry& theEventTypeRegistry(void);
public:

  enum event_type 
  {
    Unknown_type          =  0,
    First_type_sentinel   =  1000,
    All_type              ,
    None_type             ,
    Call_Submission_type  ,
    Call_Arrival_type     ,
    Call_Admission_type   ,
    Call_Rejection_type   ,
    DTL_Expansion_type    ,
    Crankback_type        ,    // don't put explicit
    I_am_PGL_type         ,    // numbers here
    Voted_Out_type        ,
    Lost_Election_type    ,
    Start_Election_type   ,
    Hlink_Aggr_type       ,
    Hlink_Deaggr_type     ,
    Induced_Uplink_type   ,
    NSP_Aggr_type         ,
    Hello_Up_type         ,
    Hello_Down_type       ,
    NP_Exchanging_type    ,
    NP_Full_type          ,
    NP_Down_type          ,
    Switch_Up_type        ,
    Switch_Down_type      ,
    Local_Synchrony_type  ,
    Local_Discord_type    ,
    Global_Synchrony_type ,
    Global_Discord_type   ,
    Last_type_sentinel         // all future additions to the type
  };                           // list must be placed before 
                               // the Last_type_sentinel

  static const int _FIRST_EVENT_TYPE      = (int)First_type_sentinel + 1;
  static const int _LAST_EVENT_TYPE       = (int)Last_type_sentinel  - 1;
  static const int _NUM_EVENT_TYPES = _LAST_EVENT_TYPE - _FIRST_EVENT_TYPE + 1;

  int type2index(event_type t) const;
  void type2str(event_type t, char* str) const;
  event_type str2type(const char* str) const;

  int isSelected(event_type t) const;

  void filterEvents(void);
  void dumpSelections(void) const;

  void event2color(char* str, event_type t) const;

private:

  void setSelected(event_type t, int state);
  void setSelectedAll(int state);

  eventTypeRegistry(void);
  ~eventTypeRegistry();

  int _state [_NUM_EVENT_TYPES];

  static eventTypeRegistry * _singleton;
};

#endif // __EVENTTYPEREGISTRY_H__
