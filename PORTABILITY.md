# Portability Seams — Catalogue

> Phase 2.2 deliverable of the mini-smarttar reduction plan.
> Last updated: 2026-06-10.

This document names, locates, and rates every platform-dependent surface in the
SmartTar codebase.  The goal is not to fix them *now* but to make them visible so
a future port (new compiler, new OS, or new language) has a checklist.

---

## 1. Runtime Environment: DOS / Pharlap 286

The deepest seam.  SmartTar runs in DOS protected mode under the Pharlap 286
DOS-Extender (`BIND286` + `CFIG286`).  A modern target is flat 32- or 64-bit;
everything below is invisible there.

|#|What|Where|Effort|
|-|----|-----|------|
|1.1|**Protected-mode startup** — `c0pl.obj` (Pharlap startup), `st.def` (segment/stack), `BIND286`/`CFIG286` bind step|`MAKEFILE` `STARTUP_OBJS`, `DEF_FILE`, link step|High — the linker, extender, and segment model all change.|
|1.2|**`far` pointers** — Borland/Pharlap `far` qualifier on code and data pointers throughout the app.  A flat-memory target has one address space; `far` is either a NOP (Win64) or a compile error.|Every file that declares a callback, a Zinc callback, or a hardware port address. Search: `far` keyword (grep).|Medium — mechanical removal once the toolchain is confirmed flat.|
|1.3|**`BIND286` / `run286b`** runtime — the Pharlap RUN module ships with the EXE.  A modern OS needs a different loader (or none).|`MAKEFILE` `$(BIND)` step|High — replaced by the native OS loader.|

---

## 2. Compiler: Borland C++ 3.1

Borland C++ 3.1 is a pre-ISO C++ compiler.  The code exploits several quirks.

|#|What|Where|Effort|
|-|----|-----|------|
|2.1|**No `std` namespace** — all library functions (`strcpy`, `memset`, `fopen`, etc.) are global.  This is how C++98 shipped, so any compiler from 1995-2010 accepts it; C++23 modules builds would need wrapping.|Throughout|Low — a compatibility header, or accept the global-namespace style.|
|2.2|**No STL** — no `std::vector`, `std::string`, etc.  Dynamic arrays are hand-rolled with `malloc`/`realloc`/`free`.  The new `CsvStorage` follows this pattern (`csv_stor.cpp`).|`dstorage.cpp` (IndexCache), `csv_stor.cpp` (Entry array), `dstatist.cpp`, `mb_conf.cpp`|Low — the hand-rolled arrays are simple; a port can replace them with STL containers when available.|
|2.3|**No exceptions, no RTTI** — error handling is return-code / status-flag throughout.  `new` returns `NULL` on failure (Borland C++ 3.1 does not throw `std::bad_alloc`).|Every allocation site|Low — return-code style is compatible with every environment.|
|2.4|**`BOOl` as `int`** — `BOOL` is `typedef int BOOL`.  `TRUE`/`FALSE` are 1/0.  A `bool`-native compiler should map `BOOL` → `bool` at the boundary and accept implicit int→bool elsewhere.|`st_defs.h`|Low — a typedef swap.|

---

## 3. UI: Zinc 3.5 (proprietary, DOS-only, 16-bit)

The largest single port cost.  Zinc 3.5 is a DOS-only, Borland-specific, 16-bit
GUI framework.  Replacing it is a full rewrite of the UI layer.

