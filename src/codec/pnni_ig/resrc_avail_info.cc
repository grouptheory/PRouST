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
static char const _resrc_avail_info_cc_rcsid_[] =
"$Id: resrc_avail_info.cc,v 1.20 1999/01/25 11:23:46 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <iostream.h>
#include <codec/pnni_ig/resrc_avail_info.h>
#include <codec/pnni_ig/optional_gcac_params.h>

ig_resrc_avail_info::ig_resrc_avail_info(resrc_type type) :
  InfoGroup(InfoGroup::ig_unknown_id), _type(type), _RAIG_Flags(0),
  _Admin_weight(0), _max_cell_rate(0), _avail_cell_rate(0), _cell_xfer_delay(0),
  _cell_delay_variation(0), _cell_loss_ratio_0(0), _cell_loss_ratio_01(0),
  _gcac(0)
{
  if (type == outgoing)
    SetId(InfoGroup::ig_outgoing_resource_avail_id);
  else
    SetId(InfoGroup::ig_incoming_resource_avail_id);
}

ig_resrc_avail_info::ig_resrc_avail_info(resrc_type type, u_short flags,
		      int weight, int mcr, int acr,
		      int ctd, int cdv, sh_int clr0, sh_int clr01) :
  InfoGroup(InfoGroup::ig_unknown_id), _type(type), _RAIG_Flags(flags),
  _Admin_weight(weight), _max_cell_rate(mcr), _avail_cell_rate(acr), _cell_xfer_delay(ctd),
  _cell_delay_variation(cdv), _cell_loss_ratio_0(clr0), _cell_loss_ratio_01(clr01),
  _gcac(0)
{
  if (type == outgoing)
    SetId(InfoGroup::ig_outgoing_resource_avail_id);
  else
    SetId(InfoGroup::ig_incoming_resource_avail_id);
}

ig_resrc_avail_info::ig_resrc_avail_info(const ig_resrc_avail_info & rhs) 
  : InfoGroup(InfoGroup::ig_unknown_id), _type(rhs._type), _RAIG_Flags(rhs._RAIG_Flags),
    _Admin_weight(rhs._Admin_weight), _max_cell_rate(rhs._max_cell_rate), 
    _avail_cell_rate(rhs._avail_cell_rate), _cell_xfer_delay(rhs._cell_xfer_delay),
    _cell_delay_variation(rhs._cell_delay_variation), 
    _cell_loss_ratio_0(rhs._cell_loss_ratio_0), _cell_loss_ratio_01(rhs._cell_loss_ratio_01),
    _gcac(0)
{
  if (rhs._gcac)
    _gcac = (ig_optional_gcac_params *)rhs._gcac->copy();

  if (_type == outgoing)
    SetId(InfoGroup::ig_outgoing_resource_avail_id);
  else
    SetId(InfoGroup::ig_incoming_resource_avail_id);
}

ig_resrc_avail_info::~ig_resrc_avail_info( ) { }

InfoGroup * ig_resrc_avail_info::copy(void) 
{
  return new ig_resrc_avail_info(*this);
}

void ig_resrc_avail_info::size(int & length)
{
  length += RAIG_CONTENT; // Without gcac 32
  if(_gcac)
    _gcac->size(length);
}

