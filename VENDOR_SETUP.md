# Vendor Setup

SmartTar requires proprietary toolchain binaries that are **not included** in
the main repository. These live in a separate private repository:

> **[`smarttar-vendor`](https://github.com/contento/smarttar-vendor)** (private)

The separation exists to avoid copyright / redistribution issues with the
third-party toolchain components.

---

## What's in vendor/

| Directory      | Contents |
| -------------- | -------- |
| `vendor/bc/`   | **Borland C++ 3.1** — compiler (`BCC`/`BCC286`), debugger (`TD`), assembler (`TASM`), linker (`TLINK`), librarian (`TLIB`), `MAKE`, includes, and libraries |
| `vendor/pharlap/` | **Pharlap 286 v3.0** — DOS extender runtime (`RUN286`, `GORUN286`), binder (`BIND286`), config tool (`CFIG286`), `BCC286` compiler wrapper, includes, libraries, and runtime DLLs |
| `vendor/zinc/` | **Zinc Interface Library 3.5** — UI framework headers, static libraries (`d16_zil.lib`, `d16_gfx.lib`), `GENHELP.EXE` (help compiler), and Zinc Designer |
| `vendor/util/` | **DOS utilities** — Norton Commander, QEdit, PKWARE (PKZIP/PKLITE), ASTYLE, SWEEP, Microsoft Mouse driver (reference only — DOSBox-X handles INT 33h) |

---

## Quick setup

### Option A: Clone the vendor repo (recommended)

```sh
# macOS / Linux
./setup-vendor.sh

# Windows (PowerShell)
.\setup-vendor.ps1
```

This clones `git@github.com:contento/smarttar-vendor.git` into `./vendor/`.

To re-clone from scratch:
```sh
./setup-vendor.sh --force     # bash
.\setup-vendor.ps1 -Force     # PowerShell
```

### Option B: Manual placement

If you can't clone the private repo (no SSH access, corporate firewall, etc.),
you can obtain the components individually and place them under `vendor/`:

1. **Borland C++ 3.1** → `vendor/bc/`
   - Need: `BIN/` (bcc, tasm, tlink, tlib, make, td, ...), `INCLUDE/`, `LIB/`
   - Source: Borland historical archives, eBay, or original installation media

2. **Pharlap 286 v3.0** → `vendor/pharlap/`
   - Need: `BIN/` (bcc286, bind286, cfig286, run286, gorun286, ...),
     `INCLUDE/`, `LIB/` (c0pl.obj, phapi.lib, ...), runtime DLLs
   - Source: Pharlap Inc. historical distribution

3. **Zinc Interface Library 3.5** → `vendor/zinc/`
   - Need: `BIN/` (genhelp.exe, design.exe), `INCLUDE/`, `LIB/` (d16_zil.lib,
     d16_gfx.lib, ...), `SOURCE/`
   - Source: Zinc Development Tools historical distribution

4. **DOS utilities** → `vendor/util/`
   - Norton Commander, QEdit, PKWARE tools, ASTYLE, SWEEP
   - Optional — only needed for interactive DOS development, not for builds

### Required directory structure

```
vendor/
  bc/
    BIN/        compilers, linker, librarian, debugger, MAKE
    INCLUDE/    standard C/C++ headers
    LIB/        standard libraries (emu.lib, mathl.lib, etc.)
  pharlap/
    BIN/        BCC286, BIND286, CFIG286, RUN286, GORUN286
    INCLUDE/    phapi.h, etc.
    LIB/        c0pl.obj, phapi.lib, etc.
    *.DLL       runtime DLLs (bound at link time, shipped with releases)
  zinc/
    BIN/        GENHELP.EXE, DESIGN.EXE (Zinc Designer)
    INCLUDE/    zil*.h, ui*.h, etc.
    LIB/        d16_zil.lib, d16_gfx.lib, etc.
    SOURCE/     Zinc 3.5 source (optional, for reference / bug fixes)
  util/         DOS utilities (optional for builds)
```

---

## Verifying the setup

After cloning or manual placement, verify the toolchain is in place:

```sh
# Check key files exist
ls vendor/bc/bin/bcc.exe          # Borland compiler
ls vendor/pharlap/bin/bcc286.exe  # Pharlap compiler wrapper
ls vendor/zinc/bin/genhelp.exe    # Zinc help compiler
ls vendor/pharlap/lib/c0pl.obj   # Pharlap startup object
```

Or run the build — it will fail early with clear messages if vendor/ is missing:

```sh
./build.sh demo
```

---

## SSH access to the private repo

The vendor repository is private. You need SSH access:

```sh
# Test connectivity
ssh -T git@github.com

# If key isn't loaded
ssh-add ~/.ssh/id_rsa
```

Ask the repository owner to add your GitHub SSH key if you get
`Permission denied (publickey)`.

---

## Why a separate repository?

The vendor binaries (Borland C++ 3.1, Pharlap 286, Zinc 3.5) are
**proprietary, non-redistributable** software. Keeping them in the main
smarttar repository would:

1. **Copyright risk** — the binaries are copyrighted by Borland, Pharlap Inc.,
   and Zinc Development Tools. Tracking them in a public (or even private)
   source repo creates redistribution liability.
2. **Repository size** — the toolchain binaries are ~50 MB. Keeping them
   separate keeps the main repo lightweight for contributors who only need
   the source code.
3. **Clean separation** — first-party code (SmartTar source, build scripts,
   docs) stays cleanly separated from third-party toolchain binaries.

The main repository's `.gitignore` excludes `vendor/`. The build and run
scripts check for its presence and guide you to set it up if missing.
