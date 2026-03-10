/*= Archive Library v2.12 ================================================
 |
 |  H\XORCRYPT.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  XORCRYPT.H contains the class declaration for ALXorCrypto, a simple
 |  encryption class.
 |
 +- Classes --------------------------------------------------------------
 |  ALXorCrypto
 |
 +=======================================================================*/

#ifndef __XORCRYPT_H__
#define __XORCRYPT_H__

#include "crypto.h"

#ifdef __cplusplus

/*- Class Name -----------------------------------------------------------
 |  ALXorCrypto
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  This class is derived from the base class ALCrypto.  It implements
 |  simple XOR based encryption using a variable length key.  Yes, this
 |  means that it is about as far from bullet-proof as you can get.  It
 |  will provide some discouragement to prying eyes, though.
 |
 +- Data Members ---------------------------------------------------------
 |  mpcKey           -  The key passed in when the crypto engine
 |                      is constructed.  I make a local copy of it
 |                      in storage allocated in the ctor, and destroyed
 |                      in the dtor.
 |  mpcCurrentOffset -  During the actual encryption process, this points
 |                      to the next character that will be used to XOR
 |                      a data byte.  It is incremented on every character,
 |                      and reset to 0 when it reaches the end, singled
 |                      by a '\0' terminator.
 |
 +- Member Functions -----------------------------------------------------
 |  ALXorCrypto()         -  The constructor.  Pass the key here.
 |  ~ALXorCrypto()        -  The virtual destructor.
 |  operator new()        -  Memory allocation operator, only gets used
 |                           when the library is in a DLL.
 |  StartEncrypt()        -  A virtual function called before attaching a
 |                           crypto engine to a storage object.  It
 |                           sets up the key pointer.
 |  Encrypt()             -  The virtual function that actually does the
 |                           encryption.  It is called with variable size
 |                           chunks of data to be encrypted.
 |  StartDecrypt()        -  The mirror image of StartEncrypt().  Called
 |                           just before attaching the engine to a
 |                           storage object.
 |  Decrypt()             -  The virtual function that actually does the
 |                           decryption.  It is called with variable size
 |                           chunks of data to be encrypted.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public class
class AL_CLASS_TYPE ALXorCrypto : public ALCrypto {
 public :
  AL_PROTO ALXorCrypto(const char AL_DLL_FAR *key);
  virtual AL_PROTO ~ALXorCrypto();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// The copy constructor and assignment operator do not exist.  I define
// them here to prevent the compiler from creating default versions.

 protected :
  AL_PROTO ALXorCrypto(ALXorCrypto AL_DLL_FAR &);
  ALXorCrypto AL_DLL_FAR & AL_PROTO operator=(ALXorCrypto AL_DLL_FAR & rhs);
 public :
  virtual int AL_PROTO StartEncrypt(ALStorage AL_DLL_FAR &input, ALStorage AL_DLL_FAR &output);
  virtual int AL_PROTO StartDecrypt(ALStorage AL_DLL_FAR &input, ALStorage AL_DLL_FAR &output);
  virtual void AL_PROTO Encrypt(unsigned char AL_DLL_FAR *buffer, size_t length);
  virtual void AL_PROTO Decrypt(unsigned char AL_DLL_FAR *buffer, size_t length);
  virtual ALCrypto AL_DLL_FAR * AL_PROTO Clone(int object_type) const;
 protected :
  char AL_DLL_FAR *mcpKey;
  char AL_DLL_FAR *mcpCurrentOffset;
 public :
  AL_CLASS_TAG(_ALXorCryptoTag);
};

extern "C" {

#endif  // #ifdef __cplusplus

AL_LINKAGE hALCrypto AL_FUNCTION newALXorCrypto(const char AL_DLL_FAR *key);

#ifdef __cplusplus
};
#endif

#endif // #ifndef __XORCRYPT_H__
