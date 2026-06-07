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

- [ ] **Zinc Grid issue** — *(describe symptoms, reproduction, suspected
      cause)*
- [ ] *(add other UI bugs as they surface)*

## Milestone: UI improvements (new edition) — v4.0

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
  binaries via `-DSTYLE=...` build flag + `build` `-Style`
  wiring); dropped as a product decision. Do not revive without
  reopening the topic explicitly.

## Milestone: `DEMO_ENGINE` — pluggable engine for demo mode [Phases 1-2 DONE, merged to main]

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
- [~] **Graceful stop (drain, not freeze).** Stopping the simulation
  must hang up in-progress calls cleanly instead of freezing them
  mid-FSM. `TogglePaused()` now enters a *draining* state: `OnTimerTick`
  stops new arrivals and drives every active booth to hang up (clear
  `OOD`), so connected (TALK) calls run the normal `TALK -> STORE ->
  StoreReceipt` settlement (receipt + RX.DAT) and pre-answer dial
  attempts abort with no record; once all booths are idle it sets
  `_paused`. The FSM (`EvalToneState` in the ISR) keeps ticking while
  paused, so the forced hang-ups settle even after the pause latches.
- [~] **Graceful drain on exit.** `~CONTROLLER` flushes the receipt
  *queue* but active TALK calls were never enqueued, so they were lost
  on quit. New public `ENGINE::ForceStoreActiveCalls()` enqueues a
  receipt for every booth still in TALK (past `T_TALK`); called before
  the existing flush loop so those calls are printed/stored on exit.
- [~] **Total simulation time limit (default 60 min).** New
  `total_minutes` key in `demo.ini [GLOBAL]` (0 = unlimited). The demo
  engine counts run-ticks (100 Hz -> 6000/min) and, when the cap is
  reached, triggers the same graceful drain as a manual stop. A manual
  resume resets the elapsed counter (fresh budget).

## Milestone: Toolchain portability — v4.0

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

## Milestone: Documentation -- Obsidian wiki (EN + ES)

**Status (2026-06-07): substantially built, living at root [wiki/](wiki/)**
(moved there from `st/docs/wiki` -- matches the structure draft below). The
EN + ES User's Guide, Reference Manual, and in-app help pages exist, with a
self-locating `build-docs.sh` + `manifest.json` pandoc / `md2help.py` pipeline
that regenerates the `.docx` manuals and `help.txt` into `wiki/_build/`
(promoted to `st/docs/`). The shipped layout diverged from the draft below
(organized as users-guide / reference-manual / help, not the
Build&Run/Architecture/Operations vault sketched here). **Remaining:** fold in
the dev/maintenance docs listed below, and reconcile the checklist with what
actually shipped. The checkboxes below describe the *original plan*, not
current state.

Bring the scattered docs ([README.md](README.md), [README.es.md](README.es.md),
the four `.docx` manuals in [st/docs/archive/](st/docs/archive/), the in-app `help.txt` in [st/res/](st/res/),
[STABILITY_AUDIT.md](STABILITY_AUDIT.md), [HANDOFF.md](HANDOFF.md),
[RELEASING.md](RELEASING.md), [CLAUDE.md](CLAUDE.md),
[dosbox-x-smarttar-setup.md](dosbox-x-smarttar-setup.md),
[ISR_VOLATILE_NOTES.md](ISR_VOLATILE_NOTES.md)) into a single
browsable wiki built on Obsidian. Obsidian was picked because: (1) it
treats a folder of plain Markdown as a vault -- no proprietary format,
no server, git-friendly; (2) it has native bilingual support via
separate vaults; (3) wikilinks, backlinks, and the graph view make
cross-references between the tariff engine, dial plan, and printer DLLs
discoverable; (4) it works offline, which matches the project's
"single machine in a DOS booth" deployment story.

**Structure.** Two parallel vaults to match the existing
README.md / README.es.md split:

