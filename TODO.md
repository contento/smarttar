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

## Milestone: `DEMO_ENGINE` — pluggable engine for demo mode

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

- [ ] Extract an `ENGINE` interface (pure-virtual base) from
      `RT_ENGINE`'s public surface. Must include the ISR lifecycle
      (`Install()` / `Uninstall()` for IRQ0 + the keyboard / break /
      crit-error vectors) so concretes own their own vector
      management — not just data accessors.
- [ ] `RT_ENGINE` becomes a concrete implementing the interface; the
      existing `NewISR08h` / `NewISR09h` / `NewISR23h` / `NewISR1Bh` /
      `NewISR24h` move behind `Install()`. No behavior change in
      production builds.
- [ ] Empty `DEMO_ENGINE` concrete that compiles and links. Its
      `Install()` hooks IRQ0 with a stub `DemoISR08h` that does
      nothing (booths stay idle). Proves the swap end-to-end before
      Phase 2 adds real generation.
- [ ] Factory wired to `[ENGINE] kind = real | demo` in `st.ini`
      (default `real`). Add the key via `ini2cfg`.
- [ ] Strip `#ifdef __DEMO__` from the RT layer; the gate becomes
      "factory instantiates `DEMO_ENGINE`" instead. `__DEMO__` may
      remain elsewhere (dongle, EEPROM) where it gates non-engine
      concerns — review case by case.

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

- [ ] Implement Poisson arrival generator (one stream, classified into
      `LOCAL` / `NAL` / `INTER` by the per-type weights). All
      generator state pre-allocated at `Install()` time so the ISR
      never touches the heap.
- [ ] Ship a private ISR-safe RNG (LCG, fixed seed from config) —
      `rand()` is not reentrant under Borland C++ 3.1.
- [ ] Duration sampling: uniform in `[min, max]` for v1 (exponential /
      normal can come later if needed).
- [ ] Destination numbers: draw from the existing `.inf` place tables
      (`util/inf2dat/local.inf`, `ddn.inf`, `ddi.inf`) so tariff
      calculation exercises real data.
- [ ] Parse `demo_engine.ini` in `DEMO_ENGINE` ctor using the same
      patterns as `cfg.cpp`; fail loud if missing or malformed when
      `kind = demo`.
- [ ] `DemoISR08h` writes `DataPort.OOD` / `.Answer` / `.ThreadC` /
      `.DTMFFlags` / `.U_DTMFDigits[]` (same fields RT's ISR writes)
      to advance each booth through ONHOOK → RINGUP → OFFHOOK →
      ANSWER → TALK → ONHOOK on its scheduled timeline.

**Phase 3 — Polish (optional, not blocking Phase 1/2).**

- Time-of-day variation (peak vs off-peak arrival rates).
- Scripted scenario replay (a recorded `.scn` sequence for repeatable
  regression tests).
- Lightweight operator controls — start/stop generator, reload rules.
  Must be a new widget / menu entry; the old `UIW_SIMULA` window is
  off-limits per the milestone framing.

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

- *(empty — add as ideas arise)*
