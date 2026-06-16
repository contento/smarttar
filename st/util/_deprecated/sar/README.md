# SAR — Secure RAR Compression Wrapper

**Status:** DEPRECATED — Superseded by modern encryption utilities.

**Purpose:** Encrypts password and invokes RAR32.EXE with the password for secure RAR archive creation.

## Overview

SAR is a legacy wrapper utility that embeds a hardcoded encrypted password and passes it to RAR32.EXE for password-protected RAR file creation. It was used for creating encrypted archives of tariff updates and configuration during distribution.

**Usage:**
```
sar archive_name file1 file2 file3 ...
```

**Workflow:**
1. Decrypts the hardcoded password (from CFG.CPP encryption scheme)
2. Builds RAR command: `rar32 -p<password> archive_name [files]`
3. Spawns RAR32.EXE with encrypted password argument
4. Returns exit code from RAR32

**Password scheme:** XOR-encrypted using `_Decrypt()` in st_util.cpp. The encrypted bytes `\x4E\x74\x78\x12` decode to the master password.

**Why deprecated:** 
- RAR32 licensing is no longer in use
- Modern distribution uses self-extracting archives (STX.EXE from GEN)
- Modern encryption (AES) is preferred over XOR-based masking
- Direct password handling in command line is a security risk
- WinRAR support ended for DOS era tools

**Historical context:** Useful for understanding password handling patterns (CFG.CPP, `_Decrypt()`, `_Encrypt()`) and format comparison (RAR vs ZIP) but should not be used for new code.

**References:**
- CFG.CPP — password encryption/decryption scheme (see `_Encrypt()`, `_Decrypt()`)
- SIP.CPP — identical pattern but for PKZIP.EXE
- GEN.CPP — successor for secure distribution packaging
