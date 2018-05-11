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

#if !defined(LINT)
static char const _blli_cc_rcsid_[] =
"$Id: blli.cc,v 1.8 1998/09/28 09:12:00 bilal Exp $";
#endif
#include <common/cprototypes.h>

#include <codec/uni_ie/blli.h>
#include <codec/uni_ie/errmsg.h>
#include <codec/uni_ie/broadband_repeat_ind.h>

//================  Broadband Lower Layer Info ===========================

//---------- layer2 ------------------------------------------------------

// TODO: fix this ==
int layer2::operator == (const layer2 & x) const {return 1;};


layer2::layer2(layer2_types id) : _layer2_type(id) {}
enum layer2::layer2_types layer2::get_layer2_type() const {return _layer2_type;}

void layer2::set_layer2_type(layer2_types lt) { _layer2_type = lt;}


int layer2::encode(u_char * buffer)
{
  u_char * temp = buffer;
  int buflen = 0;

  switch(_layer2_type) {
  case lan_iso_8802: // bit 8 should be 1
    *temp++ = 0x80 | _layer2_type;
    break;
  case user2: // bit 8 should be 0
    *temp++ =  _layer2_type;
    break;
  }
  buflen++;
  return (buflen);
}


char layer2::PrintSpecific(ostream & os)
{
  os << "   layer2_type::";
  switch (_layer2_type) {
    case lan_iso_8802: os << "lan_iso_8802 "; break;
    case user2:        os << "user2 ";        break;
    default:           os << "unknown ";      break;
  }
  os << endl;
  return 0;
}

l2_user::l2_user(int user_info) : layer2(user2),_user_info(user_info) { }

layer2 * l2_user::copy(void) const { return new l2_user(_user_info);}

int l2_user::encode(u_char *buffer) 
{
  int buflen = 0;

  *buffer = 0x80 | _user_info;
  buflen++;
  return (buflen);
}


char l2_user::PrintSpecific(ostream & os)
{
  layer2::PrintSpecific(os);
  os << "   user_info::" << _user_info << endl;

  return 0;
}

l2_lan_iso_8802::l2_lan_iso_8802(void) : layer2(lan_iso_8802) { }
layer2 * l2_lan_iso_8802::copy(void) const { return new l2_lan_iso_8802;}


//---------- layer3 ------------------------------------------------------

// TODO: fix this ==
int layer3::operator == (const layer3 & x) const {return 1;};

layer3::layer3(layer3_types id) : _layer3_type(id) {}

enum layer3::layer3_types layer3::get_layer3_type() 
const {return _layer3_type;}

int layer3::encode(u_char * buffer)
{
  int buflen = 0;
  if (_layer3_type == user3) {

    *buffer = _layer3_type;
    buflen++;
    return (buflen);
  }
  return(0);
}

char layer3::PrintSpecific(ostream & os)
{
  os << "   layer3_type::";
  switch (_layer3_type)
    {
    case user3:          os << "user3 ";          break;
    default:             os << "unknown ";        break;
    }
  os << endl;
  return 0;
}


layer3 * l3::copy(void) const { return (0L); }

int l3::equals (const layer3 * l3) const { return 0;}

int l3::encode(u_char * buf) { return 0L; }

l3::l3(layer3::layer3_types type) : layer3(type) {}

l3::~l3() { }

//---------- ie_blli ------------------------------------------------------

ie_blli::ie_blli(layer2::layer2_types l2, layer3::layer3_types l3) :
  InfoElem(ie_blli_id), _l2_len(0), _l3_len(0),
  _num_blli(1), _next_blli(0), _blli(0), _blli_len(0)
{
  switch (l2) {
    case layer2::none:
      _l2 = 0;
      break;
      
    case layer2::lan_iso_8802:
      _l2 = new l2_lan_iso_8802();
      break;
    default:
      cerr << "ie_blli() Unsupported layer 2 type" << hex << l2 << dec << endl;
      _l2 = 0;
      break;
  }
  
  switch (l3) {
    case layer3::none:
      _l3 = 0;
      break;
    default:
      cerr << "ie_blli() Unsupported layer 3 type" << hex << l2 << dec << endl;
      _l3 = 0;
      break;
  }
}

ie_blli::ie_blli(const ie_blli & rhs) : InfoElem(ie_blli_id), _l2_len(rhs._l2_len),
  _l3_len(rhs._l3_len), _num_blli(1), _next_blli(0), _blli(rhs._blli), _blli_len(rhs._blli_len),
  _l2(0), _l3(0)
{  
}

ie_blli::ie_blli(void) :
  InfoElem(ie_blli_id), _l2(0), _l2_len(0), _l3(0), _l3_len(0), 
  _num_blli(1), _next_blli(0), _blli(0), _blli_len(0)
{}

