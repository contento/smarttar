# PORTABILITY.md — Platform Seam Catalogue

> Catalogue of every platform-specific surface in mini-smarttar.
> Created 2026-06-10 as Phase 2.2 deliverable (MINI_SMARTTAR_PLAN).
> A future port cuts along these seams.

**Codebase at snapshot:** 32,603 LOC across 83 `.cpp` / 10 `.c` files.

---

## 1. Zinc 3.5 UI

**Severity: EXTREME — largest single port cost.**

| Where | Files | LOC |
|-------|-------|-----|
| `st/src/ui/` | 8 | 2,605 |
| `st/src/mb/` (menu-bar dialogs) | 7 | 4,036 |
| `st/src/tb/` (toolbar callbacks) | 6 | 1,803 |
| `st/res/res.dat` (binary resource) | 1 | — |

**Total:** ~8,444 LOC (26% of codebase) plus `res.dat`.

**Zinc widget types used:** 60+ distinct `UIW_*` classes (207 `UIW_BUTTON`, 166 `UIW_BIGNUM`, 149 `UIW_INTEGER`, 140 `UIW_STRING`, 133 `UIW_WINDOW`, etc.). Many are SmartTar-specific subclasses (`UIW_PHONE`, `UIW_RECEIPT`, `UIW_SP_SERV`, `UIW_NAL_TAR`, `UIW_INTER_TAR`, etc.).

**What Zinc provides:** event loop, windowing, menus, tables, scroll bars, status bars, dialog boxes, minicell grid layout, focus management, file I/O (via `UI_STORAGE` for `res.dat`).

**Port strategy:** Replace with a target-platform UI toolkit. The `UI_*` abstraction in `st/src/ui/` is the natural boundary — views talk to the engine through `g_dbEngine` and `g_controller`. A rewrite would reimplement `view.cpp`, `w_table.cpp`, `w_statbr.cpp`, and every `mb_*.cpp` / `tb_*.cpp` dialog. The `res.dat` binary resource format is proprietary (Zinc 3.5 `UI_STORAGE`) and would need a decompiler (see `MINI_SMARTTAR_PLAN` §8) or replacement with a declarative layout format.

**Already done:** `st/include/db_stor.h` provides the `DB_STORAGE_BACKEND` interface — storage is already portable.

---

## 2. DOS / Pharlap / Borland C++ Runtime

**Severity: EXTREME — deeply embedded.**

| Where | What |
|-------|------|
| `st/include/stdst.h` | Precompiled header: pulls in `<dos.h>`, `<io.h>`, `<dir.h>`, `<sys/stat.h>`, `<bios.h>`, `<conio.h>` |
| `st/MAKEFILE` | Borland MAKE 3.6 syntax, BCC286 compiler flags |
| `st/st.cfg` | BCC config: `-ml` (large model), `-k-`, Pharlap include/lib paths |
| `st/st.def` | DOS/Pharlap EXE format: `PROTMODE`, `EXETYPE OS2`, `STUB gorun286.exe` |
| `st.cfg` → `BIND286` → `CFIG286` | Post-link binding to Pharlap protected-mode runtime |

**What DOS/Borland provides:**
- 16-bit segmented memory model (`far`/`near` pointers)
- UNIX-style file I/O (`open`, `close`, `read`, `write`, `lseek`)
- C++ stream I/O (`fstream.h`, `iostream.h` — Borland-specific headers)
- `dir.h` functions (`getcwd`, `mkdir`, etc.)
- `bios.h` (`biosprint`)
- `dos.h` (`int86`, `intdos`, `geninterrupt`, `inportb`, `outportb`)

**Port strategy:** Replace the precompiled header and build system. The DOS API surface is concentrated in:
- File I/O: `dstorage.cpp` (42 calls), `dstatist.cpp` (22), `d_ext_st.cpp` (18) — already behind `DB_STORAGE_BACKEND`
- Port I/O: `serial.cpp` (28), `rt_isr.cpp` (7) — in `core/`, isolated behind `ISerial`
- BIOS: `spooler.cpp` (biosprint) — printer output path

The `core/` subtree is the cleanest extraction target: engine-agnostic, hardware behind `I*` interfaces, config from `.ini`, storage via `DB_STORAGE_BACKEND`.

---

## 3. Far Pointers and Segmented Memory

**Severity: HARD — concentrated in printer DLLs and ISR.**