```text
wiki/
  en/                      Obsidian vault -- English
    .obsidian/             Obsidian config (theme, plugins, hotkeys)
    Home.md                Landing page, mirrors README structure
    Build & Run/           DOSBox-X setup, build variants, make flags
    Configuration/         st.ini, ph_info.dat, demo.ini schemas
    Architecture/          Subsystem map, call lifecycle, ISR contract
    Reference Manual/      Imported from SmartTar_Reference_Manual_EN.docx
    User Guide/            Imported from SmartTar_Users_Guide_EN.docx
    Operations/            Receipts, statistics, dongle, modem, EEPROM
    Troubleshooting/       Common errors, log diagnosis, recovery
    History/               1993-2003 era, MicroDiseno Ltda., dial plan
  es/                      Obsidian vault -- Espanol (mirror of en/)
    .obsidian/
    Inicio.md
    Compilacion y ejecucion/
    Configuracion/
    Arquitectura/
    Manual de Referencia/  Imported from SmartTar_Manual_de_Referencia_ES.docx
    Guia del Usuario/      Imported from SmartTar_Guia_del_Usuario_ES.docx
    Operaciones/
    Solucion de problemas/
    Historia/
```

**README simplification.** Once the wiki holds the depth, the
top-level README.md / README.es.md shrink to: project tagline,
screenshot, 5-line "what it is", one paragraph "how to build", and a
prominent "Full documentation -> [wiki/en/](wiki/en/Home.md)" /
"Documentacion completa -> [wiki/es/](wiki/es/Inicio.md)" pointer.
Everything currently in the "Configuration" / "Architecture" /
"Runtime Data Files" sections moves into wiki pages; the README keeps
only what a first-time visitor needs in the first 30 seconds.

**Manual ingestion.** The four `.docx` manuals in `st/docs/archive/` are the
authoritative content but locked in a Word-era format. Conversion path:

  1. Bulk-convert `.docx` -> `.md` with `pandoc -t gfm-raw_html`
     (clean Markdown, no inline HTML).
  2. Manual cleanup pass: chapter -> file split (Obsidian prefers
     one topic per note), wikilink conversion (`[chapter X]` ->
     `[[Chapter X]]`), image relinking (the `.docx` embedded images
     need to be extracted to `attachments/` and re-referenced).
  3. The original `.docx` files stay in [st/docs/archive/](st/docs/archive/) as the
     historical record but are no longer the working copy. Add a
     "Source: st/docs/archive/SmartTar_Reference_Manual_EN.docx" footer to
     each imported page.
     each imported page.

**`help.txt` cross-reference.** The in-app help text
([st/res/help.txt](st/res/help.txt)) drives `bin/help.dat` via
`genhelp`. It overlaps the Reference Manual but is keyboard-focused.
Cross-link each wiki page to its matching help.txt section so editors
know to update both when behavior changes.

**Out of scope.** Not building a docs site (no Jekyll / Hugo /
mdBook). The wiki ships as a git-tracked Obsidian vault; viewers
either open it in Obsidian or read the raw Markdown on GitHub. If a
hosted site is wanted later, point a static-site generator at the
same Markdown.

- [ ] Decide vault layout (`wiki/en/` + `wiki/es/` per draft above,
      or single bilingual vault with `lang::` frontmatter). Locks in
      naming conventions for the rest of the tasks.
- [ ] Convert the four `.docx` manuals to clean Markdown via
      pandoc; extract embedded images to `wiki/<lang>/attachments/`.
- [ ] Split converted manuals into Obsidian-sized pages (one topic
      per file) and add wikilinks between related sections.
- [ ] Port `README.md` "Configuration" / "Architecture" / "Runtime
      Data Files" sections into wiki pages; replace those sections
      in the README with a short pointer.
- [ ] Mirror the EN vault into the ES vault, with translations
      sourced from `SmartTar_Manual_de_Referencia_ES.docx` and
      `SmartTar_Guia_del_Usuario_ES.docx` where they cover the
      same material.
