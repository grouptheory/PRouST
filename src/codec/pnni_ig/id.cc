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
static char const _id_cc_rcsid_[] =
"$Id: id.cc,v 1.117 1999/03/01 17:54:59 mountcas Exp $";
#endif

#include <common/cprototypes.h>
#include <codec/pnni_ig/id.h>

#ifndef CONV_TEST
#include <codec/uni_ie/addr.h>
#endif

int PeerID::_refcnt = 0;
int NodeID::_refcnt = 0;

ostream & operator << (ostream & os, const PeerID & rhs)
{
  char section[1024]; // enough for the whole thing + terminator
  sprintf(section,"%d:",rhs._id[0]);  // 0
  os << section;
    
  sprintf(section,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
	  rhs._id[1], rhs._id[2], rhs._id[3], rhs._id[4], rhs._id[5],
	  rhs._id[6], rhs._id[7], rhs._id[8], rhs._id[9], rhs._id[10],
	  rhs._id[11], rhs._id[12], rhs._id[13]);
  os << section;

  return os;
}

const char * PeerID::Print(void) const
{
  bzero((char *)_string, 32);
  sprintf((char *)_string,"%d:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
	  _id[0], _id[1], _id[2], _id[3], _id[4], _id[5], _id[6], _id[7], 
	  _id[8], _id[9], _id[10], _id[11], _id[12], _id[13]);
  return _string;
}

void PeerID::SetLevel(int level) 
{ 
  _id[0] = level; 

  // Zeros all bits after level
  int len = _id[0]/8, rem = _id[0]%8;
  if (len < 13) {
    if (rem)
      _id[len+1] &= ((signed char)0x80 >> (rem-1));
    else
      _id[len+1] = 0;
    
    for (int i = len+2; i < 14; i++)
      _id[i] = 0;
  }
}


PeerID::PeerID(const u_char level, const u_char * prefix)
{
  _refcnt++;

  _id[0] = level;
  // bcopy(prefix, _id + 1, 13);
  for (int i = 1; i < 14; i++)
    _id[i] = prefix[i - 1];

  SetLevel(level);
}

PeerID::PeerID(const u_char * pgid)
{
  _refcnt++;
  bcopy((void *)pgid, (void *)_id, 14);

  SetLevel(_id[0]);
}

PeerID::PeerID(const char * input)
{
  _refcnt++;
  // str looks something like: "96:47000580ffde0000000000110100"
  int next_pos = 1; /* advance after each pair of hex chars */
  bool right_nibble = false; /* begins with left */
  int  hexx, expected = 14, val = 0;

  while (*input != ':') {
    val = val * 10 + *input - '0';
    input++;
  }
  assert( val <= 160 );
  u_char id_length = val;
  input++; /* step over first : */
  val = 0;

  _id[0] = id_length;
  
  while (*input && next_pos < expected) {
    hexx = isdigit(*input) ? *input - '0' : tolower(*input) - 'a' + 10;
    if (right_nibble == false) {
      _id[ next_pos ] = (hexx << 4);
      right_nibble = true;
    } else {
      _id[ next_pos++ ] |= hexx;
      right_nibble = false;
    }
    input++;
  }
  // assert that there is nothing left over
  assert(! ((next_pos < expected) || *input) );
}

PeerID::PeerID(const PeerID & rhs)
{
  _refcnt++;
  bcopy((void *)rhs._id, (void *)_id, 14);

  SetLevel(_id[0]);
}
 
PeerID::~PeerID() 
{  
  assert( --_refcnt >= 0 );
}

const u_char * PeerID::GetPGID(void) const { return _id; }

const u_char   PeerID::GetLevel(void) const { return _id[0]; }

const u_char * PeerID::GetPID(void) const { return GetPGID(); }

int PeerID::GetPGID(u_char * rval) const
{
  if (rval) {
    memcpy(rval, _id, 14);
    return 0;
  }
  return -1;
}


PeerID * PeerID::copy(void) const 
{
  assert(this);
  return new PeerID(_id);
}

int PeerID::PGIDcmp(const PeerID * p2) const
{
  return memcmp(_id, p2->_id, 14);
}

int PeerID::IsZero(void) const
{
  for (int i = 0; i < 14; i++) {
    if (_id[i])
      return(0);
  }
  return(1);
}

