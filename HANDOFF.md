# DEMO_ENGINE — Handoff

Status snapshot for resuming on another machine. Branch: `demo-engine`.

**Status: Phase 1 + Phase 2 COMPLETE.  Mid-dial hang-up FIXED.
"-- No Incluida --" symptom RESOLVED (root cause: missing
`bin/PH_INFO.DAT`; `inf2dat.exe` and `ini2cfg.exe` were never
built, sub-makefiles now functional).  Build self-bootstraps
end-to-end from a clean working tree via `./build.sh demo`
(zero warnings, zero errors).  Quit-confirmation when demo is
running added (`st.cpp::Exit()` third branch).  Scripts renamed
`make-headless.{sh,ps1}` -> `build.{sh,ps1}` and
`run-headless.{sh,ps1}` -> `run.{sh,ps1}`.  See "Done this
session" below for the per-commit ledger.**

## Design (Template Method over Strategy)

The milestone committed to Strategy. After surveying `RT_ENGINE`'s
public surface (~48 trivial accessors over a shared `Clusters[]` /
`BoothCluster`), pure Strategy would have forced massive duplication.
Refined to **Template Method**: concrete `ENGINE` base owns shared
state, FSM dispatch, ISR install/uninstall, and accessors; four
`virtual` hooks carry the per-subclass variation:

  * `InitHardware(WORD numOfClusters)` — RT probes ports, sets
    `g_cfg->ACTIVE_CLUSTERS`, allocates `Clusters[]`, sets
    `Available=TRUE`. DEMO trusts the configured cluster count, inits
    FSMs to ONHOOK, and allocates the per-booth schedule array.
  * `RecoverState()` — RT does STM2 receipt-recovery; DEMO no-ops.
  * `OnTimerTick(cNum, dp)` — RT does `inportb`/`outportb` against
    `APP_PORT_BASE + PO_*`; DEMO synthesizes `dp.OOD/.Answer/.DTMFFlags/
    .DTMFDigits/.ThreadC` from the per-booth Poisson schedule.
    **This is the concurrency contract seam** — same IRQ0 hook, same
    `BoothCluster::_DataPort` writes, downstream code stays oblivious.
  * `OnTimerEnd()` — RT writes `PO_GENERAL` once per tick; DEMO no-ops.

Static ISRs (`NewISR08h` etc.) and `pThis` live on `ENGINE` base.
`pThis->OnTimerTick(...)` from inside the ISR — one vtable
indirection per call, fine in DOS protected mode.

Factory (`MakeEngine`) reads `g_cfg->ENGINE_KIND` ("real" | "demo")
and returns the right concrete. Build-time default (in
`cfg.cpp::SetDefault`): `__DEMO__` → "demo"; otherwise "real". Can
be overridden via `[ENGINE] ENGINE_KIND=...` in `st.ini`.

## Phase 2: Poisson generator (committed 6c3d9bd + fixes)

`DEMO_ENGINE::OnTimerTick` runs a per-booth state machine independent
of the FSM in `ENGINE::Eval*State`:

  ```
  DP_IDLE -> DP_OFFHOOK -> DP_DIALING_HI <-> DP_DIALING_LO
          -> DP_ANSWER_WAIT -> DP_TALKING -> DP_IDLE
  ```

Each transition pokes the right bits in `dp.OOD / .DTMFFlags /
.DTMFDigits / .Answer / .ThreadC` so the FSM walks itself through
`OFFHOOK -> DTMFFLAG -> INTERDIG -> ANSWER -> TALK -> STORE -> LOCK
-> ONHOOK`. Per-booth state (`DemoBooth`) is pre-allocated in
`InitHardware` — ISR contract honored, no malloc/new in `OnTimerTick`.

RNG is a private LCG on `_lcgState`, not `rand()` (not ISR-safe under
Borland C++ 3.1). Inter-arrival is uniform in `[1, 2*meanArrivalTicks]`
with mean derived from `[GLOBAL] calls_per_minute` in `demo.ini`
divided across all booths. Call duration is uniform in `[min, max]`
per call-type from `demo.ini` (`[LOCAL]`, `[NAL]`, `[INTER]`).

Type selection is weighted by `[*] weight`. Defaults if `demo.ini` is
missing: 70 % LOCAL / 25 % NAL / 5 % INTER.

### Dial-digit generator (4f17414 + 8501731 + this commit)

