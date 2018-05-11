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

#ifndef LINT
static char const _HelpSearchDlg_cc_rcsid_[] =
"$Id: HelpSearchDlg.cc,v 1.2 1999/01/25 13:21:59 mountcas Exp $";
#endif
// -*- C++ -*-
#include "HelpSearchDlg.h"
#include "v_app.h"
#include <common/cprototypes.h>

#include <qchkbox.h>
#include <qpushbt.h>
#include <qlabel.h>
#include <qlined.h>

HelpSearchDlg::HelpSearchDlg(QWidget * parent, 
			     const char * name,
			     v_app * app)
  : QDialog(parent, name, TRUE), _data(app)
{
  this->resize(365, 155);

  _searchLbl = new QLabel(this);
  assert( _searchLbl != 0 );
  _searchLbl->setGeometry(15, 15, 50, 25);
  _searchLbl->setText("Search:");
  _searchLbl->setAlignment(33);

  _searchTextLinEdt = new QLineEdit(this);
  assert( _searchTextLinEdt != 0 );
  _searchTextLinEdt->setGeometry(65, 10, 285, 30);

  _searchBut = new QPushButton(this);
  assert( _searchBut != 0 );
  _searchBut->setGeometry(70, 105, 100, 30);
  _searchBut->setText("Search");
  connect( _searchBut, SIGNAL(clicked()), SLOT(searchClicked()) );

  _cancBut = new QPushButton(this);
  assert( _cancBut != 0 );
  _cancBut->setGeometry(220, 105, 100, 30);
  _cancBut->setText("Cancel");
  connect( _cancBut, SIGNAL(clicked()), SLOT(cancelClicked()) );

  _caseChkBox = new QCheckBox(this);
  assert( _caseChkBox != 0 );
  _caseChkBox->setGeometry(15, 45, 100, 30);
  _caseChkBox->setText("Case Sensitive");
}

HelpSearchDlg::~HelpSearchDlg()
{
  // It seems as though Qt does its own memory management
  //    and freaks out when I delete these ...
#if 0
  delete _searchLbl;
  delete _searchTextLinEdt;
  delete _searchBut;
  delete _cancBut;
  delete _caseChkBox;
#endif
}

void HelpSearchDlg::searchClicked( )
{
  hide();
  // tell the editor to search for a specific string
  _data->searchHelp( _searchTextLinEdt->text() );
}

void HelpSearchDlg::cancelClicked( )
{
  hide();
}
