# SmartTar TODO

Working list of milestones and tasks. Detailed findings live in
[STABILITY_AUDIT.md](STABILITY_AUDIT.md); this file is the day-to-day
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
      [make-headless](make-headless.sh))
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

- [ ] **Zinc Grid issue** — *(describe symptoms, reproduction, suspected
      cause)*
- [ ] *(add other UI bugs as they surface)*

## Milestone: UI improvements (new edition)

- [ ] **Higher-resolution display via Zinc** — Zinc 3.5 supports SVGA
      modes through its BGI backend; the project currently runs at the
      default VGA 640×480. Investigate switching `machine = svga_s3` +
      a higher Zinc display mode, the cost in font rework, and how
      `st.cfg`'s `-D__BTN__` defines interact with resolution.
- ~~**Theme switching** — Zinc 3.5 has a built-in palette / "scheme"
  mechanism. Expose it as a runtime toggle (config option +
  menu-bar entry) so the operator can switch between palettes
  without recompiling.~~ **Abandoned 2026-05-25 — dead end.**
  Investigated on the `zinc-theme-switching` branch (multi-style
  binaries via `-DSTYLE=...` build flag + `make-headless` `-Style`
  wiring); dropped as a product decision. Do not revive without
  reopening the topic explicitly.

## Milestone: Toolchain portability

- [ ] **Build with open-source toolchains** — investigate whether
      SmartTar can build with Open Watcom (closest historical
      replacement for Borland C++ + Pharlap), DJGPP / GCC, or a more
      modern C++ targeting 32-bit DOS. Two flavors:
  - *Drop-in*: keep Zinc 3.5 + Pharlap, change only the compiler.
    Risk: Zinc has BCC-specific calling conventions and PCH usage.
  - *Full replacement*: swap Zinc for an open UI lib (Turbo Vision?
    custom BGI? FOX?) and/or replace Pharlap with HX-DOS / CWSDPMI /
    DOS/4GW. Bigger surgery; loses some Zinc behavior we may want to
    preserve.
  Document risk/reward per layer (compiler, DOS extender, UI lib)
  before committing to a path.

## Milestone: Stability under Extended DOS

Findings from [STABILITY_AUDIT.md](STABILITY_AUDIT.md) — see that file for
full context and severity rationale.

- [ ] Address CRITICAL findings (audit § 3)
- [ ] Address HIGH findings (audit § 3)
- [ ] Address MEDIUM / LOW findings (audit § 3) — opportunistically
- [ ] Verification pass per audit § 6

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
- [~] Document the Zinc Designer workflow for `RES.DAT` edits in
      `docs/` so future work doesn't re-discover it. Draft at
      [st/docs/zinc-designer-workflow.md](st/docs/zinc-designer-workflow.md);
      `FIXME` markers tag the parts Gonzalo still needs to verify.

---

## Idea backlog (not yet a milestone)

Loose notes that aren't ready to be scheduled. Promote into a milestone
when scoped.

- **`DEMO_ENGINE`: a fake `RT_ENGINE` for demo mode** — `RT_ENGINE`
  (`rt/rt_eng.cpp`) is the real-time engine that talks to the
  booth-cluster hardware: ISR-driven port reads, state machine over
  ONHOOK/RINGUP/OFFHOOK/DTMF/BREAK/MAKE, serial driver, EEPROM, etc.
  In dev (no booths, no PBX, no dongle) the `__DEMO__` build gate
  currently stubs the hardware paths inline across `rt_eng.cpp`,
  `rt_isr.cpp`, and `ctrl_ev.cpp`. Replace those scattered gates
  with a clean engine swap:
  - `RT_ENGINE` — unchanged: real hardware, real ports, real ISR.
    Used in production.
  - `DEMO_ENGINE` — a parallel implementation with the same public
    surface but no hardware dependencies. Generates a realistic
    event stream (scripted sequences for repeatable demos; could
    grow a Poisson arrival generator later). No ISR, no serial, no
    EEPROM. Used in demo builds and dev.
  Goal: every `#ifdef __DEMO__` in the RT layer goes away; the
  controller holds a pointer to an `ENGINE` base class and never
  cares which concrete is behind it.
  Suggested GoF patterns:
  - **Strategy** (primary fit) — define an `ENGINE` interface
    (pure-virtual base class) capturing what the controller / view
    layer actually needs from the engine. `RT_ENGINE` and
    `DEMO_ENGINE` are interchangeable concrete strategies. Your
    "adapter" framing maps to Strategy here, not to GoF Adapter
    (which wraps an existing incompatible interface — `DEMO_ENGINE`
    isn't wrapping anything, it's a parallel implementation).
  - **Factory Method / Abstract Factory** — pick the concrete from
    config (e.g. `[ENGINE] kind = real | demo` in `st.ini`) at
    startup. Replaces the build-time `__DEMO__` split with a
    runtime switch, so a single binary can do either.
  - **Template Method** (alternative to Strategy) — if `RT_ENGINE`
    and `DEMO_ENGINE` end up sharing significant init / dispatch
    scaffolding, lift it into an `ENGINE_BASE` with the diverging
    bits as `virtual ReadNextEvent() = 0` etc. Pick this *or*
    Strategy, not both: Template Method puts variation in
    subclasses, Strategy puts it in a composed object — using both
    invents needless layers.
  - Likely **not** State (engine doesn't switch concretes at
    runtime), **not** Observer at this layer (the existing event
    bus already handles fan-out downstream).
  **Out of scope — explicitly NOT touching `UIW_SIMULA`.** The old
  `UIW_SIMULA` window (F2 from the main view, in `mb_simul.cpp`,
  plus `RT_ENGINE::SIMULA` state and `Simula[]`/`SimulaPhones[]`
  in the booth cluster) is a separate, abandoned initiative for
  *hardware* testing — let an operator manually drive booth state
  transitions to verify a real install. That code stays as-is;
  this initiative builds a new engine for *demo* purposes and does
  not refactor, extend, or remove the old SIMULA feature.