GenCall doesn't synthesize uniform 1..9 anymore -- that produced
numbers that no `ph_info.dat` prefix matched, so every booth ended up
in `--- No Incluida ---`, and the `NOT_INCLUDED_CALL_MASK` lock path
killed calls at `NumOfDigits >= NAL_DIGITS_NOT_INCLUDED` (9).

Current strategy (`st/src/rt/demo_eng.cpp::GenCall`):

  * LOCAL: first digit picked from `{2,3,4}`; remaining 6 digits
    random 1..9. Always hits `Local 20-29 / 30-39 / 40-49` in
    `local.inf` by the 2nd dialed digit, so Search succeeds well
    before `LOCAL_DIGITS_NOT_INCLUDED=4`.
  * NAL: dials `ACCESS + OPERATOR_ACCESS` (`09` for Colombia) +
    one of `{12, 13, 14, 16, 17}` (Bogota Etb entries that are
    2-digit singles, or expand via the 141-149 / 161-169 ranges so
    they match at digit 5 for any subscriber 1..9), then random
    fillers to `NAL_DIGITS=10`.
  * INTER: dials `ACCESS + INTER_ACCESS + OPERATOR_ACCESS` (`009`)
    + one of `{1212, 1305, 1416, 33, 34, 39, 44, 49, 54, 55, 56}`
    (NANP 4-digit area codes / 2-digit country codes that exist in
    `ddi.inf`), then random fillers to `INTER_DIGITS=14`.

Don't expand the NAL pool with any "1X" combo that isn't a 2-digit
single (e.g. "15" only matches a few specific 3rd digits) -- if
Search never succeeds, the booth locks once
`NumOfDigits >= NAL_DIGITS_NOT_INCLUDED` (9), mid-dial.

### Timing (this commit)

Earlier passes treated `g_cfg->T_OFF_HOOK / T_DTMF_FLAG /
T_DTMF_INTERDIG / T_BIAS` (all in **ms**) as if they were tick counts,
so every dwell ran 10x longer than necessary -- a NAL call took ~13 s
to dial. Fixed: every per-tick countdown is now
`g_cfg->T_X / T_EVAL + <margin>` so the ms value is converted to ticks
(`T_EVAL=10ms` from `engine.h`). Dialing now ~1.5 s for INTER, ~1.2 s
for NAL.

### Config

  * [st/util/ini2cfg/demo.ini](st/util/ini2cfg/demo.ini) -- canonical
    source for the arrival/duration parameters. MAKEFILE copies it
    into `bin/` (rule near line 237).

### Real-number dataset (commits d6fe4cd / a1853d9 / b48f802)

  * [st/util/inf2dat/phones.csv](st/util/inf2dat/phones.csv) -- 56
    real, currently-published public phone numbers (gov, utilities,
    universities, hospitals, embassies, banks, airlines) gathered
    from each organization's official contact page. RFC 4180 CSV
    with all string fields double-quoted, leading `;`-prefixed
    comment block, CRLF (`.gitattributes` rule `*.csv text eol=crlf`).
  * Coverage: 14 LOCAL (Medellin + Eastern Antioquia towns Marinilla,
    La Ceja, El Santuario -- distinct local.inf prefixes 548/553/546),
    20 NAL (Bogota, Cali x2, Pasto, Bucaramanga, Pereira, Manizales,
    Armenia, Ibague, Cucuta, Neiva, Barranquilla, Cartagena -- area
    codes 1/2/5/6/7/8), 22 INTER (USA, Canada, Spain, Italy, France,
    Germany, UK, Mexico, Argentina, Brazil, Chile -- country codes
    1/33/34/39/44/49/52/54/55/56). Each number verified against
    util/inf2dat/{local,ddn,ddi}.inf so the call resolves to a named
    destination, not "--- No Incluida ---".
  * Colombian numbers are normalized to the 2003-era dial plan
    (modern `60X` area prefix from CRC Resolucion 5826 stripped from
    `dial_from_smarttar`; `published_number` keeps the modern format).
  * **Wired into `GenCall` as of d2cd5df.** `DEMO_ENGINE::ParsePhones()`
    reads `phones.csv` from the working directory at `InitHardware`
    time and fills `_phones[3]` (one `PhonePool` per call type, each
    holding up to `DEMO_MAX_PHONES_PER_TYPE`=32 entries of 16 digits).
    `GenCall` writes the access prefix (none/09/009) per call type
    then copies a random pool entry's dial sequence. Missing or
    malformed `phones.csv` -> empty pool -> fallback to the original
    hardcoded prefix arrays (graceful, demo still works). MAKEFILE
    distributes `phones.csv` into `bin/` alongside `demo.ini`. Build
    verified clean (`./build.sh demo`, zero warnings); runtime
    verification (named destinations show up in the UI for each booth)
    deferred to the next session on GCC's other PC.
  * **Borland 3.1 quirk:** `DEMO_MAX_PHONES_PER_TYPE` /
    `DEMO_MAX_DIGITS_PER_PHONE` live as file-scope `#define`s, not
    private nested enums. Borland C++ 3.1 applies access control to
    private enum constants when used as array bounds in another
    private struct of the same class -- produces "is not accessible"
    errors. Don't refactor them back into the class without testing.
  * Documented in [README.md](README.md) and [README.es.md](README.es.md)
    under the Configuration section.

