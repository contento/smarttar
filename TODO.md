# SmartTar TODO

Working list of milestones and tasks. Detailed findings live in
[STABILITY_AUDIT.md](wiki/dev/STABILITY_AUDIT.md); this file is the day-to-day
"what's next" view.

## Conventions

- `[ ]` open
- `[~]` in progress
- `[x]` done
- A milestone is a coherent body of work; tasks are concrete steps inside it.
- When a task is done, leave it checked under the milestone for a while
  (history is useful), then prune when the milestone closes.

---

## Milestone: Build & run pipeline (done — `v2.34.1-runnable-by-claude`)

- [x] Resurrect the build under Borland MAKE 3.6 (explicit per-`.obj`
      rules; `.PATH.x` doesn't exist in MAKE 3.6, only 4.0+)
- [x] Wire `help.dat` regen (zinc/BIN on PATH, `HELP=1` flag in
      [build.sh](build.sh))
- [x] Source `RES.DAT` from `st/res/`, copy to `st/bin/` via MAKEFILE rule
- [x] TLINK lib-path + `c0pl.obj` full path
- [x] `st.def` CRLF (TLINK choked on bare LF)
- [x] `.gitattributes` to prevent future EOL bugs
- [x] DOSBox-X tuning: `cycles=max`, `mouse_emulation=integration`,
      `COPYCMD=/Y`, REM `->` quoting (phantom-file bug)
- [x] Headless build runners: bash + PowerShell Core
- [x] Skip-worktree on per-user runtime state files
      (`st/bin/st.cfg`, `st/bin/st.ini`, `util/NC/NC.INI`)

## Milestone: Data file hygiene and build consolidation (done)

- [x] Untrack `st/bin/` entirely -- all contents are build outputs; gitignore
      replaces the old skip-worktree approach on `st.cfg` / `st.ini`
- [x] Wire `util/inf2dat` into MAKEFILE: canonical `.inf` sources compile
      `PH_INFO.DAT` via `mk_ph.bat` (ST_TEST=1 bypasses password prompt);
      result copied to `bin/`
- [x] Wire `util/ini2cfg` into MAKEFILE: `st.ini` is the single source of
      truth; `ini2cfg` generates `st.cfg`, MAKEFILE distributes it to `bin/`
      and every `util/` subdir via `CFG_DESTS`; `st.cfg` is gitignored
- [x] Remove `st/data/` (files moved to `util/ini2cfg/`)
- [x] `ini2cfg.cpp`: skip st.cfg seed load when ST_TEST=1 and file is
      missing -- no chicken-and-egg on fresh clone
- [x] Convert all `.inf` files CP850 -> ISO-8859-1; fix pre-existing
      data-entry byte errors in `util/inf2dat/local.inf` (Buritica,
      Canasgor, Briceno, Narino, Medellin)
- [x] Consolidate `.inf` to `util/inf2dat/` only (removed duplicates from
      `test/ph_eng/`, `util/setup/`); gitignored in those locations

## Milestone: Fix Zinc 3.5 UI bugs

- [x] **Zinc Grid geometry (row tear + thick separators + bottom)**
      (branch `fix/zinc-grid-geometry`) — *Cause:* stock Zinc 3.5
      `RegionConvert` (`Z_WIN2.CPP:390-395`) floors each cell's pixel
      top/left *independently* of its size, so minicell layouts drift.
      With the runtime ratios (Y: `8*24/10 = 19.2 px/row`; X: `0.7 px/
      minicell`) the fraction accumulates to a whole pixel periodically:
      rows jumped 19->20 px every 5th booth (tear at 5/6, 10/11, 15/16),
      and the column 1-minicell overlaps opened 1 px gaps at Tel|Loc and
      Tar|Val (double = thick separators). Zinc fixed this in v4; we stay
      on 3.5. *Fix (app-side, pixel-space, no Zinc rebuild):*
      `UIW_VIEW::NormalizeRowPitch()` snaps rows to the uniform integer
      pitch from the driftless first gap and pulls the table's bottom
      border to the last row; `NormalizeColumnBorders()` closes the column
      gaps so every separator is one shared line. Both run from
      `UIW_VIEW::Event` on `S_CREATE`/`S_SIZE` -- *before* the first paint,
      so the natural redraw is correct (an earlier post-paint repaint left
      white notches at the Est|Are boundary). *Verified:* geometry dump
      shows uniform 19 px pitch, all column overlaps 0, table bottom flush;
      confirmed visually in DOSBox-X (`wiki/assets/zinc-gui-bug-03.png`).

## Milestone: UI improvements (new edition) — v4.0

- [x] **Higher-resolution display via Zinc** — Investigated (Jun 2026):
      `IdentifySuperVGA()` + `SetSuperVGAMode()` can switch to VESA
      800×600×256 through Zinc's GFX library, but not a quick win:
      bitmapped fonts don't scale (text gets smaller); RES.DAT layouts
      are position-specific and need Zinc Designer rework. Full findings:
      [wiki/dev/svga-investigation.md](wiki/dev/svga-investigation.md).
      **Verdict:** doable as v4.0 feature, needs font + layout work.
- ~~**Theme switching** — Zinc 3.5 has a built-in palette / "scheme"
  mechanism. Expose it as a runtime toggle (config option +
  menu-bar entry) so the operator can switch between palettes
  without recompiling.~~ **Abandoned 2026-05-25 — dead end.**
  Investigated on the `zinc-theme-switching` branch (multi-style
  binaries via `-DSTYLE=...` build flag + `build` `-Style`
  wiring); dropped as a product decision. Do not revive without
  reopening the topic explicitly.

## Milestone: `DEMO_ENGINE` — pluggable engine for demo mode [ALL PHASES DONE, merged to main]

New capability. **Not related to the abandoned SIMULA work** (`UIW_SIMULA`
F2 window in `mb_simul.cpp`, `RT_ENGINE::SIMULA` state, `Simula[]` /
`SimulaPhones[]` in the booth cluster — those exist to manually verify
a real hardware install, an orthogonal feature that stays as-is, with
no further work planned).

Goal: a single binary that can run either against real telephony
hardware (production, default) or against a fake event generator (dev,
demos, training). Replaces the scattered `#ifdef __DEMO__` gates in
`rt_eng.cpp` / `rt_isr.cpp` / `ctrl_ev.cpp` with a clean engine swap
at construction time.

**Architecture — pattern choice.** Strategy is the primary fit:
`ENGINE` is a pure-virtual interface capturing what the controller /
view layer need from the engine; `RT_ENGINE` and `DEMO_ENGINE` are
interchangeable concrete implementations; a small factory picks the
concrete from config at startup.

- **Strategy** over **GoF Adapter** — Adapter wraps an existing
  incompatible interface; `DEMO_ENGINE` isn't wrapping anything, it's
  a parallel implementation. The "adapter" framing in conversation
  maps to Strategy in GoF terms.
- **Strategy** over **Template Method** — pick one, not both.
  Template Method puts variation in subclasses; Strategy puts it in a
  composed object. If `RT_ENGINE` and `DEMO_ENGINE` end up sharing
  substantial init / dispatch scaffolding, fall back to Template
  Method with an `ENGINE_BASE` and `virtual ReadNextEvent() = 0` —
  but only if the duplication is real, not anticipated.
- **Not State** (engine doesn't switch concretes at runtime).
- **Not Observer at this layer** — the existing event bus already
  handles fan-out downstream of the engine.
- **Factory Method** for construction from `[ENGINE] kind = real |
  demo` in `st.ini`, replacing the build-time `__DEMO__` split with a
  runtime switch.

**Concurrency contract — the hard part.** `RT_ENGINE` doesn't expose a
`ReadNextEvent()` that callers pull from. It hooks **IRQ0 / INT 08h**
(`NewISR08h` in `rt_isr.cpp`, with PIT channel 0 reprogrammed at lines
~203-207 for a higher tick rate) and on every tick does port I/O
(`inportb`/`outportb` against `APP_PORT_BASE + PO_*`) to update
`BoothCluster::DataPort` in shared memory. The rest of the system
reads those structs and is oblivious to where the values came from.
That's the seam `DEMO_ENGINE` has to honor — it must *push* state
changes into the same `DataPort` fields, from inside an ISR, on the
same clock, so no downstream consumer can tell it's synthetic.

Implications:

- `ENGINE` interface owns the ISR lifecycle: `Install()` hooks IRQ0
  (and INT 09h / 23h / 1Bh / 24h as RT does today) via PHAPI's
  protected-mode vector calls; `Uninstall()` restores. The ISR
  function itself is private to the concrete.
- `DEMO_ENGINE::DemoISR08h` runs the Poisson generator and writes
  `DataPort.OOD`, `.Answer`, `.ThreadC`, `.DTMFFlags`,
  `.U_DTMFDigits[]` directly — same fields, same semantics. No port
  I/O.
- **ISR constraints apply to demo too**: no `malloc` / `new` in the
  ISR, no C runtime calls that aren't documented reentrant, no
  floating-point unless we explicitly save FPU state. Poisson state
  (RNG seed, next-arrival deadlines per booth, current call state)
  must be pre-allocated at `Install()` time. Borland C++ 3.1's `rand()`
  is *not* safe from an ISR — ship our own LCG.
- The DEMO ISR must finish well inside one tick. The Poisson decision
  per booth is O(booths); fine for any realistic count.
- The user's "INT 21h to read the signals" framing was loose — the
  actual real-world mechanism is the IRQ0 timer hook described above
  (INT 21h is the DOS API dispatcher and isn't involved here). The
  conceptual point stands: real engine reads pushed asynchronously by
  hardware; demo engine has to produce the same async push so the
  rest of the system can't distinguish them.

**Phase 1 — Skeleton (no behavior change).**

- [x] Extract an `ENGINE` interface (pure-virtual base) from
      `RT_ENGINE`'s public surface. Must include the ISR lifecycle
      (`Install()` / `Uninstall()` for IRQ0 + the keyboard / break /
      crit-error vectors) so concretes own their own vector
      management — not just data accessors.
- [x] `RT_ENGINE` becomes a concrete implementing the interface; the
      existing `NewISR08h` / `NewISR09h` / `NewISR23h` / `NewISR1Bh` /
      `NewISR24h` move behind `Install()`. No behavior change in
      production builds.
- [x] Empty `DEMO_ENGINE` concrete that compiles and links. Its
      `Install()` hooks IRQ0 with a stub `DemoISR08h` that does
      nothing (booths stay idle). Proves the swap end-to-end before
      Phase 2 adds real generation.
- [x] Factory wired to `[ENGINE] kind = real | demo` in `st.ini`
      (default `real`). Add the key via `ini2cfg`.
- [x] Strip `#ifdef __DEMO__` from the RT layer; the gate becomes
      "factory instantiates `DEMO_ENGINE`" instead.  Done in two
      passes: compound `!defined(__TEST__) && !defined(__DEMO__)`
      gates (28 sites, 7 files) then standalone gates (17 sites in
      control.cpp / st.cpp / tb_sp.cpp).  `CFG::IsDemoMode()`
      helper added for runtime checks.  Reduced from 58 references
      to 5 functional gates total -- the 4 in st.cpp dongle/STM2/
      EEPROM tangle (deferred, mix `__NO_DONGLE__` with `DONGLE`
      class scope) and `cfg.cpp:919` (intentional default-selector
      for `ENGINE_KIND`).

**Phase 2 — Poisson generator + rules.**

`DEMO_ENGINE` generates call arrivals using a Poisson process per call
type. Rules live in **INI format** — chosen over JSON/YAML because the
project already has a full INI toolchain (`util/ini2cfg/`, `cfg.cpp`
parser, `st.ini` distribution via `CFG_DESTS`), and Borland C++ 3.1 has
no JSON/YAML parser to reach for. New file
`util/ini2cfg/demo_engine.ini` (canonical source, compiled / copied to
`bin/` by MAKEFILE), pointed at via `[ENGINE] rules_file =
demo_engine.ini` in `st.ini` so multiple scenario files can ship.

Suggested schema (refine in Phase 2 design):

```ini
[GLOBAL]
booths            = 8        ; simulated booth count
seed              = 0        ; RNG seed (0 = time-based)
calls_per_minute  = 10       ; aggregate arrival rate across all booths/types

[LOCAL]
weight            = 70       ; relative share of arrivals (Poisson lambda
                             ; derives from weight * total_rate)
min_duration_secs = 30
max_duration_secs = 600

[NAL]                        ; national
weight            = 25
min_duration_secs = 60
max_duration_secs = 1200

[INTER]                      ; international
weight            = 5
min_duration_secs = 120
max_duration_secs = 1800
```

- [x] Implement Poisson arrival generator (one stream, classified into
      `LOCAL` / `NAL` / `INTER` by the per-type weights). All
      generator state pre-allocated at `Install()` time so the ISR
      never touches the heap.
- [x] Ship a private ISR-safe RNG (LCG, fixed seed from config) —
      `rand()` is not reentrant under Borland C++ 3.1.
- [x] Duration sampling: uniform in `[min, max]` for v1 (exponential /
      normal can come later if needed).
- [x] Destination numbers: draw from the existing `.inf` place tables
      (`util/inf2dat/local.inf`, `ddn.inf`, `ddi.inf`) so tariff
      calculation exercises real data.
- [x] Parse `demo_engine.ini` in `DEMO_ENGINE` ctor using the same
      patterns as `cfg.cpp`; fail loud if missing or malformed when
      `kind = demo`.
- [x] `DemoISR08h` writes `DataPort.OOD` / `.Answer` / `.ThreadC` /
      `.DTMFFlags` / `.U_DTMFDigits[]` (same fields RT's ISR writes)
      to advance each booth through ONHOOK → RINGUP → OFFHOOK →
      ANSWER → TALK → ONHOOK on its scheduled timeline.

**Phase 3 — Polish (optional, not blocking Phase 1/2).**

- Time-of-day variation (peak vs off-peak arrival rates).
- Scripted scenario replay (a recorded `.scn` sequence for repeatable
  regression tests).
- [x] **Lightweight operator controls -- start/stop generator** -- new
  "Detener/Reanudar simulacion" entry under the Configuracion menu
  (`view.cpp` line ~155).  Toggles a `_paused` flag on `DEMO_ENGINE`;
  `OnTimerTick` returns early when set so booths freeze in place.
  Wired via `virtual ENGINE::TogglePaused()` / `IsPaused()` and a
  `CONTROLLER::RTEngineToggleDemoPause()` shim mirroring the
  `IsDemo()` pattern.  Reload-rules portion deferred (no rules
  changing at runtime yet).  UIW_SIMULA stays off-limits.
- [x] **Quit confirmation when demo is running.** `st.cpp::Exit()`
  now has a third branch: when `CONTROLLER::RTEngineIsDemo()` returns
  TRUE, the busy-blocked "verifique las cabinas" dialog is skipped
  and a "Detener la simulacion y salir ?" prompt with Si/No fires
  instead.  Si sends `L_EXIT`; the demo ISR detaches via the
  existing `ENGINE` dtor chain (no extra cleanup needed).  Added
  `virtual BOOL ENGINE::IsDemo() { return FALSE; }` overridden to
  TRUE in `DEMO_ENGINE`, plus `CONTROLLER::RTEngineIsDemo()` shim.
- [x] **Graceful stop (drain, not freeze).** Draining state (`_draining`,
  `DrainTick`) hangs up active calls instead of freezing them mid-FSM;
  pre-answer attempts abort with no record; once all booths idle it
  latches `_paused`.
- [x] **Graceful drain on exit.** `ENGINE::ForceStoreActiveCalls()` enqueues
  a receipt for every booth still in TALK before the existing flush loop,
  so connected calls bill instead of dropping.
- [x] **Total simulation time limit (default 60 min).** `total_minutes` in
  `demo.ini [GLOBAL]` (0 = unlimited). Engine counts run-ticks and triggers
  the same graceful drain as a manual stop; resume resets the budget.

## Milestone: Documentation -- Obsidian wiki (EN + ES)

**Status (2026-06-09): wiki complete.** All 119 pages are in place,
organized as `users-guide/` + `reference-manual/` + `help/` (per language),
not the Build&Run/Architecture/Operations sketch below. The READMEs have been
shrunk to intro + build + wiki pointer. Remaining:
`.gitattributes` confirmation and `help.txt` cross-linking.

Bring the scattered docs ([README.md](README.md), [README.es.md](README.es.md),
the four `.docx` manuals in [st/docs/archive/](st/docs/archive/), the in-app `help.txt` in [st/res/](st/res/),
[STABILITY_AUDIT.md](wiki/dev/STABILITY_AUDIT.md), [HANDOFF.md](HANDOFF.md),
[RELEASING.md](RELEASING.md), [CLAUDE.md](CLAUDE.md),
[dosbox-x-smarttar-setup.md](dosbox-x-smarttar-setup.md),
[ISR_VOLATILE_NOTES.md](ISR_VOLATILE_NOTES.md)) into a single
browsable wiki built on Obsidian.

The **shipped layout** (actual, checked in):
```text
wiki/
  en/                         English vault
    index.md                  Landing page
    users-guide/              14 chapter files
    reference-manual/         14 chapter files
    help/                     Reserved for English help (app ships in Spanish)
  es/                         Spanish vault
    index.md
    manual-usuario/           14 chapter files
    manual-referencia/        14 chapter files
    ayuda/                    48 help topic files (compiled into help.dat)
    stc/                      SmartTar Communicator doc
  dev/                        Developer docs
  assets/                     Screenshots
  tools/                      Conversion scripts
  manifest.json               Build pipeline manifest
  build-docs.sh / .ps1        Generator scripts
  CONVENTIONS.md              Authoring rules
```

**README simplification (done 2026-06-09).** README.md / README.es.md now
fit in ~30 seconds: title, tagline, screenshot, 5-line "what it is", one
paragraph "how to build", variant table, and a prominent pointer to the wiki.
Everything in the "Configuration" / "Architecture" / "Runtime Data Files"
sections moved into the reference manual in the wiki the content was already
there — the README just stopped duplicating it.

- [x] Decide vault layout — `wiki/en/` + `wiki/es/` parallel vaults,
      users-guide/reference-manual/help structure (shipped, diverges from
      the Build&Run/Architecture/Operations sketch). No `.obsidian/` config
      committed — Obsidian auto-creates it on first open.
- [x] Convert the four `.docx` manuals to clean Markdown — done via pandoc +
      `split_manual.py`. 14 chapters per manual × 2 languages × 2 manuals.
      Images extracted to `wiki/en/` and `wiki/es/` asset dirs.
- [x] Split converted manuals into Obsidian-sized pages — one file per
      chapter, with wikilinks in each `index.md`.
- [x] Mirror the EN vault into the ES vault — translations sourced from the
      Spanish `.docx` manuals; structure is parallel.
- [x] `.gitattributes`: confirm `wiki/**/*.md` follows host-side LF policy
      — already covered by the existing `*.md text eol=lf` rule. No
      `.obsidian/` or `.canvas` files need special handling; `.obsidian/`
      added to `.gitignore` so auto-created user config stays out of git.
- [x] Cross-link wiki pages to `help.txt` sections — 24 reference manual
      pages (12 EN + 12 ES) now carry a "Related help topics" / "Temas de
      ayuda relacionados" footer with Obsidian wikilinks to the matching
      `es/ayuda/H_*` topic pages. Chapters 1 and 3 (overview, architecture)
      have no direct help topics and are intentionally left without footers.

Out-of-scope follow-ups: hosted docs site, search index, automated
docx -> md regeneration on push (manuals change rarely).

## Milestone: Stability under Extended DOS

Findings from [STABILITY_AUDIT.md](wiki/dev/STABILITY_AUDIT.md) — see that file for
full context and severity rationale.

**Spot-verified CRITICALs (audit § 6) -- all four DONE, committed:**

- [x] **C1** `src/ctrl/ctrl_ev.cpp:488` -- UE_RNPP `while (it)` never
      advances iterator; hangs the UI when "Pagar recibos por cabina"
      is selected.  Single-line fix (add `it++;` mirroring UE_RPP at
      line 469).
- [x] **C2** `src/rt/serial.cpp:191` -- Ring-buffer overflow check
      has empty body; writes at line 193 unconditional, corrupts
      queue.  Also depends on `BufLen` being power-of-two via
      precedence accident (`BufLen-1` binds tighter than `&`).
- [x] **C3** `src/dongle.cpp:34` -- `!biosprint(...) & BIOS_PRINT_BUSY`
      has `!` binding before `&`; the dongle-busy short-circuit is
      always 0 (dead code).  Add parens around `biosprint(...) &
      BIOS_PRINT_BUSY` and re-invert.
- [x] **C4** `src/ct/ct_util.cpp:30` -- `"\x1B\x70\x00\x0A\0x0A\xFF"`
      -- the `\0x0A` is `\0` then literal `x0A`.  Cash-drawer command
      sequence is malformed (9 bytes, not 6).

**Milestone CLOSED -- substantially complete (2026-06-07, audit § 8).**

- [x] Address remaining CRITICAL findings (audit § 3 C5-C22) -- **ALL DONE.**
      C5–C9 (`58e68d5`), C11–C22 (`f497445`), C19 (`d4bbd49`),
      C14/C15 (`d7d24ca`). C10 verified-defended (no code change).
- [x] Address HIGH findings (audit § 3) -- mechanical batch (`aeb1372`),
      DB/telephony (`ad15670`, `082c188`), UI/controller (`fd0e188`), and
      the final Bucket-B batch (`038f489`: spooler `strlen`, st.cpp OOM
      guard, mutex invariant doc). Several closed DEFENDED/WONTFIX with
      rationale recorded inline in the audit.
- [x] Address MEDIUM / LOW findings (audit § 3) -- clear wins done
      (`038f489`: eeprom off-by-one, w_table use-after-free); bdisplay
      STR16 closed DEFENDED (value range).
- **Tier 3 deferred (ISR / real-time concurrency)** — parked in
  [ISR_VOLATILE_NOTES.md](wiki/dev/ISR_VOLATILE_NOTES.md). Technical note
  only; no commitment to perform this audit.

## Milestone: Repo layout -- vendor consolidation [DONE]

Moved the four bundled third-party / external toolchain trees -- `bc/`,
`pharlap/`, `zinc/`, and `util/` -- into a single top-level
`vendor/` directory so the project root holds only first-party
artifacts (`st/`, `build.sh` / `run.sh` / `build.ps1` / `run.ps1`, `dosbox-x.conf`,
docs).

**Surface area updated** (every relative path that walks through one
of these four trees was retargeted by *inserting* a `vendor\` segment --
the `..\` depth is unchanged, since the project root is unchanged):

- [x] Move trees: `bc/` -> `vendor/bc/`, `pharlap/` -> `vendor/pharlap/`,
      `zinc/` -> `vendor/zinc/`, `util/` -> `vendor/util/`.
      (`st/util/` stays put -- first-party build utilities, not vendored.)
- [x] `dosbox-x.conf` PATH + comments: `C:\BC\BIN;...;C:\UTIL\...` ->
      `C:\VENDOR\BC\BIN;...;C:\VENDOR\UTIL\...`.
- [x] `st/MAKEFILE`: `..\bc\lib`, `..\pharlap\lib`, `..\zinc\lib`,
      `..\pharlap\lib\c0pl.obj`, `RTK_DIR=..\pharlap\bin` -> `..\vendor\...`.
- [x] `st/st.cfg` (BCC config): `-I`/`-L` `..\{bc,pharlap,zinc}\...`
      -> `..\vendor\{bc,pharlap,zinc}\...`.
- [x] `st/util/util.cfg` + every `st/util/*/makefile` that referenced a
      tree (`inf2dat`, `ini2cfg`, `setup`, `viewer`, `chkrx`):
      `..\..\..\{bc,pharlap,zinc}\...` -> `..\..\..\vendor\{...}\...`,
      `c:\pharlap\bin` -> `c:\vendor\pharlap\bin`.
- [x] `st/testbeds/*` build configs/makefiles (`test.cfg`, `pr_fmt/test.cfg`,
      and the per-module makefiles' `RTK_DIR`): same vendor\ insertion.
- [x] [CLAUDE.md](CLAUDE.md): "Repository Layout" + `vendor/zinc/BIN` refs.
- [x] [README.md](README.md) / [README.es.md](README.es.md): layout + prose.
- [x] [.gitignore](.gitignore): "intentionally NOT listed here" comment.
- [x] [.gitattributes](.gitattributes): no path-scoped rules -- no change.
- [x] CI [.github/workflows/release.yml](.github/workflows/release.yml):
      `Copy-Item pharlap\BIN\*.DLL` -> `vendor\pharlap\BIN\*.DLL`.
- [x] `build.sh` / `build.ps1` / `run.sh` / `run.ps1` -- no-op confirmed
      (their `util\...` refs are `st\util\...`, run after `cd ST`).
- [x] [RELEASING.md](RELEASING.md): `pharlap/BIN/*.DLL` packaging note.

Done in one big commit -- the build won't pass until every path is
updated together, so partial commits would be broken intermediate states.

## Milestone: Maintenance hygiene

- [x] Centralize version into `st/include/version.h`; bump scripts keep
      `version.h` / `CLAUDE.md` / `st/versions.txt` in lockstep
      (see [RELEASING.md](RELEASING.md))
- [x] Automate release builds via GitHub Actions
      ([.github/workflows/release.yml](.github/workflows/release.yml));
      tag push → build under DOSBox-X on `ubuntu-latest` (xvfb) → zip +
      attach to a GitHub Release
- [x] Wire `ST_VERSION` into a runtime consumer. `st/include/st_defs.h`
      now `#include`s `version.h` and derives the legacy `APP_MAJOR_VER`,
      `APP_MINOR_VER`, `APP_UPGRADE_VER`, `APP_VER_ID`, `APP_VER`, and
      `APP_BUILD` macros from `ST_VERSION_*`. The About dialog
      (`mb_help.cpp`) and file-header stamping (`filehdr.cpp`) auto-track.
      `.autodepend` triggers a PCH rebuild on every `bump-version.*`,
      so the displayed version always matches the tag.
- [x] Deleted stale `st/web/` (2003 deployment page with broken links;
      `versions.txt` there was a frozen duplicate of `st/versions.txt`).
      Current releases live on GitHub Releases per [RELEASING.md](RELEASING.md).
- [x] `st/include/help.hpp` is a byproduct of `genhelp` — gitignored.
      `help.dat` / `help.hpp` are now built unconditionally by MAKEFILE
      (removed `!if $d(HELP)` guard); `HELP=1` is no longer needed.
- [x] Renormalize legacy LF-only DOS files to CRLF. Audit found one
      legit case (`zinc/EXAMPLE/BIO/MAKEFILE`, stale CRs in repo blob)
      and surfaced a `.gitattributes` bug: `*.prj` was classed `text
      eol=crlf` but Borland Turbo C Project files are binary, so the
      filter was inflating them with spurious CRs on checkout. Fixed
      by reclassifying `*.prj` as binary.
- [x] **Document the Zinc Designer workflow for `RES.DAT` edits.** Documented
      at [wiki/dev/zinc-designer-workflow.md](wiki/dev/zinc-designer-workflow.md);
      verified by Gonzalo 2026-06-09. One encoding unknown remains inline
      (needs testing in the Designer).

---

## Idea backlog (not yet a milestone)

Loose notes that aren't ready to be scheduled. Promote into a milestone
when scoped.

- [x] **Print all receipts as PDF** — `P_PORT=pdf` in `st.ini` routes all
  spooler output through `pdf_wr.c`. Receipts are written to
  `bin/PDF/RXYYMMDD.pdf` (one file per day). Multi-receipt per page
  supported (form feeds become blank-line separators). Done on
  `feat/pdf-printer` branch.
- [x] **PDF print driver (one PDF per day)** — same implementation as above.
  `SPOOLER::Print` intercepts when `P_PORT="pdf"`, strips ESC/P codes,
  writes plain text through the PDF writer. No separate `pr_*.dll` needed —
  the interception happens at the spooler level, before printer drivers.
  compress with `gifsicle -O3`. Add to README.md Screenshots section
  and wiki/dev/ for the DEMO_ENGINE milestone documentation.
- **Add original manuals** — include the original SmartTar manuals
 (scanned or typed) in the wiki `dev/` or `docs/` section as
 historical reference material.
- **Refresh README + recover original ST manual** — (a) capture new
  screenshots of the current UI (incl. demo mode) and update the README
  Screenshots section; (b) recover the original SmartTar manual from the
  Word source docs in [st/docs/archive/](st/docs/archive/) (`help.doc`,
  `STC.doc`, the four `*.docx` manuals). Overlaps the "Add original
  manuals" note above — fold together when scoped into a milestone.

- [x] **Demo mode visual indicator** — background color shifts from
  `GREEN` to `CYAN` in demo mode; window title shows `[DEMO]` prefix;
  exit dialog and about dialog also show demo indicator.
## Milestone: Vendor separation (private repo) — `feat/vendor-separation`

Move proprietary toolchain binaries (Borland C++ 3.1, Pharlap 286, Zinc 3.5,
DOS utilities) to a separate private repository to avoid copyright /
redistribution issues in the main smarttar repo.

**Private repo:** [`smarttar-vendor`](https://github.com/contento/smarttar-vendor)

- [x] Create `smarttar-vendor` private repo with vendor/ contents
- [x] Add `setup-vendor.sh` / `setup-vendor.ps1` to clone vendor/ from private repo
- [x] Add `VENDOR_SETUP.md` with manual setup instructions (Zinc 3.5, BC 3.1, Pharlap 286)
- [x] Update `.gitignore` to exclude `vendor/`
- [x] Update `build.sh` / `build.ps1` — check for vendor/, guide user if missing
- [x] Update `run.sh` / `run.ps1` — same vendor/ check
- [x] Update `README.md` / `README.es.md` — vendor setup instructions in Quick Start
- [x] Update `CLAUDE.md` — repository layout reflects vendor is external
- [x] Remove `vendor/` from this repo's git history (once private repo is confirmed working)
- [x] Update CI workflow (`.github/workflows/release.yml`) to clone vendor/ before build
- [x] Verify build works with vendor/ cloned from private repo (not tracked locally)