|#|What|Where|Effort|
|-|----|-----|------|
|3.1|**Zinc class hierarchy** — `UIW_WINDOW`, `UIW_STRING`, `UIW_BUTTON`, etc. are the base of every screen.  Every view, dialog, and status bar inherits from a Zinc class.|`src/ui/`, `src/mb/`, `src/tb/`|Very high — the entire UI layer is Zinc.|
|3.2|**Zinc event loop** — `WINDOW_MANAGER::Event` drives the application.  Controller hooks into Zinc's post-event dispatch.|`ctrl/control.cpp`, `ctrl/ctrl_ev.cpp`|High — the event model changes entirely.|
|3.3|**Zinc resource file** — `RES.DAT` is a binary Zinc resource compiled by the Zinc Designer.  Layout, strings, and some behavior are defined there, not in code.|`res/res.dat`, `MAKEFILE` copy rule|Medium — can be decompiled to text (research exists in MINI_SMARTTAR_PLAN § 8).|
|3.4|**Zinc callbacks** — `ZIL_USERFUNCTION` and `ZFV_PTR` are used in views and event handling.|`src/ui/view_ev.cpp`, `src/ui/w_table.cpp`|Medium — each callback is small but the pattern is deep.|

---

## 4. DOS File / Console / Printer I/O

|#|What|Where|Effort|
|-|----|-----|------|
|4.1|**`_dos_open`/`_dos_close`/`_dos_read`/`_dos_write`** — raw DOS file handles used for receipt data files (performance / seek control).|`dstorage.cpp`, `dstatist.cpp`, `d_ext_st.cpp`|Medium — replace with `fopen`/`fread`/`fwrite` or the OS's native file API.|
|4.2|**`lseek`/`read`/`write`** on file descriptors — POSIX-ish but not ISO C.  Used in binary data/index files.|`dstorage.cpp`|Medium — same as 4.1.|
|4.3|**`fopen`/`fscanf`/`fprintf`/`fgets`** — standard C I/O, portable in principle but the encoding assumption differs (see § 5).|`cfg.cpp`, `csv_stor.cpp`, `ph_eng.cpp`, `st_util.cpp`|Low — standard C I/O is available everywhere.|
|4.4|**Spooler / printer DLLs** — `src/pr/*.c` compiles to `.dll` (Pharlap 286 DLLs).  These drive receipt printers via LPT/COM.  Printing depends on DOS device access.|`src/pr/*.c`, `prn_fmt.cpp`, `spooler.cpp`|High — printer architecture is DOS-native.|
|4.5|**BIOS/COM port access** — `_bios_serialcom`, outportb/inportb, and direct hardware port I/O for the real engine.|`real_dos/rt_isr.cpp`, `real_dos/rt_eng.cpp`, `core/serial.cpp`, `dongle.cpp`, `eeprom.cpp`|High — hardware-dependent; these files are already isolated in `real_dos/`.|

---

## 5. Encoding: Latin-1 / CP850

|#|What|Where|Effort|
|-|----|-----|------|
|5.1|**Source encoding** — All `.cpp`/`.h` files are Latin-1 (ISO‑8859-1) or CP850.  UTF-8 editors re-encode high-bit characters on save, corrupting strings.|Every `.cpp`/`.h` file|Low — set the editor to Latin-1 mode, or re-encode the whole tree in a migration step.|
|5.2|**Runtime encoding bridge** — `_ISO2ASCII` and related functions convert between Latin-1 internal strings and CP850 console/printer output.  A Unicode target needs no conversion.|`st_util.cpp`, `st_util.h`|Low — remove the conversion at the I/O boundary.|
|5.3|**INI / CSV files** — `ST.INI` and the new `.csv` receipt files are also Latin-1.  A UTF-8 target reads them as-is if the characters are in the ASCII subset, or needs a transcoding step.|`cfg.cpp` (`ST.INI`), `csv_stor.cpp` (`.csv`)|Low — document the assumed encoding.|

---

## 6. Timing / ISR / PIT

These are already isolated in `real_dos/` and not linked in the demo build.

|#|What|Where|Effort|
|-|----|-----|------|
|6.1|**PIT (8253/8254) programming** — `SetPITRate` reprograms the timer chip for 1-ms ticks.  Real-time call dispatching depends on this.|`real_dos/rt_isr.cpp`|High — hardware-specific; already behind `real_dos/` gate.|
|6.2|**ISR (interrupt service routine)** — hooked into the timer interrupt chain.  Accesses `volatile` cluster state from C++.|`real_dos/rt_isr.cpp`|High — full `volatile`/atomicity audit needed before optimization (see `ISR_VOLATILE_NOTES.md`).|
|6.3|**Demo synthetic timing** — `DEMO_ENGINE` uses `clock()` and a Poisson arrival generator; no hardware dependency.|`demo_dos/demo_eng.cpp`|Low — `clock()` is ISO C.|

