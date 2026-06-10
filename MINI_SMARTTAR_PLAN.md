# MINI SMARTTAR — Reduction & Portability Plan

> Status: **PLAN ONLY — not executed.** Created 2026-06-09.
> Author handoff: Gonzalo (GCC) + Claude.
> Scope decisions confirmed: relocate real/hw code to `real_dos/`; load `.ini`
> directly (drop `ini2cfg`); introduce `DB_STORAGE_BACKEND` base class with CSV as
> the demo default; document strip-down **and** the portability seam design.
> **No EDA** — the `eda` build variant is removed entirely.

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
    db/          DB_ENGINE facade + DB_STORAGE_BACKEND (CSV default, binary legacy)
    ...          (ph/, pr/, ct/, mb/, tb/, ctrl/ stay; trimmed of real-only paths)
  util/
    inf2dat/     KEPT (build input: PH_INFO.DAT)  — see note, may also go .inf-direct
    (everything else removed)
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
- **DB has a clean facade.** `DB_ENGINE` (`include/db_eng.h`) wraps storage +
  statistics. Callers (views, controller, ct printers) go through the facade, so
  a storage backend swap is containable *if* we keep the facade API stable.
- **Only 2 of 17 utils are build-essential**: `ini2cfg` and `inf2dat`. The other
  15 are maintenance / hardware / distribution tools.
- **`__DEMO__` gating is mostly already runtime** (`g_cfg->IsDemoMode()`); only
  four build-time gates remain in `st.cpp` (lines ~16, ~117-159, ~193-214).

What works *against* us:

- DB storage has **no abstraction below the facade** — `Receipt` (111-byte fixed
  struct, `include/receipt.h`) and seek-position indexing are assumed by
  `dstorage.cpp`, statistics rebuild, and the extension (`RXX.*`) parallel store.
- Hardware coupling is spread across `rt/` (ISR, ports), `dongle.cpp`,
  `eeprom.cpp`, `stm2.cpp`, `serial.cpp`.
- Config is loaded from a **binary `ST.CFG`** compiled by `ini2cfg` from
  `ST.INI`; the whole app reads the `CFG` struct, so going `.ini`-direct means a
  parser that populates `CFG` at startup.

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

**Kept:** `inf2dat` (produces `PH_INFO.DAT`). See 1.4 for whether it survives
long-term.

> Note: removing `setup` removes the only GUI that wired up EEPROM/DONGLE — fine,
> those paths are going away anyway.

Checkpoint: green demo build with 15 utils gone.

### 1.2 Establish the `core/` / `demo_dos/` / `real_dos/` split

Create the three subtrees and **move files** (no logic change yet):

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

> `inf2dat`: same logic *could* apply to `PH_INFO.DAT` (load `.inf` directly).
> **Deferred** — telephony `.inf` parsing is more involved (`parser.cpp` already
> exists for it) and `inf2dat` is harmless. Revisit in Phase 2 if we want zero
> build-time codegen. For now `inf2dat` stays as the one kept util.

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
- **`inf2dat` retention**: kept for now; decide in Phase 2 whether to go
  `.inf`-direct (reuses existing `parser.cpp`).
- **8.3 + encoding discipline** applies to every new file (`demo_dos`,
  `real_dos`, `core`, new headers). New `.cpp/.h` are CRLF + Latin-1/CP850 per
  the existing rules; this `.md` and host scripts stay LF/UTF-8.

---

## 9. Checkpoint summary

| # | Checkpoint | Build state | Status |
| - | ---------- | ----------- | ------ |
| P0 | Baseline tagged (`pre-mini-smarttar`) | demo green (st.exe 1090948) | DONE |
| 1.1 | 15 utils removed (kept inf2dat, ini2cfg) | demo green | DONE |
| 1.2 | core/demo_dos/real_dos split (move only) | demo green (st.exe identical) | DONE |
| 1.2fix | rt_do/isr/store/util+serial are core, not real_dos | demo green | DONE |
| 1.3a | demo drops rt_eng/dongle/eeprom (factory guard + LINK_OBJS) | st.exe 1087668 | DONE |
| 1.3b | STM2 abstracted (NullStm2 demo / BankStm2 real) | st.exe 1086724; demo links 0 real_dos | DONE (build-verified; runtime smoke-test pending) |
| 1.4 | config from ST.INI, ini2cfg gone | demo green | TODO |
| 1.5 | two variants; real_dos #errors; drop eda/prod | demo green / real_dos errors | TODO |
| 2.1a | BinStorage extracted behind backend iface | demo green (binary) |
| 2.1b | CsvStorage added, default flipped to csv | demo green (csv) |
| 2.2 | PORTABILITY.md seam catalogue | doc |
| 3 | C++-exit plan | doc (separate) |

---

*End of plan. Nothing in this document has been executed.*
