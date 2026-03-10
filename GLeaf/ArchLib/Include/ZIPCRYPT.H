/*= Archive Library v2.12 ================================================
 |
 |  H\ZIPCRYPT.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  ZIPCRYPT.H contains the class definition for ALZipCrypto, the ZIP
 |  compatible encryption class.
 |
 +- Classes --------------------------------------------------------------
 |  ALZipCrypto
 |
 +=======================================================================*/

#ifndef __ZIPCRYPT_H__
#define __ZIPCRYPT_H__

#include "crypto.h"

#ifdef __cplusplus

/*- Class Name -----------------------------------------------------------
 |  ALZipCrypto
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  This class is derived from the base class ALCrypto.  It implements Zip
 |  compatible encryption using a variable length key.  If you are reading
 |  this outside of the US, you will not receive the source for this class
 |  due to export restrictions.
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
 |  ALZipCrypto()          - The constructor.  Pass the key here.
 |  ~ALZipCrypto()         - The virtual destructor.
 |  operator new()         - Memory allocation operator, only gets used
 |                           when the library is in a DLL.
 |  StartEncrypt()         - A virtual function called before attaching a
 |                           crypto engine to a storage object.  It
 |                           writes out the encryption header.
 |  Encrypt()              - The virtual function that actually does the
 |                           encryption.  It is called with variable size
 |                           chunks of data to be encrypted.
 |  StartDecrypt()         - The mirror image of StartEncrypt().  Called
 |                           just before attaching the engine to a
 |                           storage object.
 |  Decrypt()              - The virtual function that actually does the
 |                           decryption.  It is called with variable size
 |                           chunks of data to be encrypted.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public class
class AL_CLASS_TYPE ALZipCrypto : public ALCrypto {
 public :
  AL_PROTO ALZipCrypto(const char AL_DLL_FAR *key);
  virtual AL_PROTO ~ALZipCrypto();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// The copy constructor and assignment operator do not exist.  I define
// them here to prevent the compiler from creating default versions.

 protected :
  AL_PROTO ALZipCrypto(ALZipCrypto AL_DLL_FAR &);
  ALZipCrypto AL_DLL_FAR & AL_PROTO operator=(ALZipCrypto AL_DLL_FAR & rhs);
 public :
  virtual int AL_PROTO StartEncrypt(ALStorage AL_DLL_FAR &input, ALStorage AL_DLL_FAR &output);
  virtual int AL_PROTO StartDecrypt(ALStorage AL_DLL_FAR &input, ALStorage AL_DLL_FAR &output);
  virtual void AL_PROTO Encrypt(unsigned char AL_DLL_FAR *buffer, size_t length);
  virtual void AL_PROTO Decrypt(unsigned char AL_DLL_FAR *buffer, size_t length);
  virtual ALCrypto AL_DLL_FAR * AL_PROTO Clone(int object_type) const;
 protected :
  int AL_PROTO CurrentXorByte();
  void AL_PROTO InitKeys();
  void AL_PROTO UpdateKeys(int c);
  char AL_DLL_FAR *mcpPassword;
  unsigned long mcpKeys[3];
 public :
  AL_CLASS_TAG(_ALZipCryptoTag);
};

extern "C" {

#endif // #ifdef __cplusplus

AL_LINKAGE hALCrypto AL_FUNCTION newALZipCrypto(const char AL_DLL_FAR *key);

#ifdef __cplusplus
};
#endif // #ifdef __cplusplus

#endif // #ifndef __ZIPCRYPT_H__
