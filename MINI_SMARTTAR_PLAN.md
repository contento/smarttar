# MINI SMARTTAR — Reduction & Portability Plan

> **Status: COMPLETE — all phases 1.1–2.2 done.**
> Created 2026-06-09. Branch: `mini-smarttar`.
> Scope: relocate real/hw code to `real_dos/`; load `.ini` directly (drop
> `ini2cfg`); introduce `DB_STORAGE_BACKEND` base class with CSV as the demo
> default; document strip-down **and** the portability seam design.
> **No EDA** — the `eda` build variant is removed entirely.
>
> UI regression (toolbar/grid misplacement) fixed 2026-06-10 — root cause was
> missing `-D__BTN__` in `st.cfg`. See `HANDOFF.md` for details.

---

## 1. Goal

Reduce SmartTar to a **mini-smarttar**: the smallest, most portable subset that
still runs the application end-to-end in demo mode. This is explicitly **the base
for two future tracks**:

1. **New compilers / new OSs** — get off Borland C++ 3.1 + Pharlap 286 + DOS 5.0.
2. **Moving out of C++** — eventually re-implement in another language.

Therefore every decision here favours **isolation of platform-specific code
behind interfaces** over in-place cleverness. We are not just deleting code; we
are drawing the seams a port will cut along.

Guiding constraints (unchanged from project rules):

- Simple and compatible with a **1995-era toolchain** — no STL, no exceptions,
  no namespaces, no `bool`, `far` pointers, 8.3 filenames, Latin-1/CP850 source.
- The mini build must still **compile and run under the current DOSBox-X +
  BCC286 + Zinc 3.5 toolchain** at every checkpoint. We do not break the build to
  chase portability; we refactor in compilable steps.

---

## 2. Target end-state (what "mini" looks like)

```text
st/
  src/
    core/        portable-leaning app + engine-agnostic logic
    demo_dos/    DEMO_ENGINE + Null* hardware mocks (THE buildable engine)
    real_dos/    RT_ENGINE + all hardware drivers — DEACTIVATED (#error on build)
    ui/          Zinc UI layer (unchanged for now; a future seam, see Phase 2)
  (util/ — entirely removed; inf2dat eliminated in 1.4b)
  include/       headers, reorganized to match src/ subtrees
  MAKEFILE       two variants only: demo_dos (default), real_dos (errors)
```

Two build variants only:

| Variant     | Buildable? | Defines / behaviour |
| ----------- | ---------- | ------------------- |
| `demo_dos`  | **Yes** (default) | `-DDEMO_DOS` — synthetic engine, Null* hw mocks, no dongle/EEPROM/STM2/serial. `-DDEBUG` may be added as a *modifier* for symbols. |
| `real_dos`  | **No** — hard `#error` | `-DREAL_DOS` — compiling any `real_dos/` TU without an explicit `-DREAL_DOS_ENABLED` override emits `#error "real engine deactivated in mini-smarttar"`. |

Removed variants: `eda`, `prod` (and the standalone `dbg` variant — debug folds
into a flag on `demo_dos`).

---

## 3. What we already have working for us

The codebase is in better shape for this than expected:

- **Engine is already polymorphic.** `include/engine.h` defines an abstract
  `ENGINE` base (`InitHardware`, `RecoverState`, `OnTimerTick`, `OnTimerEnd`,
  `IsDemo`). `rt/eng_fact.cpp::MakeEngine()` picks `DEMO_ENGINE` vs `RT_ENGINE`
  at **runtime** from `g_cfg->ENGINE_KIND`. The "mock with a base class" pattern
  the goal asks for **already exists** — we extend it, not invent it.
- **Only 2 of 17 utils were build-essential**: `ini2cfg` and `inf2dat`.
  Both are now eliminated (1.4/1.4b). The entire `util/` directory is gone.
- **`__DEMO__` gating is now fully runtime** (`g_cfg->IsDemoMode()`).
  Zero `#ifdef __DEMO__` remains in `st.cpp` (collapsed in 1.3).

What *was* against us (resolved):

- ~~DB storage has **no abstraction below the facade**~~ — still true; addressed in Phase 2.
- ~~Hardware coupling spread across `rt/`, `dongle.cpp`, `eeprom.cpp`, `stm2.cpp`, `serial.cpp`~~ — **resolved.** All hardware behind `I*` base classes; only `Null*` linked in demo.
- ~~Config loaded from binary `ST.CFG` via `ini2cfg`~~ — **resolved.** `cfg.cpp` reads `ST.INI` directly; `ini2cfg` deleted.

