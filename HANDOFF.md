# DEMO_ENGINE Phase 1 — Handoff

WIP snapshot for resuming on another machine. Branch: `demo-engine`.

**Status: WIP. Does NOT build yet.** The headers, the new ENGINE base
implementation, the RT_ENGINE refactor, and the DEMO_ENGINE stub are
in. The wiring (config, controller, MAKEFILE) is not. Do NOT try to
`make` until the "To do" list below is done.

## What the milestone says

See [TODO.md](TODO.md) "Milestone: `DEMO_ENGINE` — pluggable engine
for demo mode" (in `main`, also visible here). Phase 1 goal: introduce
the `ENGINE` interface, refactor `RT_ENGINE` to inherit it, add a
`DEMO_ENGINE` stub, wire a factory off `g_cfg->ENGINE_KIND`, strip
`#ifdef __DEMO__` from the RT layer. No behavior change in `demo` build.

## Design decision (refining the milestone)

The milestone committed to Strategy. After surveying `RT_ENGINE`'s
public surface (~48 methods, all trivial accessors over a shared
`Clusters[]`/`BoothCluster`), pure Strategy would force massive
duplication. Refined to **Template Method**: concrete `ENGINE` base
owns all shared state and accessors; three `virtual` hooks carry the
variation:

  * `InitHardware(WORD numOfClusters)` — RT probes ports, sets
    `ACTIVE_CLUSTERS`, allocates `Clusters[]`, sets `Available=TRUE`.
    DEMO trusts the configured cluster count and inits FSMs to NOPHONE.
  * `RecoverState()` — RT does STM2 receipt-recovery; DEMO no-ops.
  * `OnTimerTick(cNum, dp)` — RT does `inportb`/`outportb` against
    `APP_PORT_BASE + PO_*`; DEMO synthesizes `dp.OOD/.Answer/...`
    (Phase 2). This is the concurrency-contract seam — same IRQ0
    hook, same `BoothCluster::_DataPort` writes, downstream code
    cannot tell which concrete is running.
  * `OnTimerEnd()` — RT writes `PO_GENERAL` once per tick; DEMO no-ops.

Static ISRs (`NewISR08h` etc.) and `pThis` live on `ENGINE` base, with
`pThis->OnTimerTick(...)` virtual dispatch from inside the ISR — one
vtable indirection per call, fine in DOS protected mode.

Factory (`MakeEngine`) reads `g_cfg->ENGINE_KIND` ("real"|"demo") and
returns the right concrete. Default: "real".

## Files done

**New headers ([st/include/](st/include/)):**

  * [engine.h](st/include/engine.h) — `ENGINE` base. All shared
    state/accessors hoisted from old `rt_eng.h`. Three pure-virtual
    hooks (`InitHardware`, `RecoverState`, `OnTimerTick`, `OnTimerEnd`).
    Static ISR machinery. `pThis` is now `ENGINE *`.
  * [rt_eng.h](st/include/rt_eng.h) — slimmed to just
    `class RT_ENGINE : public ENGINE` with the four overrides.
  * [demo_eng.h](st/include/demo_eng.h) — `class DEMO_ENGINE : public
    ENGINE`, same overrides.
  * [eng_fact.h](st/include/eng_fact.h) — declares `ENGINE
    *MakeEngine(WORD numOfClusters)`.

**New impl ([st/src/rt/](st/src/rt/)):**

  * [engine.cpp](st/src/rt/engine.cpp) — `ENGINE::ENGINE` /
    `~ENGINE`, `InstallISRs` / `UninstallISRs`, `EvalToneState`,
    `EvalPulseState`, `pThis` def.
  * [rt_eng.cpp](st/src/rt/rt_eng.cpp) — rewritten. Only
    `RT_ENGINE::` methods now: ctor (delegates to ENGINE then calls
    `InitHardware` / `RecoverState` / `InstallISRs`), dtor (relay-off
    write), and the four virtual overrides.
  * [rt_isr.cpp](st/src/rt/rt_isr.cpp) — rewritten. Now `ENGINE::`
    static ISRs. `NewISR08h` dispatches `OnTimerTick` / `OnTimerEnd`
    via `pThis`. `OldIV*` statics also `ENGINE::`.
  * [demo_eng.cpp](st/src/rt/demo_eng.cpp) — `DEMO_ENGINE` stub.
    No-op hooks. Phase 2 will fill in the Poisson generator.
  * [eng_fact.cpp](st/src/rt/eng_fact.cpp) — `MakeEngine` impl.

**Bulk-renamed (`RT_ENGINE::` → `ENGINE::`):**

  * [rt_do.cpp](st/src/rt/rt_do.cpp), [rt_util.cpp](st/src/rt/rt_util.cpp),
    [rt_store.cpp](st/src/rt/rt_store.cpp). Every method these files
    define is now part of the ENGINE base.

## To do — pick up here

1. **`CFG` field** — add `char ENGINE_KIND[8];` to
   [st/include/cfg.h](st/include/cfg.h) near
   `CELLULAR_TAX` (~line 316). **Latin-1 file** — verify with
   `file st/include/cfg.h` after editing (should still report
   `ISO-8859`).

2. **`cfg.cpp` parse + default** — in
   [st/src/cfg.cpp](st/src/cfg.cpp):
   * Add `Entry(ENGINE_KIND, ENTRY::STRING|ENTRY::LOWER|ENTRY::NO_SPACES);`
     to `FillCfgTable` (~line 488, near `VIEW_*` entries).
   * In `SetDefault` (~line 795), add:
     ```cpp
     #if defined(__DEMO__)
         strcpy(ENGINE_KIND, "demo");
     #else
         strcpy(ENGINE_KIND, "real");
     #endif
     ```
     This is the one remaining `__DEMO__` gate at the *config layer* —
     OK per the milestone framing ("`__DEMO__` may remain elsewhere").

