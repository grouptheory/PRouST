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
static char const _diag_cc_rcsid_[] =
"$Id: diag.cc,v 1.4 1999/02/10 19:22:39 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include "diag.h"
#include <fstream.h>

trienode        * diag_internals::_root_diag_tree = 0;
list<ostream *> * diag_internals::_os_list        = 0;
int               diag_internals::_flags          = 0;
char              diag_internals::_level_name[6][LEVEL_NAME_LENGTH];
bool              diag_internals::_diag_is_initialized = false;

//--------------------------------------------------------------
trienode::trienode(const char * name, int level) 
{ 
  strcpy(_name,name);
  _level = (int)level;
  _children.clear();
}

trienode::~trienode() 
{ 
  dic_item di;
  forall_items(di,_children) {
    const char* key = _children.key(di);
    delete _children.inf(di);
    free((void*)key);
  }
}

void trienode::Print(char * accum, 
		     ostream & os, 
		     int ancestor_level)
{
  char tmp[trienode::MAX_NAME_LENGTH*10];
  strcpy(tmp,accum);
  if (strcmp(_name,"") == 0) {
    ancestor_level = _level;
  }
  strcat(tmp,_name);
  strcat(tmp,"\"");
  
  char lvl[40];
  strcpy(lvl,"");
  if (!_level) 
    diag_internals::AppendLevelName(lvl,ancestor_level);
  else 
    diag_internals::AppendLevelName(lvl,_level);
  
  int col = 60-strlen(tmp)-strlen(lvl);
  for (int j=0;j<col;j++) strcat(tmp,"-");
  strcat(tmp,lvl);
  
  os << tmp << endl;
  
  dic_item di;
  forall_items(di,_children){
    strcpy(tmp,accum);
    strcat(tmp,_name);
    if (strcmp(_name,"")!=0) strcat(tmp,".");
    if (!_level) _children.inf(di)->Print(tmp, os, ancestor_level);
    else _children.inf(di)->Print(tmp, os, _level);
  }
}

//
// Give each trie node in this tree the same diag level.
//
void trienode::new_child_level(int level) 
{
  dic_item ditem;

  forall_items(ditem, _children) {
    trienode *child = _children.inf(ditem);
    child->_level = level;
    child->new_child_level(level);
  }
}


void trienode::search_and_set(const char * remaining_mask,
			      int level) 
{
  char * dot_ptr = strchr(remaining_mask,'.');
  char prefix[MAX_NAME_LENGTH];
  int prefix_length = (dot_ptr ? 
		       (dot_ptr-remaining_mask) : 
		       (strlen(remaining_mask)));
  strncpy(prefix,remaining_mask, prefix_length);
  prefix[prefix_length] = 0;

  trienode * child = 0;
  const char * tmp = prefix;
  dic_item di = _children.lookup(tmp);
  if (di) {
    child = _children.inf(di);
    // DT Feb 25 '98 - BUG always sets this "parent" level
    //    child->_level=level;

  } else {
    if (dot_ptr) child = new trienode(prefix,
				      diag_internals::DIAG_INHERITED);
    else child = new trienode(prefix, level);
    _children.insert(strdup(prefix),child);
  }

  if (dot_ptr) child->search_and_set(dot_ptr+1,level);
  else {
    //
    // There is no prefix.
    // Give all of the decendents of this trie node
    // the same diag level.
    //
    // child->_level=level;
    // child->new_child_level(level);
  }
}

int trienode::search_and_get(const char* remaining_mask) 
{
  if (!remaining_mask) return _level;

  char *dot_ptr = strchr(remaining_mask,'.');
  char prefix[MAX_NAME_LENGTH];
  int prefix_length = (dot_ptr ? 
		       (dot_ptr-remaining_mask) : 
		       (strlen(remaining_mask)));
  strncpy(prefix,remaining_mask,prefix_length);
  prefix[prefix_length]=0;

  const char * tmp = prefix;
  dic_item di = _children.lookup(tmp);
  if (di) {
    trienode* child = _children.inf(di);
    int childs_answer = (dot_ptr ? child->search_and_get(dot_ptr+1) : child->search_and_get(dot_ptr));
    if (childs_answer==diag_internals::DIAG_INHERITED) return _level;
    else return childs_answer;
  }
  else return _level;
}

//-----------------------------------------------------------------------

void diag(const char* mask, diag_internals::diag_level level, 
	  const char * fmt, ...) 
{
  int trigger_level = diag_internals::_root_diag_tree->search_and_get(mask);
  if (level <= trigger_level) {
    va_list ap;

    va_start(ap,fmt);
    vdiag(mask, level, fmt, ap);
    va_end(ap);

    if (level == DIAG_FATAL) {
      list_item li;
      forall_items(li, *diag_internals::_os_list) {
	ostream * os = diag_internals::_os_list->inf(li);
	*(os) << "\nFatal Error: Terminating." << endl;
      }
      abort();
    }
  } // else keep quiet.
}


