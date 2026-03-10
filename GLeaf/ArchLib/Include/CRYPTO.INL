/*= Archive Library v2.12 ================================================
 |
 |  H\CRYPTO.INL
 |  Copyright (c) 1994-1997, Greenleaf Software, Inc.
 |  All Rights Reserved.
 |
 +- Description ----------------------------------------------------------
 |  Inline functions for base class ALCrypto.
 |
 +- Functions ------------------------------------------------------------
 |  ALCrypto::StartEncrypt()
 |  ALCrypto::StartDecrypt()
 |  ALCrypto::EndEncrypt()
 |  ALCrypto::EndDecrypt()
 |  ALCrypto::ClearError()
 |
 +=======================================================================*/

/*- Function Name---------------------------------------------------------
 |  ALCrypto::StartEncrypt()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Most encryption engines have some initialization that needs to be
 |  done at startup.  This virtual function does it.  The base class
 |  version, shown here, does nothing.
 |
 +- Arguments ------------------------------------------------------------
 |  ALStorage &  -  A reference to an ALStorage object.  This file will be
 |                  the source of the data to be encrypted.  The only reason
 |                  this parameter is needed in the virtual function is that
 |                  PKWare compression needs the file's time stamp as part
 |                  of the encryption header.
 |
 |  ALStorage &  -  A reference to an ALStorage object.  This fill will be
 |                  the one receiving the encrypted data.  PKWare compression
 |                  writes a 12 byte encryption header to the output file,
 |                  thus making this parameter necessary.
 |
 +- Return Value ---------------------------------------------------------
 |  AL_SUCCESS
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline int AL_INLINE_PROTO
ALCrypto::StartEncrypt(ALStorage AL_DLL_FAR &,
                       ALStorage AL_DLL_FAR &)
{
 return AL_SUCCESS;
}

/*- Function Name---------------------------------------------------------
 |  ALCrypto::StartDecrypt()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  Most decryption engines have some initialization that needs to be
 |  done at startup.  This virtual function does it.  The base class
 |  version, shown here, does nothing.
 |
 +- Arguments ------------------------------------------------------------
 |  ALStorage &  -  A reference to an ALStorage object.  This file will be
 |                  the source of the data to be decrypted.  At this time,
 |                  PKWare has to read in the encryption header and mangle
 |                  the keys from this file.
 |
 |  ALStorage &  -  A reference to an ALStorage object.  This file will be
 |                  the one receiving the decrypted data.  At this time,
 |                  none of the derived encryption classes do anything with
 |                  the output storage object in their implementations of
 |                  this function.
 |
 +- Return Value ---------------------------------------------------------
 |  AL_SUCCESS
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline int AL_INLINE_PROTO
ALCrypto::StartDecrypt(ALStorage AL_DLL_FAR &,
                       ALStorage AL_DLL_FAR &)
{
 return AL_SUCCESS;
}

/*- Function Name---------------------------------------------------------
 |  ALCrypto::EndEncrypt()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  If an encryption engine needs to do any cleanup when it completes
 |  it's encryption, it can do it here.  None of the presently defined
 |  derived classes implement this function.
 |
 +- Arguments ------------------------------------------------------------
 |  ALStorage &  -  A reference to an ALStorage object.  This file was the
 |                  source of the data to be encrypted.  None of the
 |                  encryption engines currently defined for ArchiveLib do
 |                  anything with the input storage object in their
 |                  implementations of this function.
 |
 |  ALStorage &  -  A reference to an ALStorage object.  This file was the
 |                  destination of the encrypted data.  None of the
 |                  encryption engines currently defined for ArchiveLib do
 |                  anything with the output storage object in their
 |                  implementations of this function.
 |
 +- Return Value ---------------------------------------------------------
 |  AL_SUCCESS
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline int AL_INLINE_PROTO
ALCrypto::EndEncrypt(ALStorage AL_DLL_FAR &,
                     ALStorage AL_DLL_FAR &)
{
 return AL_SUCCESS;
}

/*- Function Name---------------------------------------------------------
 |  ALCrypto::EndDecrypt()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  If an decryption engine needs to do any cleanup when it completes
 |  it's decryption, it can do it here.  None of the presently defined
 |
 +- Arguments ------------------------------------------------------------
 |  ALStorage &  -  A reference to an ALStorage object.  This file was the
 |                  source of the encrypted data.  None of the encryption
 |                  engines currently defined for ArchiveLib do anything
 |                  with the input storage object in their implementations
 |                  of this function.
 |
 |  ALStorage &  -  A reference to an ALStorage object.  This file was the
 |                  destination of the unencrypted data.  None of the
 |                  encryption engines currently defined for ArchiveLib do
 |                  anything with the output storage object in their
 |                  implementations of this function.
 |
 +- Return Value ---------------------------------------------------------
 |  AL_SUCCESS
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline int AL_INLINE_PROTO
ALCrypto::EndDecrypt(ALStorage AL_DLL_FAR &,
                     ALStorage AL_DLL_FAR &)
{
 return AL_SUCCESS;
}

/*- Function Name---------------------------------------------------------
 |  ALCrypto::ClearError()
 |
 +- Version --------------------------------------------------------------
 |  2.11.00 01 Oct 1996 12:00
 |
 +- Description ----------------------------------------------------------
 |  An ALCrypto object carries around a status object in its mStatus
 |  member.  For various reasons, this member might get set to an error
 |  condition.  Error conditions aren't cleared automatically by the library,
 |  so the user will have to manually clear it with a call to this function.
 |
 |  This is a real simple function, so in C++ it will be implemented as
 |  an inline function.  The rest of the supported languages don't have
 |  this luxury.
 |
 +- Arguments ------------------------------------------------------------
 |  None
 |
 +- Return Value ---------------------------------------------------------
 |  AL_SUCCESS
 |
 +- Revision History -----------------------------------------------------
 |
 +-----------------------------------------------------------------------*/

// Tag public function
inline void AL_INLINE_PROTO
ALCrypto::ClearError()
{
 mStatus.SetError(AL_SUCCESS, 0);
}