int PeerID::operator == (const PeerID & pgid) const
{
  return pgid.equals(this);
}

int PeerID::equals(const PeerID * rhs) const
{
  if (rhs && (_id[0] == rhs->_id[0])) {
    int len = _id[0]/8;
    if (len == 20)
      len = 13;
    return (!memcmp(_id+1, rhs->_id+1, len));
  }
  return(0);
}

int PeerID::equals(const u_char * rhs) const
{
  PeerID tmp(rhs);
  return equals(&tmp);
}

int compare(PeerID const & lhs, PeerID const & rhs)
{
  return lhs.PGIDcmp(&rhs);
}

int compare(PeerID * const & lhs, PeerID * const & rhs)
{
  return lhs->PGIDcmp(rhs);
}

//==================== NODES =========================================
NodeID::NodeID(const u_char level, const u_char * childpgid, const u_char * esi, const u_char sel)
{
  _refcnt++;
  int i = 0, j = 0;

  _id[0] = level;

  for (i = 1, j = 0; i < 14 && j < 13; i++, j++)
    _id[i] = childpgid[j];

  for (i = 14, j = 0; i < 21 && j < 6; i++, j++)
    _id[i] = esi[j];

  _id[21] = sel;
}

NodeID::NodeID(const unsigned char * nid)
{
  _refcnt++;
  if (nid)
    bcopy((void *)nid, (void *)_id, 22);
  else
    bzero(_id, 22);
}

NodeID::NodeID(const char * input)
{
  _refcnt++;
  // str looks something like: "96:160:47000580ffde0000000000110100000000000000"
  int next_pos = 2; /* advance after each pair of hex chars */
  bool right_nibble = false; /* begins with left */
  int  hexx, expected = 22, val = 0;

  while (*input != ':') {
    val = val * 10 + *input - '0';
    input++;
  }
  assert( val <= 160 );
  u_char id_length = val;
  input++; /* step over first : */
  
  val = 0;
  // since this is a NodeID we expect another ':'
  assert( strchr(input, ':') );

  while (*input != ':') {
    val = val * 10 + *input - '0';
    input ++;
  }
  assert( val <= 160 );
  u_char first_byte = val;
  input++; /* step over second : */

  _id[0] = id_length;
  _id[1] = first_byte;
  
  while (*input && next_pos < expected) {
    hexx = isdigit(*input) ? *input - '0' : tolower(*input) - 'a' + 10;
    if (right_nibble == false) {
      _id[ next_pos ] = (hexx << 4);
      right_nibble = true;
    } else {
      _id[ next_pos++ ] |= hexx;
      right_nibble = false;
    }
    input++;
  }
  // assert that there is nothing left over
  assert(! ((next_pos < expected) || *input) );
}

NodeID::NodeID(const Addr * atmaddr, const PeerID * pgid)
{
  _refcnt++;
#ifndef CONV_TEST
  ((Addr *)atmaddr)->encode(_id + 2);
#endif

  // In this case we MUST have a non-zero selector byte to use
  // as the level.
  assert( _id[21] != 0 );
  _id[0]  = _id[21];
  _id[21] = 0;

  if (pgid) 
    _id[1] = pgid->GetLevel();
  else   // If you don't specify a PGID, I must assume this is the physical level
    _id[1] = _id[0] == 96 ? 160 : _id[0] + 8 ; // _id[1] = 160;
}

NodeID::NodeID(const NodeID & rhs)
{
  _refcnt++;
  bcopy((void *)rhs._id, (void *)_id, 22);
}

NodeID::NodeID(const u_char level, const u_char child_level, const NodeID * base_nodeid)
{
  _refcnt++;

  bcopy((void *)base_nodeid->_id, (void *)_id, 22);
  _id[0] = level;
  _id[1] = child_level;
}

NodeID::~NodeID() 
{ 
  assert(--_refcnt >= 0);
}

NodeID * NodeID::copy(void) const
{
  assert(this);
  return new NodeID((const unsigned char *)_id);
}

int NodeID::IsZero(void) const
{
  for (int i = 0; i < 22; i++) {
    if (_id[i])
      return(0);
  }
  return(1);
}

int NodeID::operator < (const NodeID & rhs) const
{
  if ((_id[0] < rhs._id[0]) ||
      (memcmp(_id, rhs._id, 22) < 0))
    return 1;
  return 0;
}

