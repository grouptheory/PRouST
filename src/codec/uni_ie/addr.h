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
#ifndef _ADDR_H_
#define _ADDR_H_

#ifndef LINT
static char const _addr_h_rcsid_[] =
"$Id: addr.h,v 1.16 1998/10/22 15:04:16 mountcas Exp $";
#endif

#include <codec/uni_ie/ie_base.h>

class ie_errmsg;

class E164_addr;
class NSAP_addr;
class NSAP_DCC_ICD_addr;
class NSAP_E164_addr;

//----------------------------------------------------

NSAP_DCC_ICD_addr * ThisHostNSAP(int fd);

/** class Addr.
 *
 * specify the addresses:
 * used by information elements Called Party Number, Called Party Subaddress,
 * Calling Party Number,and Calling Party Subaddress which will be
 * friends of these classes.
 *
 * There are two types of addressing: International (telephone numbers)
 * and Unknown (NSAPA). The addressing plan specifies which type is
 * in use. 0001 = ISDN/telephony OR 0010 = ATM  End System
 *
 * In the Unknown addressing type, there are 3 different formats:
 * DCC_ATM format (AFI=39), ICD_ATM format (AFI=47) , and E.164_ATM format.
 * All these 3 formats share the AFI, ESI, and SEL fields which are
 * encapsulated in the NSAP_addr.
 *
 * The DCC_ATM and ICD_ATM formats have same IDP structure so will be 
 * considered as 1 class NSAP_DCC_ICD_addr, while the E164_ATM
 * makes the other class NSAP_E164_addr.
 *
 * @see addr.fig in the doc directory.
 * @see 5.1.3 page 160 of UNI3.1
 * @see Attributes_q93b::get_called_party_num
 * @see Attributes_q93b::get_called_party_subaddr
 * @see Attributes_q93b::get_calling_party_num
 * @see Attributes_q93b::get_calling_party_subaddr
 * @see Attributes_q93b::set_called_party_num
 * @see Attributes_q93b::set_called_party_subaddr
 * @see Attributes_q93b::set_calling_party_num
 * @see Attributes_q93b::set_calling_party_subaddr
 */
class Addr : public Cache {
  // Illegal
  // friend int operator == (const Addr * a1, const Addr * a2);
  friend int compare(Addr *const &, Addr *const &);
public:

  /** Types of addresses. */
  enum Addressing_type {
    /// type number = unknown(0000) + ATM End System (0010) ==> Unknown = 0x02
    Unknown  = 0x02,

    /// type number=International(0001)+ISDN/telephony(0001)==>International=0x11
    International  = 0x11
  };

  /** Constructor.
   * Make an Addr of type addr_type
   *
   * @param addr_type The kind of address.  Defaults to Unknown.
   */
  Addr(Addressing_type addr_type = Unknown);

  /// Destructor.
  virtual ~Addr();

  /** Equality operator.
   * This is a binary, infix operator.
   * @return true if the two addresses are equal; false otherwise.
   */
  int operator == (const Addr& addr2) const; 

  /** Virtual method for mapping an Addr into an array of unsigned characters.
   * Derived classes must implement encode().
   *
   * @param buffer A pointer to an array of unsigned characters.
   *
   * @param buflen The size of the buffer in characters.  On return,
   * contains the number of characters written into the buffer.
   *
   * @return The number of characters written into the buffer.
   */
  virtual int encode(u_char * buffer) = 0;

  /** Virtual method for mapping an array of unsigned characters into an Addr.
   * Fills in the fields of the Addr object from the contents of the array.
   * Derived classes must implement decode().
   *
   * @param buffer A pointer to an array of unsigned characters.
   *
   * @param buflen The size of the buffer in characters.
   *
   * @return An ie_status enumeration indicating success or failure.
   */
  virtual InfoElem::ie_status decode(u_char *buffer, int & id) = 0;

  /// Copy constructor.
  virtual Addr      * copy(void) const = 0;

  /** Tell if the address is of variable length.
   *
   * @return 0 always.
   */
  virtual u_char    IsVariableLen(void) const;

  /** Reveal the kind of address.
   *
   * @return The type of the address.
   */
  Addressing_type   get_addressing_type(void) const;

  /** Tell if the Addr has reasonable values.
   *
   * @return 1 if the Addr is OK;  0 otherwise.
   */
  char IsValid(void) const;

  virtual char PrintSpecific(ostream & os);
  int Length(void);

  virtual int equals(const Addr & him) const;
  virtual int equals(const Addr * him) const;

protected:

  void TogValid(void);

  char            _valid;
  Addressing_type _addr_type;
};

//----------------------------------------------------

/** Class NSAP_addr.
 */
class NSAP_addr : public Addr{
  friend class     ie_calling_party_num;
  friend class     ie_calling_party_subaddr;
  friend class     ie_called_party_num;
  friend class     ie_called_party_subaddr;
 public:

  /// Values for the AFI field of the address.
  enum afi_type {
    /// DCC address.
    DCC_ATM  = 0x39,
    DCC_ATM_ANYCAST  = 0xBD,

