# SmartTar — Handoff

Status snapshot for resuming on another machine.
Branch: `main` — **4 commits ahead of `origin/main`, unpushed** (the two
stability batches below). Push is gated on GCC's confirmation.

---

## Active thread: Stability audit

Working through the CRITICALs in [STABILITY_AUDIT.md](STABILITY_AUDIT.md)
on `main`, one small verified-then-fixed batch at a time. That doc is the
source of truth for per-finding status; summary here:

- **Fixed & committed:** C1–C13, C16–C18, C20–C22.
  - C1–C4 (quick wins), C5–C9 (`58e68d5`), C11–C22 batch (`f497445`).
- **C10:** verified as *defended* — `Add()` already checks both writes; the
  non-atomic data-vs-index window is a design limitation, not a bug. No code
  change.
- **Open CRITICALs:**
  - **C14** `control.cpp:541-548` — `NewGPFHandler` calls `delete` on the
    engine/db globals *inside* the #0Dh GPF ISR; `delete` re-enters
    PHAPI/heap, so the recovery path can itself deadlock.
  - **C15** `control.cpp:697-717` — OOM `new_handler` tears down some globals
    but leaves `g_dbEngine`/`g_phEngine`/`g_cfg`/`g_spooler` — partial
    teardown leaks files and leaves the DB inconsistent.
  - **C19** `spooler.cpp:23,33` — `Buffers` is a static class array
    reallocated on every `SPOOLER` ctor; a second instantiation leaks the
    prior queues and stomps the statics.
  - Plus all HIGH findings (audit § "HIGH").

**Next step: C14, then C15, then C19.** C14/C15 are riskier than every batch
so far — they touch the crash/OOM recovery paths, so the fix has to avoid
heap re-entry, not just add a null check. Verify each cited file:line before
touching it (audit § 7 step 2).

---

## Completed & merged: DEMO_ENGINE milestone

The runtime engine-selection arc is **complete and merged into `main`**; the
`demo-engine` branch no longer exists. What shipped:

- Template-Method `ENGINE` base with `RT_ENGINE` / `DEMO_ENGINE` subclasses
  and a `MakeEngine` factory driven by `g_cfg->ENGINE_KIND` (`st.ini`).
- `DEMO_ENGINE` Poisson call generator + real-number `phones.csv` dataset, so
  demo booths resolve to named destinations instead of "-- No Incluida --".
- Build pipeline repair (`inf2dat`/`ini2cfg` sub-makefiles bootstrap), scripts
  renamed `make-headless`→`build` and `run-headless`→`run` (`.sh` + `.ps1`).
- Demo quit-confirmation (`st.cpp::Exit()`) and operator start/stop menu.

Architecture lives in the code (`st/src/rt/engine.cpp`, `rt_eng.cpp`,
`demo_eng.cpp`, `eng_fact.cpp`) and is summarized in `README` /
`GRAPH_REPORT.md`. `pre-simula-trash` tag at `e64284a` is the rollback point
if the whole arc ever needs discarding.

### Deferred remnant: the last `__DEMO__` gates

Down from 58 references to **5 functional gates**: `st.cpp` lines 16 / 116 /
158 / 192, plus the `cfg.cpp` default-selector (a build-time hint for
`makedemo`, intended to stay). The st.cpp four interleave the `__NO_DONGLE__`
build flag with `DONGLE` class scope around the dongle/STM2/EEPROM init
tangle. To finish: hoist `DONGLE dongle;` out of the `if/else` so both the
initial check and the event-loop re-check can reach it, then convert the
outer `__DEMO__` to a runtime `g_cfg->IsDemoMode()` check. Not blocking
anything — engine selection is already runtime.

---

## Notes / known issues (still apply)

- **Latin-1 file editing**: UTF-8-native editors (incl. the `Edit` tool)
  re-encode Latin-1/CP850 `.cpp`/`.h` files to UTF-8 on save, corrupting every
  high-bit char. After any such edit, verify with `file <path>` — it must
  report `ISO-8859` or `Non-ISO extended-ASCII`, never `UTF-8`. Repair
  byte-level (see CLAUDE.md). Host-side files like this one are LF/UTF-8 and
  safe.
- **Engine dtor ordering**: derived `~DEMO_ENGINE`/`~RT_ENGINE` (relay-off
  port write) runs before base `~ENGINE` (ISR uninstall), so the ISR fires for
  a few ticks with `GP_CASH` already cleared. Harmless in practice; flag if
  anything misbehaves at shutdown.