u_char * ig_resrc_avail_info::encode(u_char * buffer, int & buflen)
{
  buflen = 0;
  u_char * temp = buffer + ig_header_len;

  *temp++ = (_RAIG_Flags >> 8) & 0xFF;
  *temp++ = (_RAIG_Flags & 0xFF);

  *temp++ = 0;   // Reserved
  *temp++ = 0;

  *temp++ = (_Admin_weight >> 24) & 0xFF;
  *temp++ = (_Admin_weight >> 16) & 0xFF;
  *temp++ = (_Admin_weight >>  8) & 0xFF;
  *temp++ = (_Admin_weight & 0xFF);

  *temp++ = (_max_cell_rate >> 24) & 0xFF;
  *temp++ = (_max_cell_rate >> 16) & 0xFF;
  *temp++ = (_max_cell_rate >>  8) & 0xFF;
  *temp++ = (_max_cell_rate & 0xFF);

  *temp++ = (_avail_cell_rate >> 24) & 0xFF;
  *temp++ = (_avail_cell_rate >> 16) & 0xFF;
  *temp++ = (_avail_cell_rate >>  8) & 0xFF;
  *temp++ = (_avail_cell_rate & 0xFF);

  *temp++ = (_cell_xfer_delay >> 24) & 0xFF;
  *temp++ = (_cell_xfer_delay >> 16) & 0xFF;
  *temp++ = (_cell_xfer_delay >>  8) & 0xFF;
  *temp++ = (_cell_xfer_delay & 0xFF);

  *temp++ = (_cell_delay_variation >> 24) & 0xFF;
  *temp++ = (_cell_delay_variation >> 16) & 0xFF;
  *temp++ = (_cell_delay_variation >>  8) & 0xFF;
  *temp++ = (_cell_delay_variation & 0xFF);

  *temp++ = (_cell_loss_ratio_0 >> 8) & 0xFF;
  *temp++ = (_cell_loss_ratio_0 & 0xFF);
  *temp++ = (_cell_loss_ratio_01 >> 8) & 0xFF;
  *temp++ = (_cell_loss_ratio_01 & 0xFF);

  buflen += 28;

  if (_gcac) {
    int tlen = 0;
    temp = _gcac->encode(temp, tlen);
    buflen += tlen;
  }

  encode_ig_header(buffer, buflen);
  return temp;
}

errmsg * ig_resrc_avail_info::decode(u_char * buffer)
{
  int type = (((buffer[0] << 8) & 0xFF00) | (buffer[1] & 0xFF));
  int len = (((buffer[2] << 8) & 0xFF00) | (buffer[3] & 0xFF));

  // Check if this is in fact a resource availability information packet
  if (type != 128 && type != 129)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);

  decode_ig_header(buffer);
  u_char * temp = buffer + ig_header_len;

  _RAIG_Flags  = (*temp++ << 8) & 0xFF00;
  _RAIG_Flags |= (*temp++ & 0xFF);;

  temp+= 2;   // Reserved

  _Admin_weight  = (*temp++ << 24) & 0xFF000000;
  _Admin_weight |= (*temp++ << 16) & 0x00FF0000;
  _Admin_weight |= (*temp++ <<  8) & 0x0000FF00;
  _Admin_weight |= (*temp++ & 0xFF);

  _max_cell_rate  = (*temp++ << 24) & 0xFF000000;
  _max_cell_rate |= (*temp++ << 16) & 0x00FF0000;
  _max_cell_rate |= (*temp++ <<  8) & 0x0000FF00;
  _max_cell_rate |= (*temp++ & 0xFF);

  _avail_cell_rate  = (*temp++ << 24) & 0xFF000000;
  _avail_cell_rate |= (*temp++ << 16) & 0x00FF0000;
  _avail_cell_rate |= (*temp++ <<  8) & 0x0000FF00;
  _avail_cell_rate |= (*temp++ & 0xFF);

  _cell_xfer_delay  = (*temp++ << 24) & 0xFF000000;
  _cell_xfer_delay |= (*temp++ << 16) & 0x00FF0000;
  _cell_xfer_delay |= (*temp++ <<  8) & 0x0000FF00;
  _cell_xfer_delay |= (*temp++ & 0xFF);

  _cell_delay_variation  = (*temp++ << 24) & 0xFF000000;
  _cell_delay_variation |= (*temp++ << 16) & 0x00FF0000;
  _cell_delay_variation |= (*temp++ <<  8) & 0x0000FF00;
  _cell_delay_variation |= (*temp++ & 0xFF);

  _cell_loss_ratio_0   = (*temp++ <<  8) & 0x0000FF00;
  _cell_loss_ratio_0  |= (*temp++ & 0xFF);
  _cell_loss_ratio_01  = (*temp++ <<  8) & 0x0000FF00;
  _cell_loss_ratio_01 |= (*temp++ & 0xFF);

  len -= 28;

  errmsg * rval = 0;
  if (len > 0) {
    int tlen = (((temp[2] << 8) & 0xFF00) | (temp[3] & 0xFF));
    _gcac = new ig_optional_gcac_params(0,0);
     rval = _gcac->decode(temp);

    if (rval)
      return rval;

    len -= tlen + 4;
  }

  if (len < 0)
    return new errmsg(errmsg::normal_unspecified, errmsg::wrong_id, type);
  else
    return rval;
}