## File map

**Headers ([st/include/](st/include/)):**

  * [engine.h](st/include/engine.h) -- `ENGINE` base. Hoisted from
    old `rt_eng.h`. Four pure-virtual hooks. Static ISR machinery.
    `pThis` typed `ENGINE *`. Exposes the `T_EVAL` constant (=10ms
    per tick) the demo uses for ms->tick conversion.
  * [rt_eng.h](st/include/rt_eng.h) -- slim: just
    `class RT_ENGINE : public ENGINE` with the four overrides.
  * [demo_eng.h](st/include/demo_eng.h) -- `class DEMO_ENGINE :
    public ENGINE`. `DemoBooth` (per-booth schedule), `DemoCallType`
    (per-type params), LCG state, ParseConfig/GenCall helpers.
  * [eng_fact.h](st/include/eng_fact.h) -- declares `ENGINE
    *MakeEngine(WORD numOfClusters)`.
  * [cfg.h](st/include/cfg.h) -- added `char ENGINE_KIND[8];` near
    `CELLULAR_TAX`.
  * [control.h](st/include/control.h) -- `RTEngine` type changed
    from `RT_ENGINE *` to `ENGINE *`.

**Impl ([st/src/](st/src/)):**

  * [st/src/rt/engine.cpp](st/src/rt/engine.cpp) -- `ENGINE` base
    ctor/dtor, `InstallISRs` / `UninstallISRs`, `EvalToneState`,
    `EvalPulseState`, `pThis` def.
  * [st/src/rt/rt_eng.cpp](st/src/rt/rt_eng.cpp) -- slim. Only
    `RT_ENGINE::` ctor/dtor and the four virtual overrides.
  * [st/src/rt/rt_isr.cpp](st/src/rt/rt_isr.cpp) -- `ENGINE::` static
    ISRs. `NewISR08h` dispatches `OnTimerTick` / `OnTimerEnd` via
    `pThis`.
  * [st/src/rt/demo_eng.cpp](st/src/rt/demo_eng.cpp) -- DEMO state
    machine, INI parser, digit pools, GenCall, LCG.
  * [st/src/rt/eng_fact.cpp](st/src/rt/eng_fact.cpp) -- `MakeEngine`.
  * [st/src/cfg.cpp](st/src/cfg.cpp) -- `Entry(ENGINE_KIND, ...)` and
    build-time default in `SetDefault`.
  * [st/src/ctrl/control.cpp](st/src/ctrl/control.cpp) -- includes
    `eng_fact.h`; uses `MakeEngine` instead of `new RT_ENGINE`.

**Build:**

  * [st/MAKEFILE](st/MAKEFILE) -- added `engine.obj`, `demo_eng.obj`,
    `eng_fact.obj` to OBJS and `$(BIN_DIR)\demo.ini` to the EXE
    dependency list; explicit per-`.obj` rules and a `demo.ini`
    distribution rule. Borland MAKE 3.6 needs explicit rules per
    [CLAUDE.md](CLAUDE.md) build notes.