---

## 4. Phase 0 — Baseline & safety net

Before touching anything:

1. Tag the current tree: `git tag pre-mini-smarttar`.
2. Confirm a clean `./build.sh demo` succeeds and `st.exe` runs in DOSBox-X.
   This is the reference behaviour every later checkpoint is compared against.
3. Capture a known-good demo run artifact set (a sample `RX.DAT`, a screenshot of
   the main view, a printed receipt capture) to diff against after the CSV swap.

Checkpoint: **green build + runnable demo recorded.**

---

## 5. Phase 1 — Strip down to mini

Order matters: remove dead weight first (smallest risk), then relocate, then
re-wire the build. Each step ends compilable.

### 1.1 Remove non-essential utils

Delete these `st/util/` subtrees and their `MAKEFILE`/`buildall.bat` references:

| Util | Why removed |
| ---- | ----------- |
| `chkrx`, `viewer`, `viewlog` | Diagnostic-only. (Re-derivable; CSV makes `viewer` trivial later.) |
| `defpwd` | Password reset maintenance. |
| `repair`, `sar` | Archive/STM2 maintenance (hardware-bound). |
| `rweeprom` | EEPROM hardware tool. |
| `stc` | Modem/serial remote-config tool (hardware). |
| `stl` | Data loader w/ dongle validation. |
| `stm2` (dump/fill/fdump) | STM2 memory-bank diagnostics (hardware). |
| `gen`, `update`, `rxback`, `sip`, `setup` | Distribution / install / packaging wrappers. |
| `_old/` | Already dead. |

- ~~**Kept:** `inf2dat` (produces `PH_INFO.DAT`)~~ — later removed in 1.4b;
  `PH_ENGINE::Load()` now reads `.inf` files directly.

> Note: removing `setup` removes the only GUI that wired up EEPROM/DONGLE — fine,
> those paths are going away anyway.

Checkpoint: green demo build with 15 utils gone.

### 1.2 Establish the `core/` / `demo_dos/` / `real_dos/` split

- `src/demo_dos/` ← `demo_eng.*` (the `DEMO_ENGINE`), plus new `Null*` mocks
  (see 1.3).
- `src/real_dos/` ← `rt/` (all of it: `rt_eng`, `rt_isr`, `rt_do`, `rt_store`,
  `rt_util`, `serial`), `dongle.cpp`, `eeprom.cpp`, `stm2.cpp`, and the real
  half of `eng_fact.cpp`.
- `src/core/` ← engine-agnostic application logic that neither demo nor real
  should own (the factory's selection point, `ENGINE` base usage, controller
  wiring). Most root `.cpp` (`st.cpp`, `cfg.cpp`, `cstr.cpp`, `info.cpp`,
  `log.cpp`, `calc.cpp`, `calendar.cpp`, `res.cpp`, `st_util.cpp`) stays at the
  app level for now; only clearly engine-coupled files move.

Update `MAKEFILE` `.obj` rules for every moved file (Borland MAKE 3.6 needs one
explicit `$(OBJ_DIR)\x.obj: $(SRC)\sub\x.cpp` rule per object — no path search).

Checkpoint: green demo build, files relocated, **no behaviour change**.

### 1.3 Mock the hard hardware dependencies (Null-object base classes)

For each hardware dependency, introduce an **abstract base** + a **null mock**
that the demo build links, and move the real implementation to `real_dos/`.
Pattern mirrors the existing `ENGINE`/`DEMO_ENGINE`/`RT_ENGINE` trio.

| Dependency | New base (in `core/include/`) | Demo mock (in `demo_dos/`) | Real impl (in `real_dos/`) |
| ---------- | ----------------------------- | -------------------------- | -------------------------- |
| Copy-protection dongle | `IDongle` (`IsThere()`, `Read/Write`) | `NullDongle` — always present, no I/O | `LptDongle` (was `dongle.cpp`) |
| Version EEPROM | `IEeprom` | `NullEeprom` — returns canned version | `Cs46Eeprom` (was `eeprom.cpp`) |
| STM2 persistent NVRAM | `IStm2Store` | `NullStm2` — in-RAM, no banks | `BankStm2` (was `stm2.cpp`) |
| Booth/PBX port I/O + ISR | already `ENGINE` base | `DEMO_ENGINE` (exists) | `RT_ENGINE` (was `rt/`) |
| Serial port | `ISerial` | `NullSerial` / loopback | `BiosSerial` (was `serial.cpp`) |

