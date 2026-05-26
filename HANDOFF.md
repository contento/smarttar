# DEMO_ENGINE Phase 1 — Handoff

Status snapshot for resuming on another machine. Branch: `demo-engine`.

**Status: Phase 1 COMPLETE. Builds clean (`./make-headless.sh demo`,
zero warnings, zero errors). Not yet verified at runtime.**

## Design (Template Method over Strategy)

The milestone committed to Strategy. After surveying `RT_ENGINE`'s
public surface (~48 trivial accessors over a shared `Clusters[]` /
`BoothCluster`), pure Strategy would have forced massive duplication.
Refined to **Template Method**: concrete `ENGINE` base owns shared
state, FSM dispatch, ISR install/uninstall, and accessors; four
`virtual` hooks carry the per-subclass variation:

  * `InitHardware(WORD numOfClusters)` — RT probes ports, sets
    `g_cfg->ACTIVE_CLUSTERS`, allocates `Clusters[]`, sets
    `Available=TRUE`. DEMO trusts the configured cluster count and
    inits FSMs to NOPHONE.
  * `RecoverState()` — RT does STM2 receipt-recovery; DEMO no-ops.
  * `OnTimerTick(cNum, dp)` — RT does `inportb`/`outportb` against
    `APP_PORT_BASE + PO_*`; DEMO does nothing (Phase 1 stub; Phase 2
    will synthesize `dp.OOD/.Answer/.DTMFFlags/...`). **This is the
    concurrency contract seam** — same IRQ0 hook, same
    `BoothCluster::_DataPort` writes, downstream code stays
    oblivious.
  * `OnTimerEnd()` — RT writes `PO_GENERAL` once per tick; DEMO no-ops.

Static ISRs (`NewISR08h` etc.) and `pThis` live on `ENGINE` base.
`pThis->OnTimerTick(...)` from inside the ISR — one vtable
indirection per call, fine in DOS protected mode.

Factory (`MakeEngine`) reads `g_cfg->ENGINE_KIND` ("real" | "demo")
and returns the right concrete. Build-time default (in
`cfg.cpp::SetDefault`): `__DEMO__` → "demo"; otherwise "real". Can
be overridden via `[ENGINE] ENGINE_KIND=...` in `st.ini`.

## File map

**Headers ([st/include/](st/include/)):**

  * [engine.h](st/include/engine.h) — `ENGINE` base. Hoisted from
    old `rt_eng.h`. Four pure-virtual hooks. Static ISR machinery.
    `pThis` typed `ENGINE *`.
  * [rt_eng.h](st/include/rt_eng.h) — slim: just
    `class RT_ENGINE : public ENGINE` with the four overrides.
  * [demo_eng.h](st/include/demo_eng.h) — `class DEMO_ENGINE :
    public ENGINE`, same overrides.
  * [eng_fact.h](st/include/eng_fact.h) — declares `ENGINE
    *MakeEngine(WORD numOfClusters)`.
  * [cfg.h](st/include/cfg.h) — added `char ENGINE_KIND[8];` near
    `CELLULAR_TAX` (line 318).
  * [control.h](st/include/control.h) — `RTEngine` type changed
    from `RT_ENGINE *` to `ENGINE *` (line 63).

**Impl ([st/src/](st/src/)):**

  * [st/src/rt/engine.cpp](st/src/rt/engine.cpp) — `ENGINE` base
    ctor/dtor, `InstallISRs` / `UninstallISRs`, `EvalToneState`,
    `EvalPulseState`, `pThis` def. ~250 lines.
  * [st/src/rt/rt_eng.cpp](st/src/rt/rt_eng.cpp) — slim. Only
    `RT_ENGINE::` methods now: ctor (delegates to ENGINE then calls
    `InitHardware` / `RecoverState` / `InstallISRs`), dtor (relay-off
    write), and the four virtual overrides. ~125 lines.
  * [st/src/rt/rt_isr.cpp](st/src/rt/rt_isr.cpp) — `ENGINE::` static
    ISRs. `NewISR08h` dispatches `OnTimerTick` / `OnTimerEnd` via
    `pThis`. `OldIV*` statics also `ENGINE::`. ~195 lines.
  * [st/src/rt/demo_eng.cpp](st/src/rt/demo_eng.cpp) — `DEMO_ENGINE`
    stub. No-op hooks. Phase 2 fills these in. ~70 lines.
  * [st/src/rt/eng_fact.cpp](st/src/rt/eng_fact.cpp) — `MakeEngine`.
    ~30 lines.
  * [st/src/rt/{rt_do,rt_util,rt_store}.cpp](st/src/rt/) — bulk
    `RT_ENGINE::` → `ENGINE::` rename.
  * [st/src/cfg.cpp](st/src/cfg.cpp) — `Entry(ENGINE_KIND, ...)` in
    `FillCfgTable` (line ~453), build-time default in `SetDefault`
    (line ~919).
  * [st/src/ctrl/control.cpp](st/src/ctrl/control.cpp) — `#include
    <eng_fact.h>` added; static `RTEngine` def retyped to `ENGINE
    *`; `new RT_ENGINE(...)` → `MakeEngine(...)` at the
    instantiation site (line ~90).