| Area | Files | `far` refs |
|------|-------|------------|
| `st/src/pr/*.c` (printer DLLs) | 10 | 28 each (280 total) |
| `st/src/core/rt_isr.cpp` | 1 | 15 |
| `st/src/core/serial.cpp` | 1 | 2 |
| `st/src/demo_dos/demo_eng.cpp` | 1 | 1 (comment only) |
| `st/src/ctrl/control.cpp` | 1 | 1 |

**Total:** ~299 `far` keyword references across 14 files.

**Key insight:** `demo_dos` has exactly 1 `far` reference (a comment in `demo_eng.cpp`). The real hardware code (`rt_isr.cpp`, `serial.cpp`, `pr/*.c`) is the sole consumer. Since `demo_dos` is the only buildable variant, `far` is effectively dead code for the portable path.

**Printer DLLs (`pr/*.c`):** 10 printer formatter DLLs (PR_DR18, PR_DR40, PR_DR80, PR_DRDREME, PR_DRHAL, PR_DRPRE, PR_LIN80, PR_SR28, PR_SR40, PR_SR80), ~430 LOC each. These are Borland DLLs loaded at runtime via the spooler. Each exports a `Format()` function. The `far` usage is for the DLL calling convention and shared-memory buffer access.

---

## 4. Port I/O (Hardware Registers)

**Severity: EASY — already isolated behind `I*` interfaces.**

| Area | Files | `outportb`/`inportb` refs |
|------|-------|--------------------------|
| `st/src/core/serial.cpp` | 1 | 28 |
| `st/src/real_dos/rt_eng.cpp` | 1 | 10 |
| `st/src/core/rt_isr.cpp` | 1 | 7 |
| `st/src/real_dos/dongle.cpp` | 1 | 6 |
| `st/src/real_dos/eeprom.cpp` | 1 | 6 |
| `st/src/real_dos/bankstm2.cpp` | 1 | 2 |
| `st/src/ct/ct_util.cpp` | 1 | 2 (cash drawer) |

**Port strategy:** Already abstracted. `demo_dos` uses `NullDongle`, `NullEeprom`, `NullStm2`, `NullSerial`, `DEMO_ENGINE` — none of which touch port I/O. The `rt_eng.cpp` and `dongle.cpp` / `eeprom.cpp` / `bankstm2.cpp` files are in `real_dos/` and never compiled in the demo build. The only port I/O in the demo path is `ct_util.cpp` (cash drawer command, 2 calls) — trivial to stub.

---

## 5. BIOS / Interrupt Calls

**Severity: EASY — isolated in spooler and serial.**

| What | Where | Refs |
|------|-------|------|
| `biosprint()` | `st/src/spooler.cpp` | Printer output via BIOS INT 17h |
| `int86()` / `intdos()` | `st/src/core/serial.cpp` | Serial port via BIOS INT 14h |
| `geninterrupt()` | `st/src/core/rt_isr.cpp` | ISR hooking (real hardware only) |

**Port strategy:** Replace with platform-native I/O. For a GUI port, replace `biosprint()` with file/printer API; serial is behind `ISerial` (already abstracted).

---

## 6. ISR / Timer / PIT

**Severity: EASY — already isolated in `real_dos/`.**

| Where | What |
|-------|------|
| `st/src/core/rt_isr.cpp` | IRQ0 hook (`NewISR08h`), PIT channel 0 reprogramming, keyboard/break/crit-error vectors |
| `st/src/core/engine.cpp` | `ENGINE::Install()` / `Uninstall()` — ISR lifecycle |
| `st/src/demo_dos/demo_eng.cpp` | `DEMO_ENGINE::Install()` — timer-based tick (no IRQ, no PIT) |

**Key insight:** The demo engine uses `NewTimer()` / `WaitTimer()` from Zinc's `events.h` — a platform-independent timer API. It does NOT hook IRQ0 or touch the PIT. All ISR/PIT code is in `core/` (behind `ENGINE` base) or `real_dos/` (never compiled in demo). The demo's timing is already portable.

---

## 7. Encoding (Latin-1 / CP850)

**Severity: EASY — small, well-localized bridge.**

| Where | What |
|-------|------|
| `st/src/st_util.cpp:17-111` | `_ISO2ASCII()` / `_ISO2ASCII2()` — Latin-1 → CP850 conversion (accented characters) |
| `st/src/spooler.cpp:202` | Converts output bytes before printing |
| `st/src/ctrl/ctrl_rf.cpp:592` | Converts booth city names for display |

**Total:** 3 functions, ~100 LOC.