int NodeID::operator > (const NodeID & rhs) const
{
  if ((_id[0] > rhs._id[0]) ||
      (memcmp(_id, rhs._id, 22) > 0))
    return 1;
  return 0;
}

int NodeID::operator == (const NodeID & nid) const
{
  return nid.equals(this);
}

int NodeID::equals(const NodeID * rhs) const
{
  // ignore selector byte - Why?  We're checking for pure equivalency
  if (rhs != 0 && !memcmp(_id, rhs->_id, 21))
    return 1;
  return 0;
}

int NodeID::equals(const Addr * rhs) const
{
#ifndef CONV_TEST
  if (!rhs) return 0;
  u_char buf[20]; 
  int i = ((Addr *)rhs)->encode(buf);  // assert(i == 20);
  return (!memcmp(_id + 2, buf, 19));  // Skip selector byte since we hide level in there
#else
  return -1;
#endif
}

bool NodeID::prefixes(NodeID * rhs)
{
  if (!rhs) return false;
  // only compare bytes 1 thru 13
  return ((bool)!memcmp(_id + 2, (rhs->_id) + 2, 13));
}

PeerID * NodeID::GetChildPeerGroup(void) const
{
  if (_id[1] == 160)
    return 0;
  // PeerGroup knows to only copy the first 14 bytes
  return new PeerID(_id+1);
}

int NodeID::GetChildPeerGroup(PeerID & rval) const
{
  if (_id[1] != 160) {
    PeerID tmp(_id+1);
    rval = tmp;
    return 0;
  }
  return -1;
}

int NodeID::GetPeerGroup(PeerID & rval) const
{
  int length;
  // PeerGroup knows to only copy the first 14 bytes
  u_char _pg[14];
  bzero(_pg, 14);
  length = _id[0]/8;
  bcopy((void *)(_id + 2), _pg+1, length);
  _pg[0] = _id[0];
  PeerID tmp(_pg);
  rval = tmp;
  return 0;
}

PeerID * NodeID::GetPeerGroup(void) const
{
  int length;
  // PeerGroup knows to only copy the first 14 bytes
  u_char _pg[14];
  bzero(_pg, 14);
  length = _id[0]/8;
  bcopy((void *)(_id + 2), _pg+1, length);
  _pg[0] = _id[0];
  return new PeerID(_pg);
}

const u_char NodeID::GetLevel(void) const { return _id[0]; }
void NodeID::SetLevel(u_char level) { _id[0] = level; }

const u_char   NodeID::GetChildLevel(void) const { return _id[1]; }
void NodeID::SetChildLevel(u_char level) { _id[1] = level; }

const u_char * NodeID::GetNID(void) const 
{ 
  return _id; 
}

int NodeID::GetNID(u_char * rval) const
{
  if (rval) {
    memcpy(rval, _id, 22);
    return 0;
  }
  return -1;
}

const u_char * NodeID::GetATM(void) const
{
  return (_id + 2);
}

int NodeID::GetATM(u_char * rval) const
{
  if (rval) {
    memcpy(rval, _id + 2, 20);
    return 0;
  }
  return -1;
}

const u_char * NodeID::GetPID(void) const
{
  return (_id+1);
}

int NodeID::GetPID(u_char * rval) const
{
  if (rval) {
    memcpy(rval, _id+1, 14);
    return 0;
  }
  return -1;
}

Addr * NodeID::GetAddr(void) const
{
  int i = 20;
#ifndef CONV_TEST
  u_char buf[20];
  bcopy((u_char *)_id + 2, buf, 20);
  // Selector byte always holds the level
  buf[19] = _id[0];

  NSAP_addr * rval = new NSAP_DCC_ICD_addr;
  rval->decode(buf, i);
  return rval;
#else
  return 0;
#endif
}

int NodeID::GetAddr(Addr * rval) const
{
#ifndef CONV_TEST
  int i = 20;
  u_char buf[20];
  bcopy((u_char *)_id + 2, buf, 20);
  // Selector byte always holds the level
  buf[19] = _id[0];
  rval->decode(buf, i);
  return 0;
#else
  return -1;
#endif
}

const u_char NodeID::GetSel(void) const { return _id[21]; }

