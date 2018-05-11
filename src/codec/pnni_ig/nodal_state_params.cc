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
static char const _nodal_state_cc_rcsid_[] =
"$Id: nodal_state_params.cc,v 1.12 1998/12/02 17:05:31 mountcas Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/pnni_ig/nodal_state_params.h>
#include <codec/pnni_ig/resrc_avail_info.h>

ig_nodal_state_params::ig_nodal_state_params(sh_int flags, int ip, int op) : 
  InfoGroup(InfoGroup::ig_nodal_state_params_id),
  _flags(flags), _input_pid(ip),
  _output_pid(op) { }

ig_nodal_state_params::ig_nodal_state_params(const ig_nodal_state_params & rhs) 
  : InfoGroup(InfoGroup::ig_nodal_state_params_id),
    _flags(rhs._flags), _input_pid(rhs._input_pid),
    _output_pid(rhs._output_pid) 
{ 
  if (rhs._raigs.empty() == false) {
    list_item li;
    forall_items(li, rhs._raigs) {
      _raigs.append( (ig_resrc_avail_info *)(rhs._raigs.inf(li)->copy()) );
    }
  }
}

ig_nodal_state_params::~ig_nodal_state_params() { }

InfoGroup * ig_nodal_state_params::copy(void)
{
  return new ig_nodal_state_params(*this);
}

void ig_nodal_state_params :: AddRAIG(ig_resrc_avail_info *p)
{
  _raigs.append(p);
}

const list<ig_resrc_avail_info *> & ig_nodal_state_params::GetRAIGS(void) const
{
  return _raigs;
}

void ig_nodal_state_params::size(int & length)
{
 length += NODAL_STATE_HEADER;
 if (_raigs.size() != 0) {
   list_item li;
   forall_items(li, _raigs) {
     ig_resrc_avail_info * raig = _raigs.inf(li);
     raig->size(length);
   }
 }
}

u_char * ig_nodal_state_params::encode(u_char * buffer, int & buflen)
{
  buflen = 0;
  u_char * temp = buffer + ig_header_len;

  *temp++ = (_flags >> 8) & 0xFF;
  *temp++ = (_flags & 0xFF);

  // Reserved
  *temp++ = 0;
  *temp++ = 0;

  *temp++ = (_input_pid >> 24) & 0xFF;
  *temp++ = (_input_pid >> 16) & 0xFF;
  *temp++ = (_input_pid >>  8) & 0xFF;
  *temp++ = (_input_pid & 0xFF);

  *temp++ = (_output_pid >> 24) & 0xFF;
  *temp++ = (_output_pid >> 16) & 0xFF;
  *temp++ = (_output_pid >>  8) & 0xFF;
  *temp++ = (_output_pid & 0xFF);

  buflen += 12;

  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_nodal_state_params::decode(u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF));
  int len  = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  if (len != 12 || type != InfoGroup::ig_nodal_state_params_id)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);

  u_char * temp = &buffer[4];

  _flags  = (*temp++ << 8) & 0xFF00;
  _flags |= (*temp++ & 0xFF);

  temp += 2; // Skip the reserved bytes

  _input_pid  = (*temp++ << 24) & 0xFF000000;
  _input_pid |= (*temp++ << 16) & 0x00FF0000;
  _input_pid |= (*temp++ <<  8) & 0x0000FF00;
  _input_pid |= (*temp++ & 0xFF);

  _output_pid  = (*temp++ << 24) & 0xFF000000;
  _output_pid |= (*temp++ << 16) & 0x00FF0000;
  _output_pid |= (*temp++ <<  8) & 0x0000FF00;
  _output_pid |= (*temp++ & 0xFF);

  return 0;
}

const int ig_nodal_state_params::GetInputPID(void) const
{ return _input_pid; }

const int ig_nodal_state_params::GetOutputPID(void) const
{ return _output_pid; }

bool ig_nodal_state_params::PrintSpecific(ostream & os)
{
  os.setf(ios::hex);
  os << "     Flags::" << _flags << endl;
  os.setf(ios::dec);
  os << "     Input Port ID::" << _input_pid << endl;
  os << "     Output Port ID::" << _output_pid << endl;

  if (_raigs.empty() == false) {
    os << "RAIGs ----------------------------------" << endl;
    list_item li;
    forall_items(li, _raigs) {
      ig_resrc_avail_info * raig = _raigs.inf(li);
      os << *raig << endl;
    }
  } else
    os << "No RAIGs present." << endl;

  return true;
}

bool ig_nodal_state_params::ReadSpecific(istream & is)
{
  char buf[80], *temp = buf;

  is.getline(buf, 80);
  while (*temp && !isdigit(*temp))
    temp++;
  _flags = atoi(temp);

  is.getline(buf, 80);
  temp = buf;
  while (*temp && !isdigit(*temp))
    temp++;
  _input_pid = atoi(temp);

  is.getline(buf, 80);
  temp = buf;
  while (*temp && !isdigit(*temp))
    temp++;
  _output_pid = atoi(temp);
  return true;
}

ostream & operator << (ostream & os, ig_nodal_state_params & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return (os);
}

istream & operator >> (istream & is, ig_nodal_state_params & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return is;
}