Each base class header lives in `core/`; each `Null*` is the **only** hardware
class the `demo_dos` build compiles. This is the literal realization of "create a
mock with a base class to show that it is being stripped out" — the base class is
the documented seam; the `Null*` says *"this hardware is intentionally absent."*

Then collapse the four remaining `st.cpp` `__DEMO__` gates: replace
`#if defined(__DEMO__)` branches with construction of the appropriate interface
(`NullDongle` vs `LptDongle`) chosen the same way `MakeEngine()` chooses the
engine. After this, `st.cpp` has **zero** `#ifdef __DEMO__`.

Checkpoint: green demo build, `st.cpp` ifdef-free, all hw behind interfaces.

### 1.4 Config: load `ST.INI` directly, drop `ini2cfg`

- Add an `.ini` reader in `cfg.cpp` (or a small `IniReader` helper) that
  populates the existing `CFG` struct field-by-field at startup. Keep the `CFG`
  struct shape unchanged so the rest of the app is untouched.
- Default values come from the existing `CFG::SetDefault`; the `.ini` overrides.
  `ENGINE_KIND` defaults to `"demo"` (no `__DEMO__` needed once gates are gone).
- Delete `util/ini2cfg/`, the binary `ST.CFG` distribution in `MAKEFILE`
  (`CFG_DESTS`), and any `bin/st.cfg` copy steps. `ST.INI` becomes the single,
  human-editable, **portable** config source.
- Parser must be 1995-safe: plain `fgets` + manual key=value split, Latin-1
  tolerant, no regex/STL.

> `inf2dat`: **Done in 1.4b** — `PH_ENGINE::Load()` now reads `.inf` files
> directly when `PH_INFO.DAT` is absent. `inf2dat` and the entire `util/`
> directory were eliminated. No build-time codegen remains.

Checkpoint: green demo build reading `ST.INI` directly; `ini2cfg` gone.

### 1.5 Build system: two variants

- Replace `makedemo/makedbg/makeeda/makeprod.bat` with **`makedemo_dos.bat`**
  (`-DDEMO_DOS -DRUN`) and **`makereal_dos.bat`** (`-DREAL_DOS -DRUN`).
- `real_dos/` translation units begin with:
  ```c
  #if !defined(REAL_DOS_ENABLED)
  #error "real engine deactivated in mini-smarttar -- build demo_dos. \
          To work on real hardware code, define REAL_DOS_ENABLED explicitly."
  #endif
  ```
  So `makereal_dos.bat` fails loudly by design; a developer must opt in.
- Update `build.sh` / `build.ps1`: variant set becomes `demo_dos|real_dos`,
  default `demo_dos`; `--debug` flag adds `-DDEBUG`. Remove `eda`/`prod`/`dbg`.
- Strip `MAKEFILE` of `eda`/`prod` defines and the removed-util rules.

Checkpoint: `./build.sh` (== `demo_dos`) green; `./build.sh real_dos` fails with
the intended `#error`.

### 1.6 Phase 1 exit criteria

- `demo_dos` builds and runs identically to the Phase 0 baseline.
- `real_dos` refuses to build with a clear message.
- 15 utils gone; `ini2cfg` gone; config from `.ini`.
- All hardware behind `I*` base classes; only `Null*` linked in demo.
- `st.cpp` free of `__DEMO__`/`__NO_DONGLE__`.

---

## 6. Phase 2 — Portability seams

Phase 1 produces a smaller, demo-only DOS app. Phase 2 makes it **portable** by
isolating the three things a new compiler / new OS cannot keep:
**DOS, Pharlap, and Zinc.** No port happens here — we install the seams.

### 2.1 Receipts → CSV behind `DB_STORAGE_BACKEND`

The single most portability-blocking artifact is the **binary, seek-indexed
`RX.DAT`**. Make CSV the demo default behind an abstraction:

- New abstract `DB_STORAGE_BACKEND` (in `core/include/`) exposing the operations
  `DB_ENGINE` actually needs: `Add`, `Update`, `Get`, iterate, count, repair.
  Derive the API from current `DB_ENGINE` usage so the facade is unchanged.
- `CsvStorage` (default for `demo_dos`): one CSV row per `Receipt`; an in-memory
  index built on load (record count is small for demo). Human-readable, trivially
  portable, no seek math. Statistics recomputed from rows on load/repair.
- `BinStorage` (legacy): the existing `dstorage.cpp` behaviour, kept behind the
  same interface for anyone needing to read old `RX.DAT` archives.
- Backend chosen by config (`ST.INI`, e.g. `STORAGE=csv|bin`), default `csv`.
- The parallel extension store (`RXX.*`) and `RX.STA` follow the same backend
  selection.