u_char * NodeID::GetESI(void) const
{
  u_char * esi = new u_char [6];
  int i, j;
  for (i = 0, j = 15; i < 6 && j < 21; i++, j++)
    esi[i] = _id[j];
  return esi;
}

int NodeID::GetESI(u_char * rval) const
{
  if (rval) {
    int i, j;
    for (i = 0, j = 15; i < 6 && j < 21; i++, j++)
      rval[i] = _id[j];
    return 0;
  }
  return -1;
}

ostream & operator << (ostream & os, const NodeID & rhs)
{
  // " 72:152:47.000580ffe1000000f21a2e060020481a2e0600"
  char section[1024]; // enough for the whole thing + terminator
  int i;
  
  sprintf(section,"%d:%d:",rhs._id[0],rhs._id[1]);
  os << section;
  
  sprintf(section,
	  "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
	  rhs._id[ 2],rhs._id[ 3], rhs._id[ 4], rhs._id[ 5],
	  rhs._id[ 6], rhs._id[ 7], rhs._id[ 8],
	  rhs._id[ 9], rhs._id[10], rhs._id[11],
	  rhs._id[12], rhs._id[13], rhs._id[14]);
  os << section;
  
  sprintf(section,"%02x%02x%02x%02x%02x%02x",
	  rhs._id[15],rhs._id[16],
	  rhs._id[17],rhs._id[18],
	  rhs._id[19],rhs._id[20]);
  os << section;
  
  sprintf(section,"%02x",rhs._id[21]);
  os << section;
  return os;
}

const char * NodeID::Print(void) const
{
  bzero((char *)_string, 50);
  sprintf((char *)_string,  // 48 bytes should be sufficient to hold this w/o terminator
	  "%d:%d:%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
	  _id[0], _id[1], _id[2], _id[3], _id[4], _id[5], _id[6], _id[7], 
	  _id[8], _id[9], _id[10], _id[11], _id[12], _id[13], _id[14], 
	  _id[15], _id[16], _id[17], _id[18], _id[19], _id[20], _id[21]);
  return _string;
}

// ------------------------------------------------------------------
void abbreviatedPrint(char * str, const NodeID * node, bool dots) 
{
  // convert 96:160:47000580ffde0000000000220300000000000000
  //    into 96:160:-2203- OR 96:160:.2203.
  char nodeid[80];
  strcpy(nodeid, node->Print());

  strcpy(nodeid, node->Print());
  int len = strlen(nodeid);
  char fixed[80]; 
  int j=0;
  int colonctr=0;
  bool zeroseg=false;
  bool copymode=false;
  for (int i=0; i<len; i++) {
    if (i == 33) {
      fixed[j] = (dots ? '.' : '-');
      j++;
      break;
    }

    if (nodeid[i]==':') {
      colonctr++;
      if (colonctr==2)
	i+=13; // skip common prefix
    }

    if (colonctr<2) {
      fixed[j]=nodeid[i];
      j++;
    }
    else {
      if (nodeid[i]=='0') {
	if ((!zeroseg) && (!copymode))
	  zeroseg=true;
	if (copymode) {
	  fixed[j]=nodeid[i];
	  j++;
	}
      }
      else {
	if (zeroseg) {
	  zeroseg=false;
	  copymode=true;
	  fixed[j] =  (dots ? '.' : '-');
	  j++;
	}
	if (copymode) {
	  fixed[j]=nodeid[i];
	  j++;
	}
      }
    }
  }
  fixed[j]=0;
  strcpy(str,fixed);
}