ie_blli::~ie_blli() 
{
  delete _l2; _l2=0L;
  delete _l3; _l3=0L;
}


int ie_blli::Length()
{
  u_char temp[500];
  int len = 0;
  // L2
  if(_l2)
    len += _l2->encode(temp);
  // L3
  if(_l3)
    len += _l3->encode(temp);
  return len;
}

int ie_blli::equals(const InfoElem* him) const
{
  return him->equals(this);
}

int ie_blli::equals(const ie_blli* himptr) const
{
  layer2::layer2_types t2_me= (_l2)? _l2->_layer2_type : layer2::none;
  layer3::layer3_types t3_me= (_l3)? _l3->_layer3_type : layer3::none;

  layer2::layer2_types t2_you= (himptr->_l2)? 
    himptr->_l2->_layer2_type : layer2::none;
  layer3::layer3_types t3_you= (himptr->_l3)? 
    himptr->_l3->_layer3_type : layer3::none;

  return ((t2_me==t2_you)&&(t3_me==t3_you));
}

u_char ie_blli::IsVariableLen(void) const
{
  return ((u_char)1);
}

InfoElem* ie_blli:: copy(void) const
{
  layer2::layer2_types t2= (_l2)? _l2->_layer2_type : layer2::none;
  layer3::layer3_types t3= (_l3)? _l3->_layer3_type : layer3::none;

  return new ie_blli(t2,t3);
}

int ie_blli :: operator == (const ie_blli & blli) const
{
  return equals(&blli);
}

int ie_blli::get_num_blli(void) {return _num_blli;}

int ie_blli::max_repeats() { return 3;}

int ie_blli::encode(u_char * buffer)
{
  u_char  * temp;
  ie_blli * iptr = this;
  int len = 0;

  _l2_len = _l3_len = 0;

  if ((temp = LookupCache()) != 0L) {
    bcopy((char *)temp, (char *)buffer, (len = CacheLength()));
    buffer += len;
    return(len);
  } 
  temp   = buffer;
  int i  = NumInList();
  len = 0;

  if (IsFirstOfMany()) {
    ie_broadband_repeat_ind bri;
    len += bri.encode(temp);
  }
  u_char *cptr = temp;
  while (i-- > 0) {
    temp += ie_header_len;
    put_id(cptr,_id);
    put_coding(cptr,_coding);
    // encode L2
    if (iptr->_l2)
      len += iptr->_l2->encode(temp);
    // encode L3
    if (iptr->_l3)
      len += iptr->_l3->encode(temp);
    // finish up the header for this BLLI
     put_len(cptr,len);
    len += ie_header_len;
    cptr = temp;
    iptr = (ie_blli *)iptr->get_next_ie();
  }
  FillCache(buffer, len);
  return (len);
}

InfoElem::ie_status ie_blli::decode(u_char * buffer, int & id)
{
  id = buffer[0];
  int len = get_len(buffer);

  layertype layer_id;
  layer2::layer2_types l2_proto;
  layer3::layer3_types l3_proto;
  _l3_len = 0;
  _l3 = 0;

  if (!len)
    return (InfoElem::empty);
  if (len > 17)
    return (InfoElem::invalid_contents);
  buffer += ie_header_len;
  layer_id = (layertype) (*buffer & 0x60);
  switch (layer_id) {
    case layer_1_id: // ignore this one
      if((*buffer++ & 0x80)!= 0x80)
	return (InfoElem::invalid_contents);
      len--;
      break;
    case layer_2_id:
      delete _l2;
      _l2 = 0L;
      l2_proto = (layer2::layer2_types) (*buffer & 0x7F);
      switch (l2_proto)
	{
	case layer2::lan_iso_8802:
	  _l2 = new l2_lan_iso_8802();
	  _l2_len = 1;
	  break;
	default:
	  break;
	}

    case layer_3_id:
      delete _l3;
      _l3 = 0L;
      l3_proto=layer3::none;
      switch(l3_proto)
	{
	case layer3::user3:
	  break;
	default:
	  break;
	}
    default:
      break;
    }
  return (InfoElem::ok);
}


char ie_blli::PrintSpecific(ostream & os)
{
  os << endl << "   blli::";
  if (_blli_len > 0) os << "0x";
  for (int i = 0; i < _blli_len; i++) {
    int val = (0xFF & (int)_blli[i]);
    if (val < 16) os << "0";
    os << val;
  }
  os << endl;
  
  if (_l2)
    _l2->PrintSpecific(os);
  if (_l3)
    _l3->PrintSpecific(os);
  
  if (_next_blli)
    os << "--> " << *(_next_blli);
  os << endl;
  return 0;
}

ostream & operator << (ostream & os, ie_blli & x)
{
  x.PrintGeneral(os);
  x.PrintSpecific(os);
  return (os);
}