Risk: ~1000–1400 LOC, statistics recompute, archive back-compat. Mitigation:
land `BinStorage` (pure extraction, behaviour-identical) **first** as its own
green checkpoint, then add `CsvStorage`, then flip the default. The Phase 0
golden artifacts validate round-trip equivalence.

### 2.2 Catalogue the platform seams (design, not implementation)

Document (and, where cheap, introduce the interface for) the remaining
non-portable surfaces so a future port has a checklist:

| Seam | Where today | Portability note |
| ---- | ----------- | ---------------- |
| **Pharlap / 286 protected mode** | `st.def`, `BIND286`/`CFIG286`, `PHAPI`, `far` pointers everywhere | The deepest seam. A modern target is flat 32/64-bit; `far`/segment assumptions must be catalogued. |
| **Zinc 3.5 UI** | `src/ui/`, `mb/`, `tb/`, `res.dat` | UI is the largest single port cost. Define a thin `IView`/`IWidget`-style boundary *only as documentation* now; do not rewrite Zinc. |
| **DOS file/console/printer I/O** | `spooler.cpp`, `pr/*.c` DLLs, BIOS/COM calls, `_ISO2ASCII` | Wrap behind `core` I/O helpers so encoding + device assumptions are in one place. |
| **Encoding (Latin-1 / CP850)** | dual-encoding bridge in `st_util.cpp` | A portable target should standardize on one encoding (likely UTF-8) at the boundary; note the conversion points. |
| **Timing / ISR / PIT** | `rt_isr.cpp`, `SetPITRate` | Already isolated in `real_dos/`; demo timing must not depend on it. |

Deliverable of 2.2 is a **`PORTABILITY.md`** seam catalogue, not code.

### 2.3 Phase 2 exit criteria

- Demo runs on CSV storage; binary archives still readable via `BinStorage`.
- Every platform seam is named, located, and rated in `PORTABILITY.md`.
- The dependency on DOS/Pharlap/Zinc is concentrated, not diffuse.

---

## 7. Phase 3 — Move off C++ (sketch only)

Out of scope to execute; recorded so Phase 1–2 decisions stay aligned with it.
The strip-down + seams above are precisely what make this feasible:

- The `core/` subtree (engine-agnostic, hardware behind `I*`, config from `.ini`,
  receipts as CSV) is the **first candidate to re-implement** in a new language —
  it has the fewest platform tentacles.
- `demo_dos`/`real_dos`/`Null*` boundaries map cleanly onto a future
  trait/interface system in the target language.
- CSV + `.ini` mean **data formats are already language-neutral** — a rewrite
  shares files with the C++ build during transition.
- Language choice, FFI-vs-rewrite strategy, and UI-replacement are deferred to a
  dedicated Phase 3 doc once Phase 2 lands.

---

## 8. Risks & open items

- **CSV round-trip fidelity** for statistics and the extension store is the main
  technical risk; gated behind the BinStorage-first checkpoint.
- **Zinc resource (`res.dat`)** still binds the UI to DOS Zinc; untouched here.
  It is the biggest *unaddressed* portability cost — flagged for Phase 2 catalogue
  only.
- **`inf2dat` eliminated**: `PH_ENGINE::Load()` now falls back to reading
  `.inf` files directly when `PH_INFO.DAT` is absent; tariffs/schedules
  use constructor defaults.  The entire `util/` directory is removed.
- **8.3 + encoding discipline** applies to every new file (`demo_dos`,
  `real_dos`, `core`, new headers). New `.cpp/.h` are CRLF + Latin-1/CP850 per
  the existing rules; this `.md` and host scripts stay LF/UTF-8.
- **PDF pseudo-device** (research):
  SmartTar's printing pipeline is: `PrnFormatter` DLL (e.g. `PR_DR80.DLL`)
  formats receipt text → `SPOOLER` buffers it → `SPOOLER::Flush()` sends it
  to BIOS LPT or serial.  The spooler writes plain text (ESC/P-like control
  codes, line feeds, tabs) to a channel.  A PDF pseudo-device would:
  (1) intercept the spooler channel, (2) capture the formatted text per
  receipt, (3) render to PDF using a lightweight library.  For DOS/16-bit:
  a minimal PDF writer (~200 LOC) can emit PDF directly from text lines
  (PDF spec §8 — text objects, no images needed for receipts).  For the
  portable target: any language can emit PDF.  This makes demo mode fully
  self-contained (no physical printer required).  Implementation: add
  `SP_CHANNEL_PDF` to the spooler, configurable via `ST.INI` (e.g.
  `P_PORT=pdf`).  The existing `P_PORT` field already dispatches to
  different backends (`lpt`, `com`, `prn`); `pdf` would be a new one.
  **Candidate for Phase 2** (portable, language-neutral, complements CSV
  storage).