void vdiag(const char* mask, diag_internals::diag_level level, 
	   const char * fmt, va_list ap) 
{
  char buffer[diag_internals::BUF_SIZ],
       header[diag_internals::BUF_SIZ];
  buffer[0] = header[0] = '\0';

  if (diag_internals::_flags & SHOW_MOD || 
      diag_internals::_flags & SHOW_LEV) {

    if (diag_internals::_flags & SHOW_MOD)
      sprintf(header, "%s", mask);
    if (diag_internals::_flags & SHOW_LEV)
      sprintf(header, "%s%s(%s)", header, (!*header ? "" : " "), 
	      diag_internals::_level_name[level]);
    strcat(header, ": ");
  }
  vsprintf(buffer, fmt, ap);
  
  list_item li;
  forall_items(li, *diag_internals::_os_list) {
    ostream * os = diag_internals::_os_list->inf(li);
    if (os->good()) {
      // ostream cp;
      *os << flush << header << buffer << flush;
    }

    if (os->bad()) {
      printf("--DIAG-ERROR--BAD OSTREAM-- %s %s\n",header,buffer);
    }
  }
}


void DiagLevel(char * mask, diag_internals::diag_level newlevel)
{
  // Added 7-15-98 - mountcas
  if (newlevel == DIAG_ENV)
    newlevel = NameToLevel( getenv( mask ) );

  diag_internals::_root_diag_tree->search_and_set(mask, newlevel);
}

void DiagLevel(diag_internals::diag_level level)
{
  DiagLevel("", level);
}

diag_internals::diag_level DiagLevel(const char * mask)
{
  return (diag_internals::diag_level)
    diag_internals::_root_diag_tree->search_and_get(mask);
}

const ostream & DiagStream(ostream & os)
{
  return diag_internals::SetOutput(os);  
}

void DiagFlags(int flags)
{
  diag_internals::_flags = flags;
}


int DiagFlags(void){
  return diag_internals::_flags;
}


void DiagSetFlag(int flags){
  int oldf = diag_internals::_flags;
  diag_internals::_flags |= flags;
}


void DiagClrFlag(int flags){
  int oldf = diag_internals::_flags;
  diag_internals::_flags &= ~flags;
}


void PrintDiagMIB(ostream & os){
  int ancestor_level=0;
  diag_internals::_root_diag_tree->Print("Diag-MIB >   \"",os, ancestor_level);
}

void diag_internals::AppendLevelName(const char* str, int level) {
  strcat((char*)str,diag_internals::_level_name[level]);
}

diag_internals::diag_internals(void) { }
diag_internals::~diag_internals(void) { }

const ostream & diag_internals::SetOutput(ostream & os) 
{
  if (os.good()) {
    if (!_os_list->search( &os ))
      _os_list->append( &os );
  }
  return os;
}

void InitDiag(void) 
{
  if (diag_internals::_diag_is_initialized) return;

  diag_internals::_root_diag_tree = new trienode("", DIAG_FATAL);
  diag_internals::_os_list = new list<ostream *> ;
  diag_internals::_os_list->append( &cout );
  diag_internals::_flags = SHOW_ALL;
  
  strcpy(diag_internals::_level_name[0],"Inherited");
  strcpy(diag_internals::_level_name[1],"Fatal");
  strcpy(diag_internals::_level_name[2],"Error");
  strcpy(diag_internals::_level_name[3],"Warning");
  strcpy(diag_internals::_level_name[4],"Info");
  strcpy(diag_internals::_level_name[5],"Debug");

  diag_internals::_diag_is_initialized = true;
}

void FreeDiag(void) 
{
  delete diag_internals::_root_diag_tree;
  diag_internals::_diag_is_initialized=false;
}

diag_internals::diag_level NameToLevel(const char * name)
{
  diag_internals::diag_level rval = DIAG_DEBUG;

  // a null ptr is death to strcasecmp
  if (!name) return rval;

  if (!strcasecmp(name, "FATAL"))
    rval = DIAG_FATAL;
  else if (!strcasecmp(name, "ERROR"))
    rval = DIAG_ERROR;
  else if (!strcasecmp(name, "WARNING"))
    rval = DIAG_WARNING;
  else if (!strcasecmp(name, "INFO"))
    rval = DIAG_INFO;
  // else rval is set to debug

  return rval;
}

void loadDiagPrefs(void) 
{
  char module[80];
  char level[25];

  DiagLevel("diag", DIAG_ENV);
  char filename[80];
  sprintf(filename, "%s/%s", getenv("HOME"), ".diag");

  FILE * fp = fopen(filename,"r");
  if (fp != 0) {
    while ( ! feof( fp ) ) {
      if ( fscanf(fp,"%s %s", module, level) == 2 ) {
	if ( module[0] != '#' ) {
	  DiagLevel( module, NameToLevel(level) );
	  DIAG( "diag", DIAG_INFO, cout << "Setting " << module << " diag level to " << level << endl);
	}
      }
    }
    fclose(fp);
    DIAG( "diag", DIAG_INFO, cout << "Completed processing of .diag file" << endl);
  } else 
    DIAG( "diag", DIAG_INFO, cout << "Unable to find any .diag file" << endl);
}

char * stripSlash(const char * string)
{
  // takes a string with zero or more slashes in it, and removes them.
  char * r = 0;

  if (!(r = strrchr( string, '/' )))
    r = (char *)string; // no slashes present return string unharmed
  else
    r++;
  // we could copy r + 1 back into string, killing the beginning ...
  return r;
}