    /// ICD address.
    ICD_ATM  = 0x47,
    ICD_ATM_ANYCAST  = 0xC5,

    /// @memo E.164 address.
    E164_ATM = 0x45,
    E164_ATM_ANYCAST  = 0xC3
  };

  /// Equality operator.
  friend operator==(const NSAP_addr& addr1, const NSAP_addr& addr2); 

  /** @name Constructors. */
  //@{
  /// Make an NSAP of no specific type.
  NSAP_addr();

  /// Make an NSAP of a specified type.
  NSAP_addr(afi_type afi);

  /// Make an NSAP of a specified type, esi, and selector.
  NSAP_addr(afi_type afi,u_char *esi, int sel);
  //@}

  /// Destructor
  virtual ~NSAP_addr();

  /// Copy constructor.
  virtual Addr * copy(void) const = 0;

  /// Copy.
  virtual NSAP_addr * nsap_addr_copy(void) const = 0;

  /// Reveal the AFI field of the address.
  afi_type get_afi_type(void) const;

  void set_afi_type(afi_type type);

  virtual char PrintSpecific(ostream & os);
  bool IsAnycast(void);

  virtual int equals(const NSAP_addr & him) const; 
  virtual int equals(const NSAP_addr * him) const; 
  virtual int equals(const Addr & him) const { return equals((const NSAP_addr&)him); }
  virtual int equals(const Addr * him) const { return equals((const NSAP_addr*)him); }

protected:

  afi_type _afi;
  u_char   _esi[6];
  int      _sel;
};

//----------------------------------------------------

/** Class E164_addr.
 */
class E164_addr : public Addr {
  friend class     ie_calling_party_num;
  friend class     ie_calling_party_subaddr;
  friend class     ie_called_party_num;
  friend class     ie_called_party_subaddr;
public:

  /** @name Equality */
  //@{
  /** Determine if this address is equal to another InfoElem object.
   * @return true if they are equal; false otherwise.
   */
  int equals(const InfoElem * him) const;

  /** Determine if this address is equal to another one of the same class.
   * @return true if they are equal; false otherwise.
   */
  virtual int equals(const E164_addr * himptr) const;

  /// Equality operator.
  friend operator==(const E164_addr& addr1, const E164_addr& addr2); 
  //@}

  /// Operator for printing an address.
  friend ostream & operator << (ostream & os, E164_addr & x);

  /** @name Constructors. */
  //@{
  /// Initialize an address from a buffer.
  E164_addr(u_char *E164_addr, int E164_addr_size);

  /// Initialize an address from another address of the same class.
  E164_addr(E164_addr * addr);

  E164_addr(const E164_addr & addr);
  //@}

  /// Destructor.
  virtual ~E164_addr();

  /// Blindly copy the buffer into the address.
  void set_E164_addr(char *buffer, int buflen);

  /// Copy constructor.
  Addr * copy(void) const;

  /** Map an E164_addr into an array of unsigned characters.
   *
   * @param buffer A pointer to an array of unsigned characters.
   *
   * @param buflen The size of the buffer in characters.  On return,
   * contains the number of characters written into the buffer.
   *
   * @return The number of characters written into the buffer.
   */
  int encode(u_char * buffer);

  /** Map an array of unsigned characters into an E164_addr.
   * Fills in the fields of the E164_addr object from the contents 
   * of the array.
   *
   * @param buffer A pointer to an array of unsigned characters.
   *
   * @param buflen The size of the buffer in characters.
   *
   * @return An ie_status enumeration indicating success or failure.
   */
  InfoElem::ie_status decode(u_char *buffer, int & id);

  virtual char PrintSpecific(ostream & os);

  virtual int equals(const E164_addr & him) const; 

  virtual int equals(const Addr & him) const { return equals((const E164_addr&)him); }
  virtual int equals(const Addr * him) const { return equals((const E164_addr*)him); }

protected:

  int    _E164_addr_size;
  u_char _E164_addr[E164_MAX_SIZE];

private:

  E164_addr(void);
};

//----------------------------------------------------


/** Class NSAP_DCC_ICD_addr.
 */
class NSAP_DCC_ICD_addr : public NSAP_addr{
  friend class     ie_calling_party_num;
  friend class     ie_calling_party_subaddr;
  friend class     ie_called_party_num;
  friend class     ie_called_party_subaddr;
public:

  /** @name Equality */
  //@{
  /** Determine if this address is equal to another InfoElem object.
   * @return true if they are equal; false otherwise.
   */
  int equals(const InfoElem * him) const;

  /** Determine if this address is equal to another one of the same class.
   * @return true if they are equal; false otherwise.
   */
  virtual int equals(const NSAP_DCC_ICD_addr * himptr) const;

  /// Equality operator.
  int operator == (const NSAP_DCC_ICD_addr & addr2) const;
  //@}

  /// Operator for printing an address.
  friend ostream & operator <<(ostream & os, NSAP_DCC_ICD_addr &x);

  /** @name Constructors. */
  //@{

  /// Make an address of a specified type.  The default is DCC.
  NSAP_DCC_ICD_addr(afi_type afi = DCC_ATM);