---

## 7. Build Toolchain

|#|What|Where|Effort|
|-|----|-----|------|
|7.1|**Borland MAKE 3.6** — proprietary make syntax (`!.if $d()`, `!else`, suffix rules).  Not GNU Make compatible.|`MAKEFILE`|Medium — translate to CMake or GNU Make for a new compiler.|
|7.2|**`bcc286`** — Borland C++ 3.1 for 286 protected mode.  No other compiler produces a compatible object format.|`MAKEFILE` `CC`|High — the compiler is the toolchain.  Must switch to GCC, Clang, MSVC, or similar.|
|7.3|**`tasm`** — Borland assembler for 286/386 real+protected mode.  Used for the ISR entry trampoline.|`MAKEFILE` `ASM`, `src/core/rt_isr.cpp` (inline asm), any `.asm` files|Medium — inline asm can be ported to the target assembler; ISR entry rewritten in C with compiler intrinsics.|
|7.4|**`tlink`** — Borland linker.  Produces Pharlap-format `.exe`.|`MAKEFILE` `LINK`|High — replaced when the compiler/target changes.|
|7.5|**DOSBox-X** — required to build even the demo variant, because the toolchain is DOS-native and the 16-bit output runs on DOS.|`build.sh`, `build.ps1`|High — goes away when the toolchain itself is native.|

---

## 8. Data Formats

|#|What|Where|Effort|
|-|----|-----|------|
|8.1|**`RX.DAT` / `RX.IDX` (binary)** — seek-indexed binary receipt store.  The legacy format, now behind `BinStorage`.|`dstorage.cpp`, `dstorage.h`|Low — `BinStorage` is one implementation of `DB_STORAGE_BACKEND`; can be read for migration then dropped.|
|8.2|**`RX.CSV` (CSV)** — the new default (since mini-smarttar).  Human-readable, trivially portable, one row per receipt.|`csv_stor.cpp`, `csv_stor.h`|Low — CSV is universally readable.|
|8.3|**`PH_INFO.DAT` (binary)** — compiled telephony place database.  Eliminated in mini-smarttar; `PH_ENGINE::Load()` falls back to `.inf` files directly.|`ph_eng.cpp`, `ph_util.cpp`|None — already removed.|
|8.4|**`.inf` files** — telephony data in human-readable INI-style format.  Portable, shared between builds.|`src/ph/*.cpp`, `*.inf` files|Low — plain text.|
|8.5|**`ST.INI` (INI)** — single human-editable config source (since mini-smarttar).  No binary `ST.CFG`.|`cfg.cpp`, `st.ini`|Low — INI is universally parsable.|
|8.6|**`RES.DAT` (Zinc binary resource)** — GUI layout/strings.  Requires Zinc Designer to edit.|`res/res.dat`|Medium — can be decompiled (research pending).|

---

## 9. Summary by effort

|Effort|Count|Items|
|------|-----|-----|
|Very High|1|3.1 Zinc class hierarchy|
|High|9|1.1 Pharlap startup, 1.3 bind module, 3.2 Zinc event loop, 4.4 printer DLLs, 4.5 hardware I/O, 6.1 PIT, 6.2 ISR, 7.2 compiler, 7.4 linker, 7.5 DOSBox-X|
|Medium|5|1.2 `far` pointers, 3.3 RES.DAT, 3.4 Zinc callbacks, 4.1 DOS file API, 4.2 `lseek`/`read`/`write`, 7.1 MAKE syntax, 7.3 assembler|
|Low|12|2.1–2.4 compiler quirks, 4.3 standard C I/O, 5.1–5.3 encoding, 6.3 demo timing, 8.1–8.6 data formats|