- [ ] Add a `wiki/README.md` (LF, host-side) explaining the vault
      layout, how to open it in Obsidian, and which plugins (if
      any) the vault depends on.
- [ ] `.gitattributes`: confirm `wiki/**/*.md` follows host-side
      LF policy (Obsidian writes LF). Add an `*.canvas` rule if
      we use Obsidian Canvas files.
- [ ] Cross-link wiki pages to `help.txt` sections so future help
      edits stay in sync.

Out-of-scope follow-ups: hosted docs site, search index, automated
docx -> md regeneration on push (manuals change rarely).

## Milestone: Stability under Extended DOS

Findings from [STABILITY_AUDIT.md](STABILITY_AUDIT.md) — see that file for
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

**Remaining batches (drain into the four lists above as scheduled):**

- [x] Address remaining CRITICAL findings (audit § 3 C5-C22) -- **ALL DONE.**
      C5–C9 (`58e68d5`), C11–C22 (`f497445`), C19 (`d4bbd49`),
      C14/C15 (`d7d24ca`). C10 verified-defended (no code change).
- [~] Address HIGH findings (audit § 3) -- 3 mechanical fixes done
      (`aeb1372`: cfg `memmove`, control `delete[]`, spooler null-order).
      Remainder triaged in the audit: bounds/IO (Bucket B) and
      ISR/concurrency (Bucket C, needs load testing) -- both open.
- [ ] Address MEDIUM / LOW findings (audit § 3) -- opportunistically.
- [ ] Verification pass per audit § 6 once the spot-verified set is
      cleared.

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
- [x] `st/test/*` build configs/makefiles (`test.cfg`, `pr_fmt/test.cfg`,
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
- [~] Document the Zinc Designer workflow for `RES.DAT` edits. Draft at
      [wiki/dev/zinc-designer-workflow.md](wiki/dev/zinc-designer-workflow.md);
      `FIXME` markers tag the parts Gonzalo still needs to verify.

---

## Idea backlog (not yet a milestone)

Loose notes that aren't ready to be scheduled. Promote into a milestone
when scoped.

- **OpenZinc** — open-source continuation of Zinc Interface Library;
  may be relevant for UI bug fixes or toolchain portability work.
  <http://www.openzinc.com/index.html>
- **Open Watcom** — open-source successor to Watcom C/C++; closest
  drop-in replacement for Borland C++ 3.1 targeting 16/32-bit DOS.
  Could replace BCC286 + Pharlap in the "Toolchain portability"
  milestone (see above).  <https://github.com/open-watcom/open-watcom-v2>
- **Print all receipts as PDF** — new entry under the Tools menu
  (`tb_tools.cpp` / `mb_print.cpp`) that batch-exports every receipt
  in RX.DAT to a single PDF file (one receipt per page). DOS has no
  native PDF library, so two paths to scope: (a) emit raw PDF
  bytestream from C++ (small subset of PDF 1.4 -- text + simple
  layout is feasible in pure code); (b) print to a virtual PDF
  printer driver loaded as a new `pr_*.dll` and use the existing
  spooler path. Path (a) is self-contained, path (b) reuses the
  printer abstraction. Decide before scoping.
- **PDF print driver (one PDF per day)** -- a new `pr_*.dll` that
  plugs into the existing physical-printer path (same per-receipt
  format/spooler flow as the real drivers in `src/pr/pr_*.c`), but
  instead of emitting to a physical port it appends each printed
  receipt to a daily PDF file (e.g. `RX-YYYYMMDD.pdf`, one receipt
  per page, rolled over at the turn-of-day boundary). This is the
  "live" counterpart to the bulk "Print all receipts as PDF" entry
  above -- it captures receipts as they are printed rather than
  exporting RX.DAT history after the fact, and concretely realizes
  that entry's path (b). Reuses the same emit logic, so scope the
  raw-PDF bytestream writer once and share it between both.
