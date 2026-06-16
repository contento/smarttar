# SIP — Secure ZIP Compression Wrapper

**Status:** DEPRECATED — Superseded by modern encryption utilities.

**Purpose:** Encrypts password and invokes PKZIP.EXE with the password for secure ZIP archive creation.

## Overview

SIP is a legacy wrapper utility that embeds a hardcoded encrypted password and passes it to PKZIP.EXE for password-protected ZIP file creation. It was used during distribution packaging to create encrypted archives of tariff updates and configuration files.

**Usage:**
```
sip archive_name file1 file2 file3 ...
```

**Workflow:**
1. Decrypts the hardcoded password (from CFG.CPP encryption scheme)
2. Builds PKZIP command: `pkzip -s<password> archive_name [files]`
3. Spawns PKZIP.EXE with encrypted password argument
4. Returns exit code from PKZIP

**Password scheme:** XOR-encrypted using `_Decrypt()` in st_util.cpp. The encrypted bytes `\x4E\x74\x78\x12` decode to the master password.

**Why deprecated:** 
- Modern distribution uses self-extracting archives (STX.EXE from GEN)
- PKZIP licensing is no longer maintained
- Modern encryption (AES) is preferred over XOR-based masking
- Direct password handling in command line is a security risk

**Historical context:** Useful for understanding password handling patterns (CFG.CPP, `_Decrypt()`, `_Encrypt()`) but should not be used for new code.

**References:**
- CFG.CPP — password encryption/decryption scheme (see `_Encrypt()`, `_Decrypt()`)
- SAR.CPP — identical pattern but for RAR32.EXE
- GEN.CPP — successor for secure distribution packaging