istream & operator >> (istream & is, NodeID & rhs)
{
  unsigned char uc[22];
  unsigned int pgl, len;
  char c;
  char left, right;
  bool good;
  
  // get the first decimal number
  if (!(is >> pgl)) {
    cerr << "Error: reading NodeID peer group len" << endl;
    is.clear(ios::failbit);
    return is;
  }
  
  // check for :
  is.get(c);
  if ( c != ':') {
    cerr << "Error: reading NodeID expecting first ':' found '"
	 << c <<"'" <<endl;
    cerr << "  NodeID so far=" << pgl <<endl;
    is.clear(ios::failbit);
    return is;
  }
  
  // get the second decimal number
  if (!(is >>len)) {
    cerr << "Error: reading NodeID len" << endl;
    cerr << "  NodeID so far=" << pgl << ":" <<endl;
    is.clear(ios::failbit);
    return is;
  }
  
  // check for :
  is.get(c);
  if (c != ':') {
    cerr << "Error: reading NodeID expecting second ':' found '"
	 << c <<"'" <<endl;
    cerr << "  NodeID so far=" << pgl << ":" << len <<endl;
    is.clear(ios::failbit);
    return is;
  }
  
  if (pgl <= len && pgl <= 104 && len <= 160) {
    rhs._id[0] = pgl;
    rhs._id[1] = len;
  } else {
    cerr << "Error: reading NodeID"<< endl;
    cerr << "  NodeID so far=" << pgl << ":" << len << ":"<< endl;
    cerr << "  invalid values" <<endl;               
    is.clear(ios::failbit);
    return is;
  }
  
  
  for (int i=2; i<22; i++) {
    left = right = 0;
    
    is.get(left);
    good = is.good() && isxdigit(left);
    if (good) {
      is.get(right);
      good = is.good() && isxdigit(right);
    }
    
    if ( !good ) {
      cerr << "Error: reading NodeID " <<endl;
      cerr << "  NodeID so far="
	   << (int)rhs._id[0] << ":" << (int)rhs._id[1] << ":";
      for (int j=2; j<i; j++) {
	cerr.width(2); cerr.fill('0');
	cerr << hex << (unsigned int)rhs._id[j];
      }
      cerr << dec <<endl;
      cerr << "  Last reading '";
      if (isprint(left))
	cerr << left;
      else
	cerr << hex << "0x" << (int)left << dec; 
      cerr << "'";
      
      if (right) {
	cerr <<" and '";
	if (isprint(right))
	  cerr << right;
	else
	  cerr << hex << "0x" << (int)right << dec; 
	
	cerr <<"'";
      }
      cerr <<endl; 
      // exit (1);
      is.clear(ios::failbit);
      return is;
    }
    rhs._id[i] =
      (isdigit(left) ?left  - '0':tolower(left) -'a'+10 )<<4 |
      (isdigit(right)?right - '0':tolower(right)-'a'+10 );
    
  }
  return is;
}

//
// we need to compare NSAPs with NodeIDs for routing
// to do this a NodeID with 152:152:Nsap:selector is constructed ( >=152 ok)
// minimal testing has revealed that the object being sought in
// the graph is the first argument (lhs)
// if lhs._id[0] < 152 then this is an ordinary NodeID
// else compare the length in second byte (converting from bits to bytes)
//
// this nonsense will go away. Keep it for now - during transition
int compare(NodeID const & lhs, NodeID const & rhs)
{
  if (lhs._id[0] < 152) { // normal case
    // NodeID to NodeID
    int len = 16 + (lhs._id[1] > 104 ? 104 : lhs._id[1]); //16bits=2leading bytes
    return bitcmp(lhs._id, rhs._id, len); // 
  } else
    // special routing comparison -- this is the nonsense
    return bitcmp(&lhs._id[2], &rhs._id[2], rhs._id[1]);
}

int compare(NodeID * const & lhs, NodeID * const & rhs)
{
  return memcmp(lhs->_id, rhs->_id, 21);
}

int compare(NodeID const * const & lhs, NodeID const * const & rhs)
{
  return memcmp(lhs->_id, rhs->_id, 21);
}

// lexico compare needed by NP
int NodeID::NIDcmp(const NodeID * n2) const
{
  return memcmp(_id,n2->_id,22);
}

// like memcmp but at the bit level
int bitcmp (const unsigned char *lhs, const unsigned char *rhs, int bits)
{
  assert(lhs && rhs);

  int bytes = bits/8;
  int rbits = bits - bytes * 8;
  int diff = 0;
  
  // compare all complete bytes 
  if (bytes) diff = memcmp(lhs, rhs, bytes);

  // if there is a difference and there are uncompared bits do a bit compare
  if (rbits && (diff == 0) ) {
    unsigned char mask = (unsigned char)(((signed char)0x80)>>(rbits-1));
    
    diff = (signed char)(lhs[bytes]&mask) - (signed char)(rhs[bytes]&mask);
  }

  // convert result to -1,0,-1 range 
  if (diff > 0)      diff = +1;
  else if (diff < 0) diff = -1;
  return diff;
}


/*
 * map of number of 0 bits to left of each possible value
 *
 * value: 128  64  32  16   8   4   2   1
 * cbit:   0    1   2   3   4   5   6   7
 *
 */