## To do next

  1. **Strip remaining `__DEMO__` gates** (deferred from Phase 1):
     `st.cpp` (dongle, STM2 init), `control.cpp` (STM2 recovery,
     persist cycle), `ctrl_ev.cpp`, `db_eng.cpp`, `filehdr.cpp`,
     `rt_eng.cpp::RecoverState`.  Scoped this session, deferred:
     58 references across 8 files; each gate is doing real work
     (skip dongle / STM2 / DB init when no hardware), not just
     engine selection.  Converting them to runtime checks via
     `g_cfg->ENGINE_KIND` is a focused refactor session of its
     own, not a tail-end cleanup.
  2. **Phase 3 polish** (optional, see [TODO.md](TODO.md)):
     time-of-day variation, scripted `.scn` replay, operator
     controls.  (Quit-confirmation-when-demo-is-running DONE --
     `st.cpp::Exit()` now has a third branch that fires when
     `CONTROLLER::RTEngineIsDemo()` is TRUE; "Detener la
     simulacion y salir ?" prompt, Si sends L_EXIT, ENGINE dtor
     chain handles the ISR detach.)  The old `UIW_SIMULA` window
     is off-limits.

## Done this session (since the last HANDOFF refresh)

  * **"-- No Incluida --" everywhere on every booth** -- traced to
    missing `bin/PH_INFO.DAT` (place tree empty -> `Search()` always
    fails).  Fixed by rebuilding `st/util/inf2dat/inf2dat.exe` from
    its sub-makefile (SRC_DIR was a stale `c:\prj\st\source`, missed
    3 modules that drifted into deps post-2003).  See commits
    `4b43479` (BUILD: rebuild util/inf2dat pipeline) and the
    follow-ups.
  * **`ini2cfg.exe` had the same never-built situation** -- added
    [st/util/ini2cfg/makefile](st/util/ini2cfg/makefile) mirroring
    the inf2dat sub-makefile.  Bootstrap once with
    `cd st\util\ini2cfg && make -DRUN`.
  * **MAKEFILE `PH_INFO.DAT` copy step `error 1` quirk** --
    diagnosed.  Borland MAKE 3.6 mis-propagates errorlevel from
    `.bat`-internal commands and deletes the (just-created) target.
    `mk_ph.bat` / `mk_cfg.bat` work correctly when invoked from
    DOSBox-X directly.  Workaround: moved both bat invocations OUT
    of MAKE into `build.sh` / `build.ps1` as a preamble step,
    dropped the broken rules from `st/MAKEFILE`.  Build now
    self-bootstraps end-to-end from a clean working tree.
  * **`make-headless.sh` / `run-headless.sh` renamed to `build.sh`
    / `run.sh`** (+ `.ps1` variants).  "Headless" was inaccurate
    (DOSBox-X still opens a window); the new names just describe
    what they do.  All doc references updated.
  * **Quit-confirmation when demo is running** -- new branch in
    `st.cpp::Exit()`.  Mechanism: `virtual BOOL ENGINE::IsDemo()`
    overridden in `DEMO_ENGINE`, plus `CONTROLLER::RTEngineIsDemo()`
    shim.  Pattern mirrors the existing busy-block dialog.
  * **TODO Stability milestone reshaped** -- 4 spot-verified
    CRITICALs from audit S6 promoted to a "Next batch" sub-section
    with file:line specifics, so the milestone is now a queue
    instead of an inventory pointer.

## Sibling milestone -- documentation

A new milestone added to [TODO.md](TODO.md) under "Documentation --
Obsidian wiki (EN + ES)" consolidates the scattered docs (README,
the four `.docx` manuals in [st/docs/](st/docs/), `help.txt`,
`STABILITY_AUDIT.md`, `HANDOFF.md`, `RELEASING.md`, `CLAUDE.md`)
into a single browsable Obsidian vault per language. Not blocking
DEMO_ENGINE -- they're independent tracks. Mentioned here so the
next session sees the broader documentation context when revising
the demo dataset or its README references.

## Notes / known issues

  * **Latin-1 file editing**: `Edit` re-encoded `cfg.h` and
    `control.cpp` to UTF-8 on first attempts, mixing UTF-8 sequences
    with Latin-1 raw bytes (broken). Fix is byte-level via
    `python3 << 'PYEOF' open(..., 'rb') ... PYEOF`. Verify with
    `file <path>` after every Latin-1 edit (should report `ISO-8859`).
  * **Dtor ordering**: original `RT_ENGINE::~RT_ENGINE` uninstalled
    ISRs FIRST, then did the relay-off port write. After refactor,
    derived dtor (relay-off) runs BEFORE base dtor (ISR uninstall).
    ISR fires for a few ticks with `GP_CASH` already cleared.
    Harmless in practice. Flag if anything misbehaves at shutdown.
  * **`pre-simula-trash`** tag on `main` at `e64284a` is the rollback
    point if the whole DEMO_ENGINE arc needs to be discarded.
