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
static char const _Moniker_cc_rcsid_[] =
"$Id: Moniker.cc,v 1.1 1999/01/21 14:55:54 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include "Moniker.h"
extern "C" {
#include <string.h>
};

Moniker::Moniker(const char * string) 
  : _string(0), _current_tok(0) 
{ 
  if (string) {
    // I build my own string and keep it safe
    _string = new char [ strlen(string) + 1 ];
    strcpy(_string, string);
    _current_tok = _string;
  }
}

Moniker::Moniker(const Moniker & rhs)
  : _string(0), _current_tok(0)
{
  _string = new char [ strlen(rhs._string) + 1 ];
  strcpy(_string, rhs._string);

  // Our current token will point the same distance into the string
  if (_current_tok)
    _current_tok = _string + (rhs._current_tok - rhs._string);
  else
    _current_tok = _string;
}

Moniker::~Moniker() { delete [] _string; }

const Moniker & Moniker::operator = (const Moniker & rhs)
{
  delete [] _string;

  _string = new char [ strlen(rhs._string) + 1 ];
  strcpy(_string, rhs._string);

  // Our current token will point the same distance into the string
  _current_tok = _string + (rhs._current_tok - rhs._string);

  return *this;
}

bool Moniker::satisfied_by(const Moniker & rhs) const
{
  // INVARIANT:  the Moniker rhs is the fully specified Moniker for the Shareable

  // Let's be optimistic for once ...
  bool rval = true;

  // this Moniker may contain wildcards and multiple tokens
  char * this_token = currentToken();

  // see if the first token of this is present anywhere in the req_compl
  char * prev_token = this_token;
  clean_token(prev_token);

  const char * complete = rhs.asString();

  while ( prev_token != 0 && rval ) {
    if (complete = strstr(complete, this_token)) {
      // clean up the memory
      delete [] this_token;

      prev_token = this_token = ((Moniker *)this)->nextToken();
      clean_token(prev_token);
    } else {
      // didn't find it, so we failed
      rval = false;
      delete [] this_token;
    }
  }
  // this resets the internal nextToken counter so others may use it
  ((Moniker *)this)->reset();
  return rval;
}

int Moniker::difference(const Moniker & req, int tind) const
{
  // INVARIANT:  the Moniker rhs is the fully specified Moniker for the Shareable

  int rval = 0;
  // this pulls the nth token off the end of this Moniker
  char * token = ((Moniker *)this)->reverseToken(tind); 
  if (!token)
    return -1; // Something got fookered

  const char * complete = req.asString();

  // perform a reverse strstr on complete for token
  const char * ptr = complete;
  while (ptr = strstr(ptr, token)) {
    if (!(rval = ptr - complete))
      break;
  }
  // clean up the memory
  delete [] token;
  return rval;
}

// Moniker::operator const char * (void) const
// { 
//   return asString();
// }

const char * Moniker::asString(void) const
{
  return _string;
}

char * Moniker::remainder(void) const
{
  char * rval = 0;

  if (_current_tok) {
    rval = new char [ strlen(_current_tok) + 1 ];
    strcpy(rval, _current_tok);
  }
  return rval;
}

char * Moniker::nextToken(void)
{
  char * rval = 0;

  // move _current_tok forward in memory
  _current_tok = strchr(_current_tok, '.');
  if (_current_tok) {
    _current_tok++; // get over the period
  
    rval = currentToken();
  }

  return rval;
}

char * Moniker::currentToken(void) const
{
  char * rval = 0;
  if (_current_tok) {
    char * tmp = strchr(_current_tok, '.');
    int len = strlen(_current_tok);  // remainder of buffer is default length
    if (tmp) len = tmp - _current_tok;

    rval = new char [ len + 1 ];
    strncpy(rval, _current_tok, len);
    // do we need to forcefully terminate rval with a null char?
    rval[len] = '\0';
  }
  return rval;
}

char * Moniker::reverseToken(int index)
{
  char * rval = 0;

  if (index > 0) {
    // work our way forward from the end of the string
    char * tmp = _string + strlen(_string);

    int n = 0, i = 0;
    while ( n != index && tmp ) {
      if (*(--tmp) == '.')
	n++;
      i++;

      // This is necessary for names that do not contain periods
      if (i == strlen(_string)) {
	n++;
	break;
      }
    }
    if (tmp) {
      if (tmp[0] == '.')
	tmp++; // get over the period
      char * old_ptr = _current_tok;
      _current_tok = tmp;
      rval = currentToken();
      _current_tok = old_ptr;
    }
  }
  return rval;
}

char * Moniker::identifier(void)
{
  return reverseToken(1);
}

void Moniker::reset(void) 
{ 
  _current_tok = _string; 
}

void Moniker::removeIdentifier(void)
{
  char * tmp = _string + strlen(_string);

  if (! strchr(tmp, '.')) {
    delete [] _string;
    _string = _current_tok = 0;
  } else {
    while ( *tmp != '.' )
      tmp--;

    if (_string - tmp > 0) {
      char * new_str = new char [ _string - tmp + 1 ];
      strncpy(new_str, _string, (_string - tmp));
      new_str [ _string - tmp ] = 0;

      delete [] _string;
      _string = _current_tok = new_str;
    }
  }
}

bool Moniker::isNull(void) const
{
  if (!_string)
    return true;
  return false;
}

void Moniker::prepend(const Moniker & rhs)
{
  int new_len = 1;
  if (_string)
    new_len += strlen(_string);
  if (rhs._string)
    new_len += strlen(rhs._string);

  char * new_str = new char [ new_len ];

  if (rhs._string)
    strcpy(new_str, rhs._string);
  if (_string) {
    strcpy(new_str + strlen(rhs._string), _string);
    delete [] _string;
  }
  _string = new_str;
}

void Moniker::append(const Moniker & rhs)
{
  int new_len = strlen(_string) + strlen(rhs._string) + 1;

  char * new_str = new char [ new_len ];

  strcpy(new_str, _string);
  strcpy(new_str + strlen(_string), rhs._string);
  
  delete [] _string;
  _string = new_str;
}

void Moniker::clear(void)
{
  delete [] _string;
  _string = _current_tok = 0;
}

void clean_token(char * token)
{
  const char * tmp;
  if (token && (tmp = strchr(token, '*'))) {
    // replace first asterik in token with end of string terminator
    // so we can strstr with it later
    token[ tmp - token ] = '\0';
  }
}