**Port strategy:** For a UTF-8 target, replace `_ISO2ASCII` with a Latin-1 → UTF-8 encoder (trivial: bytes 0x80-0xFF become 2-byte sequences). The conversion points are already centralized in `st_util.cpp`. Source data (`.inf` files, config) is Latin-1/CP850; the app stores and displays in Latin-1.

---

## 8. Binary Data Formats

**Severity: MEDIUM — already mostly abstracted.**

| Format | Where | Status |
|--------|-------|--------|
| `RX.DAT` / `RX.IDX` (receipt store) | `st/src/db/dstorage.cpp` | ✅ Behind `DB_STORAGE_BACKEND`; `BinStorage` is one implementation |
| `RXX.*` (extension store) | `st/src/db/d_ext_st.cpp` | Uses same `BinStorage` backend |
| `RX.STA` (statistics) | `st/src/db/dstatist.cpp` | Binary struct — would need a CSV alternative for full portability |
| `ST.INI` (config) | `st/src/cfg.cpp` | ✅ Plain text INI — already portable |
| `PH_INFO.DAT` / `.inf` (tariff data) | `st/src/ph/ph_eng.cpp` | Binary compiled from `.inf` text files; `.inf` is the source of truth |
| `RES.DAT` (Zinc resource) | `st/res/res.dat` | Proprietary Zinc binary format — see §1 |
| `FILE_HEADER` | `st/src/db/filehdr.cpp` | Binary header stamped on data/index files |

**Receipt struct:** Fixed 111-byte binary record (`st/include/receipt.h`). The `#if sizeof(Receipt) != 111` guard ensures layout stability. `CsvStorage` already provides a text alternative.

---

## 9. Printer Formatter DLLs

**Severity: EASY — self-contained, rarely change.**

| DLL | LOC | Format |
|-----|-----|--------|
| `pr_dr18.c` | 498 | DR-18 receipt printer |
| `pr_dr40.c` | 436 | DR-40 receipt printer |
| `pr_dr80.c` | 428 | DR-80 receipt printer |
| `pr_dreme.c` | 441 | DR-ME receipt printer |
| `pr_drhal.c` | 431 | DR-HAL receipt printer |
| `pr_drpre.c` | 420 | DR-PRE receipt printer |
| `pr_lin80.c` | 425 | LIN-80 receipt printer |
| `pr_sr28.c` | 455 | SR-28 slip printer |
| `pr_sr40.c` | 438 | SR-40 slip printer |
| `pr_sr80.c` | 420 | SR-80 slip printer |
| `plain_pr.cpp` | 814 | Plain-text printer backend |
| `prn_fmt.cpp` | 100 | Formatter dispatch |

**Total:** 5,306 LOC (16% of codebase).

These are Borland DLLs loaded by the spooler at runtime. Each exports a `Format()` function that takes a `Receipt` and emits formatted text with ESC/P control codes. The `far` keyword appears 28 times per DLL (DLL calling convention). For a portable target, these could be replaced with a single generic formatter or a PDF emitter (see `MINI_SMARTTAR_PLAN` §8 "PDF pseudo-device").

---

## Summary: Effort by Seam

| Seam | LOC affected | Difficulty | Already portable? |
|------|-------------|------------|-------------------|
| Zinc 3.5 UI | ~8,444 | EXTREME | No |
| DOS/Borland runtime | ~32,603 (all) | EXTREME | Partially (core/ is clean) |
| Far pointers | ~299 refs | HARD | Yes for demo (1 ref) |
| Port I/O | ~61 refs | EASY | Yes (behind `I*` interfaces) |
| BIOS/interrupts | ~3 refs | EASY | Partially (spooler needs work) |
| ISR/timer/PIT | ~201 LOC | EASY | Yes (demo uses platform timers) |
| Encoding | ~100 LOC | EASY | Centralized in `st_util.cpp` |
| Binary data formats | ~1,600 LOC | MEDIUM | Mostly (CSV backend exists) |
| Printer DLLs | 5,306 LOC | EASY | Self-contained DLLs |

**Bottom line:** The `core/` subtree (~5,500 LOC: engine, config, storage, telephony, statistics) is the most portable layer — it has minimal platform tentacles and is already behind interfaces (`DB_STORAGE_BACKEND`, `ENGINE`, `IStm2Store`, `IDongle`, `IEeprom`, `ISerial`). The UI layer (~8,400 LOC) is the biggest replacement effort. The printer DLLs (5,300 LOC) are self-contained and could be ported mechanically.