  /// Initialize an address from another one of the same class.
  NSAP_DCC_ICD_addr(NSAP_DCC_ICD_addr * addr);

  NSAP_DCC_ICD_addr(const NSAP_DCC_ICD_addr & addr);

  /// Initialize an address from its component parts.
  NSAP_DCC_ICD_addr(afi_type afi, u_char *dcc_icd, u_char *ho_dsp, u_char *esi, int sel); 

  /** Initialize an address from a buffer.
   * @see UNI_MSG for example of string.
   */
  NSAP_DCC_ICD_addr(char *str);
  //@}

  /// Destructor.
  virtual ~NSAP_DCC_ICD_addr();

  /** Map an NSAP_DCC_ICD_addr into an array of unsigned characters.
   *
   * @param buffer A pointer to an array of unsigned characters.
   *
   * @param buflen The size of the buffer in characters.  On return,
   * contains the number of characters written into the buffer.
   *
   * @return The number of characters written into the buffer.
   */
  int  encode(u_char * buffer);

  /** Map an array of unsigned characters into an NSAP_DCC_ICD_addr.
   * Fills in the fields of the NSAP_DCC_ICD_addr object from the contents 
   * of the array.
   *
   * @param buffer A pointer to an array of unsigned characters.
   *
   * @param buflen The size of the buffer in characters.
   *
   * @return An ie_status enumeration indicating success or failure.
   */
  InfoElem::ie_status decode(u_char *buffer, int &);

  /// Copy constructor.
  Addr * copy(void) const;

  /// Duplicate this address
  NSAP_addr * nsap_addr_copy(void) const;

  /// Creates an address from a string.
  friend Addr * newAddr(char *string);

  virtual char PrintSpecific(ostream & os);

  virtual int equals(const NSAP_DCC_ICD_addr & him) const;

  virtual int equals(const Addr & him) const { return equals((const NSAP_DCC_ICD_addr&)him); }
  virtual int equals(const Addr * him) const { return equals((const NSAP_DCC_ICD_addr*)him); }

protected:

  u_char _dcc_icd[4];
  u_char _ho_dsp[10];
};

//----------------------------------------------------


/** Class NSAP_E164_addr.
 */
class NSAP_E164_addr : public NSAP_addr{
  friend class     ie_calling_party_num;
  friend class     ie_calling_party_subaddr;
  friend class     ie_called_party_num;
  friend class     ie_called_party_subaddr;
 public:

  /** @name Equality */
  //@{
  /** Determine if this address is equal to another InfoElem object.
   * @return true if they are equal; false otherwise.
   */
  int equals(const InfoElem * him) const;

  /** Determine if this address is equal to another one of the same class.
   * @return true if they are equal; false otherwise.
   */
  virtual int equals(const NSAP_E164_addr * himptr) const;

  /// Equality operator.
  friend operator==(const NSAP_E164_addr& addr1, const NSAP_E164_addr& addr2);
  //@}

  /// Operator for printing an address.
  friend ostream & operator <<(ostream & os, NSAP_E164_addr &x);


  /** @name Constructors. */
  //@{

  /// Make an address of a specified type.  The default is DCC.
  NSAP_E164_addr(afi_type afi = E164_ATM);

  /// Initialize an address from another one of the same class.
  NSAP_E164_addr(NSAP_E164_addr * addr);

  NSAP_E164_addr(const NSAP_E164_addr & addr);

  /// Initialize an address from its component parts.
  NSAP_E164_addr(afi_type afi, u_char *ho_dsp, u_char *idi, int idilen,
		 u_char *esi, int sel);

  //@}

  /// Destructor.
  virtual ~NSAP_E164_addr();

  /** Map an NSAP_E164_addr into an array of unsigned characters.
   *
   * @param buffer A pointer to an array of unsigned characters.
   *
   * @param buflen The size of the buffer in characters.  On return,
   * contains the number of characters written into the buffer.
   *
   * @return The number of characters written into the buffer.
   */
  int encode(u_char * buffer);

  /** Map an array of unsigned characters into an NSAP_E164_addr.
   * Fills in the fields of the NSAP_E164_addr object from the contents 
   * of the array.
   *
   * @param buffer A pointer to an array of unsigned characters.
   *
   * @param buflen The size of the buffer in characters.
   *
   * @return An ie_status enumeration indicating success or failure.
   */
  InfoElem::ie_status decode(u_char *buffer, int &);

  /// Copy constructor.
  Addr * copy(void) const;

  /// Duplicate this address.
  NSAP_addr * nsap_addr_copy(void) const;

  /// Create an address from a string.
  friend Addr * newAddr(char *string);

  virtual char PrintSpecific(ostream & os);

  virtual int equals(const NSAP_E164_addr & him) const;

  virtual int equals(const Addr & him) const { return equals((const NSAP_E164_addr&)him); }
  virtual int equals(const Addr * him) const { return equals((const NSAP_E164_addr*)him); }

protected:
  
  int  _E164_addr_size;
  u_char _E164_addr[E164_MAX_SIZE];
  u_char _ho_dsp[4];
};

#endif  // _ADDR_H_