const u_int  ig_resrc_avail_info::GetFlags(void) const 
{ return (u_int)_RAIG_Flags; }

const int ig_resrc_avail_info::GetAdminWeight(void) const 
{ return _Admin_weight; }

const int ig_resrc_avail_info::GetMCR(void) const 
{ return _max_cell_rate; }

const int ig_resrc_avail_info::GetACR(void) const 
{ return _avail_cell_rate; }

const int ig_resrc_avail_info::GetCTD(void) const 
{ return _cell_xfer_delay; }

const int ig_resrc_avail_info::GetCDV(void) const 
{ return _cell_delay_variation; }

const int  ig_resrc_avail_info::GetCLR0(void) const
{ return _cell_loss_ratio_0; }

const int  ig_resrc_avail_info::GetCLR01(void) const
{ return _cell_loss_ratio_01; }

const ig_optional_gcac_params * ig_resrc_avail_info::GetGCAC(void) const 
{ return _gcac; }

bool ig_resrc_avail_info::PrintSpecific(ostream & os)
{
  if (_type == outgoing)
    os << "    Outgoing Resource Availability" << endl;
  else
    os << "    Incoming Resource Availability" << endl;
  os << "    Length::" << _length << endl;
  os << "    RAIG_Flags::" << hex << _RAIG_Flags << endl;
  os << "    Administrative Weight::" << dec << _Admin_weight << endl;
  os << "    Maximum Cell Rate::" << _max_cell_rate << endl;
  os << "    Available Cell Rate::" << _avail_cell_rate << endl;
  os << "    Cell Transfer Delay::" << _cell_xfer_delay << endl;
  os << "    Cell Delay Variation::" << _cell_delay_variation << endl;
  os << "    Cell Loss Ratio (CLP=0)::" << _cell_loss_ratio_0 << endl;
  os << "    Cell Loss Ratio (CLP=0+1)::" << _cell_loss_ratio_01 << endl;

  if (_gcac)  os << (*_gcac);

  return true; 
}

void ig_resrc_avail_info::SetFlags(u_int f)     { _RAIG_Flags = (u_short)f;        }
void ig_resrc_avail_info::SetAdminWeight(int a) { _Admin_weight = a;               }
void ig_resrc_avail_info::SetMCR(int m)         { _max_cell_rate = m;              }
void ig_resrc_avail_info::SetACR(int a)         { _avail_cell_rate = a;            }
void ig_resrc_avail_info::SetCTD(int c)         { _cell_xfer_delay = c;            }
void ig_resrc_avail_info::SetCDV(int c)         { _cell_delay_variation = c;       }
void ig_resrc_avail_info::SetCLR0(int c)        { _cell_loss_ratio_0 = (sh_int)c;  }
void ig_resrc_avail_info::SetCLR01(int c)       { _cell_loss_ratio_01 = (sh_int)c; }

bool ig_resrc_avail_info::ReadSpecific(istream & is)
{
  return false; 
}

ostream & operator << (ostream & os, ig_resrc_avail_info & ig)
{
  ig.PrintGeneral(os);
  ig.PrintSpecific(os);
  return (os);
}

istream & operator >> (istream & is, ig_resrc_avail_info & ig)
{
  ig.ReadGeneral(is);
  ig.ReadSpecific(is);
  return is;
}