static int cbit[256] ={
    8,7,6,6,5,5,5,5,4,4,4,4,4,4,4,4,  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/*
 * compare two byte streams and return number of matching bits
 * up to max of max_bits
 */
int bits_equal (const unsigned char *lhs,
		const unsigned char *rhs,
		int max_bits)
{
    int i=0, j;
    int max_bytes = max_bits/8;
    int result = 0;
    int rbits;
    /* compare full bytes where possible
     * For our purposes max_bits is <= 22*8. For this number the overhead
     * of doing int compares is too costly - the ints to be compaed must
     * be formed by shifting to avoid alignment problems. So just use bytes
     */
    while (i < max_bytes && ( lhs[i] == rhs[i]) ) i++;

    result = i*8;
    if ( result == max_bits) return result;

    /* figure out max number of bits remaining must be 1...8 (not 0)
     * there are two cases
     * 1) a difference was found in the middle somewhere
     *    so we need to compare the bytes that differed
     * 2) all the complete bytes were the same
     *    and we need to look at the remaining bits
     */
    rbits = (i==max_bytes) ? max_bits - result : 8; // left bits in the byte

    // xor the two bytes - and lookup the number of bits this represents 
    j = cbit[(lhs[i] ^ rhs[i])];

    // use the lessor of the computed value and the number of remaining bits
    j = ( j > rbits) ? rbits : j;

    // add to value from complete comparisons
    result = result + j;
    return result;
}

/*
 * set bits off - from start_bit to (last_bit -1) 
 * first bit is 0'th bit = left most bit in byte 0.
 *
 * to zero the second byte bits_zero(string, 8, 16);
 * bit 8  is the first bit of byte 1 (second byte).
 * bit 15 (=16-1) is the last bit in the second byte 
 *
 * the special case of complete bytes being zeroed occurs so
 * often (almost always) that it is treated as a special case
 *
 * code is suitabel for a small number of bits (a few hundred).
 * a complete NodeID is 16 + 160 bits =176 bits for example.
 */
void bits_zero(unsigned char *string, int start_bit, int last_bit)
{
  int start_byte = start_bit/8;
  int head_bits  = start_bit % 8; /* bits in head of start_byte */
  int last_byte  = (last_bit-1)/8;
  int tail_bits  = last_bit % 8;  /* bits in tail of last byte */
  unsigned char head_mask=0;
  unsigned char tail_mask=0;
  int i;

  //  special case full bytes
  if (head_bits == 0 && tail_bits == 0)
    {
      for( i = start_byte; i<= last_byte; i++) string[i]=0;
    }
  else
    {
      // general case
      if (head_bits != 0) {
	head_mask = (unsigned char)((signed char)0x80 >> (head_bits -1));
      }
  
      if (tail_bits != 0) {
	tail_mask = (unsigned char)~((signed char)0x80 >> (tail_bits -1));
      }

      if (start_byte == last_byte) {
	string[start_byte] =
	  (string[start_byte] & head_mask) |
	  (string[start_byte] & tail_mask);
      } else {

	if (head_bits != 0) {
	  string[start_byte] = string[start_byte] & head_mask;
	  start_byte++;
	}
	if (tail_bits != 0) {
	  string[last_byte] = string[last_byte] & tail_mask;
	  last_byte--;
	}
	for (i=start_byte; i<=last_byte; i++) string[i] = 0;
      }
    }
  return;
}

// print a bit stream of 1's and 0's on stdout
void bit_print(unsigned char *string, int len)
{
  int i=0;
  int bit=0;

  while (i<len) {
    int byte = i/8;
    int bits_left = len - byte*8;
    int bits_this_byte = (bits_left>8)? 8 : bits_left;

    unsigned char mask = 0x80;
    for (bit = 0; bit < bits_this_byte; bit++) {
      if ( ((mask >> bit) & string[byte]) != 0) cout <<"1";
      else cout << "0";
    }
    cout << " ";
    i+= bits_this_byte;
  }
  cout << endl;
}

bool NodeID::Is_Me_At_Some_Level( const NodeID * orig ) const
{
  const u_char * his_atm = orig->GetATM();
  const u_char * my_atm  = GetATM();

  return ( memcmp(his_atm,my_atm,20)==0 );
}