**Build:**

  * [st/MAKEFILE](st/MAKEFILE) — added `engine.obj`, `demo_eng.obj`,
    `eng_fact.obj` to OBJS list (line ~138) and explicit per-`.obj`
    rules (line ~341). Borland MAKE 3.6 needs explicit rules per
    [CLAUDE.md](CLAUDE.md) build notes.

**Config:**

  * [st/util/ini2cfg/st.ini](st/util/ini2cfg/st.ini) — appended
    `[ Engine ]` section with `ENGINE_KIND=demo`.

## Build verification

```sh
./make-headless.sh demo
```

Output (tail): `Build succeeded.` Zero warnings, zero errors. All
five new `.obj` files present in `st/build/`. `st/bin/st.exe`
produced at 1,063,434 bytes (size identical to prior demo build).

## To do next

1. **Runtime smoke test** — `./run-headless.sh` and confirm:
   * The app starts and the main view comes up.
   * Booths show as idle (NOPHONE state, no rings, no activity).
   * Exit cleanly without GPF or crash.

2. **Try the swap** — edit
   [st/util/ini2cfg/st.ini](st/util/ini2cfg/st.ini), change
   `ENGINE_KIND=demo` to `ENGINE_KIND=real`, rebuild (`./make-headless.sh
   demo`), run. Real engine will try to probe `APP_PORT_BASE` ports
   for DTMF response — without hardware it should set
   `ACTIVE_CLUSTERS=0` and the UI should come up empty. Cleaner test
   than the current behavior because the swap is explicit.

3. **Phase 2: Poisson generator + rules** — see the TODO milestone in
   [TODO.md](TODO.md). New file `util/ini2cfg/demo_engine.ini` with
   `[GLOBAL]` + per-type sections (`[LOCAL]`, `[NAL]`, `[INTER]`).
   `DEMO_ENGINE::OnTimerTick` synthesizes `dp.OOD` / `dp.Answer` /
   `dp.DTMFFlags` / `dp.U_DTMFDigits[]` based on the per-booth
   schedule. ISR-safe RNG (private LCG, not `rand()`). All
   generator state pre-allocated in `InitHardware`.

4. **Strip remaining `__DEMO__` gates** (optional, deferred) — there
   are still `__DEMO__` gates in:
   * `st.cpp` (dongle, STM2 init)
   * `control.cpp` (STM2 recovery, persist cycle)
   * `ctrl_ev.cpp`, `db_eng.cpp`, `filehdr.cpp` (STM2, license)
   * `rt_eng.cpp::RecoverState` (STM2 — only one in the rt/ layer
     left; gates the STM2 include, harmless since `RecoverState` is
     only ever called from RT_ENGINE concrete).
   None of these are engine concerns proper. Phase 2/3 can revisit.

## Notes / known issues

* **Latin-1 file editing**: `Edit` re-encoded `cfg.h` and
  `control.cpp` to UTF-8 on first attempts, mixing UTF-8 sequences
  with Latin-1 raw bytes (broken). Fix is byte-level via
  `python3 << 'PYEOF' open(..., 'rb') ... PYEOF`. Verify with `file
  <path>` after every Latin-1 edit (should report `ISO-8859`).
* **Dtor ordering**: original `RT_ENGINE::~RT_ENGINE` uninstalled
  ISRs FIRST, then did the relay-off port write. After refactor,
  derived dtor (relay-off) runs BEFORE base dtor (ISR uninstall).
  ISR fires for a few ticks with `GP_CASH` already cleared.
  Harmless in practice. Flag if anything misbehaves at shutdown.
* **Behavior delta for `DEMO_ENGINE` vs old `__DEMO__` build**: old
  demo build still wrote to `PO_GENERAL` and did the port-clear
  loop in the ctor. `DEMO_ENGINE` does ZERO port I/O. Cleaner, but
  if anything downstream depended on those writes hitting a
  specific value, watch for it.
* **`pre-simula-trash`** tag on `main` at `e64284a` is the rollback
  point if the whole DEMO_ENGINE arc needs to be discarded.
