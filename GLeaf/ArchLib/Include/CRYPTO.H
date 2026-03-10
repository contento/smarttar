/*= Archive Library v2.12 ================================================
 |
 |  H\CRYPTO.H
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  CRYPTO.H contains the class declaration for ALCrypt, the base class
 |  used by all ArchiveLib encryption and decryption engines.
 |
 +- Classes --------------------------------------------------------------
 |
 +=======================================================================*/

#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#ifdef __cplusplus

/*- Class Name -----------------------------------------------------------
 |  ALCrypt
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  ALCrypt is a base class with pure virtual functions, so it cannot be
 |  instantiated.  ALCrypto doesn't do any compression or decompression
 |  itself, but it does provide a few data members that keep track of the
 |  compression type and status for derived classes.
 |
 +- Data Members ---------------------------------------------------------
 |  miCryptoType        -  The type of encryption.  This enumerated value
 |                         is not used much at this point, but we define
 |                         it for all crypto engines used in ArchiveLib.
 |
 |  mszCryptoType       -  This string just describes the crypto type
 |                         in ASCII format suitable for printing or display.
 |
 |  mStatus             -  A standard ArchiveLib status object, stored
 |                         with the crypto engine.  Check this after
 |                         completing a crypto session to see how things
 |                         went.
 |
 +- Member Functions -----------------------------------------------------
 |  ALCrypto()             - The constructor.  Only called by the ctors for
 |                           derived classes.
 |  ~ALCrypto()            - The virtual destructor.
 |  operator new()         - Memory allocation operator, only gets used
 |                           when the library is in a DLL.
 |  StartEncrypt()         - A virtual function called before attaching a
 |                           crypto engine to a storage object.  It
 |                           performs any setup necessary.  The base
 |                           class version doesn't do anything.
 |  Encrypt()              - The virtual function that actually does the
 |                           encryption.  It is called with variable size
 |                           chunks of data to be encrypted.
 |  EndEncrypt()           - A virtual function called after an entire
 |                           storage object has been encrypted.  It's
 |                           responsible for any cleanup.
 |  StartDecrypt()         - The mirror image of StartEncrypt().  Called
 |                           just before attaching the engine to a
 |                           storage object.
 |  Decrypt()              - The virtual function that actually does the
 |                           decryption.  It is called with variable size
 |                           chunks of data to be encrypted.
 |  EndDecrypt()           - A virtual function called after an entire
 |                           storage object has been decrypted.  It's
 |                           responsible for any cleanup.
 |  ClearError()           - Sets the status member of an encryption object
 |                           to AL_SUCCESS.
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

class AL_CLASS_TYPE ALStorage;

// Tag public class
class AL_CLASS_TYPE ALCrypto {
 public :
  AL_PROTO ALCrypto(ALCryptoType crypto_type,
                    const char AL_DLL_FAR *crypto_type_string);
  virtual AL_PROTO ~ALCrypto();
#if defined(AL_USING_DLL) || defined(AL_BUILDING_DLL)
  void AL_DLL_FAR * AL_PROTO operator new(size_t size);
#endif

// The copy constructor and assignment operator do not exist.  I define
// them here to prevent the compiler from creating default versions.

 protected :
  AL_PROTO ALCrypto(ALCrypto AL_DLL_FAR &);
  ALCrypto AL_DLL_FAR & AL_PROTO operator=(ALCrypto AL_DLL_FAR & rhs);

// public members

 public :
  virtual int AL_PROTO StartEncrypt(ALStorage AL_DLL_FAR &input,
                                    ALStorage AL_DLL_FAR &output);
  virtual int AL_PROTO StartDecrypt(ALStorage AL_DLL_FAR &input,
                                    ALStorage AL_DLL_FAR &output);
  virtual void AL_PROTO Encrypt(unsigned char AL_DLL_FAR *buffer,
                                size_t length) = 0;
  virtual void AL_PROTO Decrypt(unsigned char AL_DLL_FAR *buffer,
                                size_t length) = 0;
  virtual int AL_PROTO EndEncrypt(ALStorage AL_DLL_FAR &input,
                                  ALStorage AL_DLL_FAR &output);
  virtual int AL_PROTO EndDecrypt(ALStorage AL_DLL_FAR &input,
                                  ALStorage AL_DLL_FAR &output);
  virtual ALCrypto AL_DLL_FAR * AL_PROTO Clone(int object_type) const = 0;
  void AL_PROTO ClearError();

// date members

 public :
  const ALCryptoType miCryptoType;
  const char AL_DLL_FAR *mszCryptoType;
  ALStatus mStatus;
  AL_CLASS_TAG(_ALCryptoTag);
};

#include "crypto.inl"

extern "C" {

#endif  // #ifdef __cplusplus

AL_LINKAGE void AL_FUNCTION deleteALCrypto(hALCrypto this_object);

#ifdef __cplusplus
};
#endif

#endif // #ifndef __CRYPTO_H__