- **RES.DAT decompiler** (research):
  `res.dat` uses Zinc 3.5's proprietary `UI_STORAGE` format — a
  filesystem-within-a-file with named directories and serialized objects.
  Each object is tagged by `OBJECTID` (defined in `UI_GEN.HPP`:
  `ID_WINDOW=1007`, `ID_STRING=13`, `ID_BUTTON=2`, etc.) followed by
  property data and child objects.

  **What we have:**
  - Full Zinc 3.5 source in `vendor/zinc/SOURCE/` including `STORAGE/`
    (file I/O: `G_STORE.CPP`, `Z_STORE.CPP`) and `DESIGN/` (the visual
    editor: `Z_CONFIG.CPP`, `Z_OBJECT.CPP`, `Z_RESRC.CPP`).
  - `BROWSE.EXE` (source: `vendor/zinc/SOURCE/STORAGE/BROWSE.CPP`) lists
    directory structure and metadata of a `.dat` file.
  - `ZDUMP.EXE` (source: `vendor/zinc/SOURCE/STORAGE/ZDUMP.CPP`) dumps raw
    hex of any named object — useful for reverse-engineering the serialization.
  - `DESIGN.EXE` and `WDESIGN.EXE` in `vendor/zinc/BIN/` for interactive
    editing (Windows and DOS versions).

  **Approach for a decompiler:**
  1. Study `G_STORE.CPP` / `Z_STORE.CPP` to understand the binary layout
    (header, directory entries, object serialization).
  2. Study `Z_OBJECT.CPP` in DESIGN to see how objects are loaded/saved
    (type tag → property list → children).
  3. Write a tool that walks the storage tree, reads each object's type tag
    and serialized properties, and emits a human-readable representation
    (JSON, YAML, or a Zinc-compatible `.znc` text format — `P_DESIGN.ZNC`
    in `vendor/zinc/BIN/` may already be such a format).
  4. The reverse path (text → `res.dat`) would re-implement the DESIGN
    serialization, making the resource file fully portable.

  **Why this matters:** `res.dat` is the *last* hard dependency on the Zinc
  toolchain.  Once decompilable, UI resources become editable by any text
  editor or code generator, enabling the Phase 3 language-exit strategy.
  **Candidate for Phase 2** portability seam work (highest ROI among the
  unaddressed seams).

---

## 9. Checkpoint summary

| # | Checkpoint | Build state | Status |
| - | ---------- | ----------- | ------ |
| P0 | Baseline tagged (`pre-mini-smarttar`) | demo green (st.exe 1090948) | DONE |
| 1.1 | 15 utils removed (kept inf2dat, ini2cfg) | demo green | DONE |
| 1.1+ | inf2dat + ini2cfg also removed (1.4/1.4b) | demo green | DONE |
| 1.2 | core/demo_dos/real_dos split (move only) | demo green (st.exe identical) | DONE |
| 1.2fix | rt_do/isr/store/util+serial are core, not real_dos | demo green | DONE |
| 1.3a | demo drops rt_eng/dongle/eeprom (factory guard + LINK_OBJS) | st.exe 1087668 | DONE |
| 1.3b | STM2 abstracted (NullStm2 demo / BankStm2 real) | st.exe 1086724; demo links 0 real_dos | DONE (build-verified; runtime smoke-test pending) |
| 1.4 | config from ST.INI, ini2cfg gone | demo green | DONE |
| 1.4b | inf2dat gone, PH_ENGINE loads .inf direct | demo green | DONE |
| fix | UI regression — missing `-D__BTN__` in st.cfg | demo green, GUI correct | DONE (`29e59ab`) |
| 1.5 | two variants; real_dos #errors; drop eda/prod | demo green / real_dos errors | DONE |
| 2.1a | BinStorage extracted behind backend iface | demo green (binary) | DONE |
| 2.1b | CsvStorage added, default flipped to csv | demo green (csv) | DONE |
| 2.2 | PORTABILITY.md seam catalogue | doc | DONE |
| 3 | C++-exit plan | doc (separate) | TODO |

---

- *Last updated: 2026-06-10. All phases 1.1–2.2 complete. See PORTABILITY.md for seam catalogue.*