3. **`st.ini`** — add to
   [st/util/ini2cfg/st.ini](st/util/ini2cfg/st.ini), under a new
   section header (with `;`-prefixed comment for visual grouping):
   ```ini
   ;
   ; [ Engine ]
   ;
   ENGINE_KIND=demo
   ```
   Dev environment is demo per
   [memory](~/.claude/projects/-Users-contento-Projects-contento-smarttar/memory/user_default_build_variant.md).

4. **`control.h`** —
   [st/include/control.h:63](st/include/control.h#L63): change
   `static RT_ENGINE *RTEngine;` to `static ENGINE *RTEngine;`. Add
   `#include <engine.h>` at the top (it currently includes `rt_eng.h`,
   which transitively pulls `engine.h`, but make it explicit).

5. **`control.cpp`** —
   [st/src/ctrl/control.cpp](st/src/ctrl/control.cpp):
   * Line 40: change static def type `RT_ENGINE *` → `ENGINE *`.
   * Line 89: change `new RT_ENGINE(g_cfg->CLUSTERS)` to
     `MakeEngine(g_cfg->CLUSTERS)`. Add `#include <eng_fact.h>`.

6. **`MAKEFILE`** — [st/MAKEFILE](st/MAKEFILE):
   * Add to the `OBJS` list: `$(OBJ_DIR)\engine.$(OBJ_EXT)`,
     `$(OBJ_DIR)\demo_eng.$(OBJ_EXT)`, `$(OBJ_DIR)\eng_fact.$(OBJ_EXT)`.
   * Add explicit per-`.obj` rules near the existing `rt_*` rules:
     ```make
     $(OBJ_DIR)\engine.obj:   $(SOURCE_DIR)\rt\engine.cpp
     	$(CC) $(OBJ_CC_OPTIONS) -o$@ $**
     $(OBJ_DIR)\demo_eng.obj: $(SOURCE_DIR)\rt\demo_eng.cpp
     	$(CC) $(OBJ_CC_OPTIONS) -o$@ $**
     $(OBJ_DIR)\eng_fact.obj: $(SOURCE_DIR)\rt\eng_fact.cpp
     	$(CC) $(OBJ_CC_OPTIONS) -o$@ $**
     ```
   * Borland MAKE 3.6 needs explicit rules — `.PATH.cpp` does NOT
     work despite the existing lines suggesting otherwise (see
     CLAUDE.md "Build System" note).

7. **Strip `#ifdef __DEMO__` from RT layer** — partly done already:
   the old gates in `rt_eng.cpp` (cluster probe, `Available` init,
   NOPHONE init, STM2 recovery) and `rt_isr.cpp` (port-read skip)
   moved into the new virtual overrides. One `__DEMO__` remains in
   [rt_eng.cpp:14-17](st/src/rt/rt_eng.cpp#L14) (STM2 include) and
   in [rt_eng.cpp's `RecoverState` body](st/src/rt/rt_eng.cpp) —
   that's OK; STM2 isn't an engine concern proper, just a persistent
   store that production needs and demo doesn't. Leave alone unless
   removing in a separate cleanup pass.

8. **Build + verify** — `./make-headless.sh demo` from repo root.
   Expect compile errors on first try; common gotchas:
   * `RT_ENGINE::ONHOOK` references in `ctrl_ev.cpp`/`ctrl_rf.cpp`
     should still resolve via inheritance (enums are on `ENGINE`,
     `RT_ENGINE` inherits) — if not, add `using ENGINE::ONHOOK;` etc.
     in `RT_ENGINE` body, or change callsites to `ENGINE::ONHOOK`.
   * Borland 3.1 doesn't have `override` keyword — already not using it.
   * `pThis` is now `ENGINE *`; references in renamed files
     (`rt_do.cpp`/`rt_util.cpp`/`rt_store.cpp`) should still work
     since they don't qualify it.
   * `RT_ENGINE::OldIV*` references — none expected; all moved.
   * `g_STM2` reference in `rt_eng.cpp::RecoverState` — guarded by
     `!__TEST__ && !__DEMO__` so production builds find the extern.
     DEMO_ENGINE never calls into STM2.

9. **Verify no behavior change in demo build:** Launch (`./run-headless.sh`)
   and confirm the UI comes up, booths show idle (NOPHONE) like before.

## Open design notes (for later phases)

* Dtor ordering: original `RT_ENGINE::~RT_ENGINE` uninstalled ISRs
  FIRST, then did the relay-off port write. After refactor, derived
  dtor (relay-off) runs BEFORE base dtor (ISR uninstall). Means the
  ISR fires for a few ticks with `GP_CASH` already cleared. Should be
  harmless. Flag if anything misbehaves.
* Demo build still writes to ports in current code (no `__DEMO__`
  gate around the port-clear loop or `PO_GENERAL` write). After the
  refactor, `DEMO_ENGINE` does ZERO port I/O. Cleaner, but a
  behavior change for the corner case of "demo binary with
  `ENGINE_KIND=real`". Not user-visible.
* Phase 2 (Poisson generator) will need an ISR-safe RNG (Borland
  C++ 3.1's `rand()` is not reentrant). Pre-allocate state in
  `InitHardware`; touch nothing on the heap from `OnTimerTick`.

## Recovery handle

`pre-simula-trash` tag at `e64284a` is on `main` as a rollback point
for the whole DEMO_ENGINE / SIMULA cleanup arc. Hard-revert from there
if needed.
