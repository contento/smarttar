# SmartTar — Handoff

Status snapshot for resuming on another machine.
Branch: `main` — in sync with `origin/main` except the latest stability
commit (`038f489`) + this doc/audit update, which are **unpushed pending
GCC's confirmation**. Push is gated on GCC's confirmation.

---

## Closed: Stability audit (substantially complete)

The stability milestone is **CLOSED — substantially complete** (audit § 8).
[STABILITY_AUDIT.md](STABILITY_AUDIT.md) is now a historical record.

- **All CRITICALs (C1–C22) resolved.** `58e68d5` (C5–C9), `f497445` (C11–C22),
  `d4bbd49` (C19), `d7d24ca` (C14/C15), plus C1–C4 quick wins. **C10** is
  verified-*defended* (non-atomic data-vs-index window is a design limitation).
- **HIGH / MEDIUM (hardware-independent) done.** Final batch `038f489`:
  spooler `strlen` deferral, st.cpp OOM NULL-guard, eeprom off-by-one,
  w_table use-after-free (NULL after delete), mutex invariant doc. Plus the
  earlier `aeb1372` / `ad15670` / `082c188` / `fd0e188` batches. Several
  findings closed DEFENDED/WONTFIX with rationale recorded inline in the audit.
- **Tier 3 deferred (ISR / real-time concurrency).** Parked in
  [ISR_VOLATILE_NOTES.md](wiki/dev/ISR_VOLATILE_NOTES.md) — needs a DOSBox-X
  build + load-test loop (ideally real hardware). **Guard-rail in force:** do
  NOT enable compiler optimization (`-O`/`-Z`/`-G`/`-Or`) without first doing
  the `volatile`/atomicity audit in that note (the code relies on the
  no-optimization compiler reloading `Clusters[]` from memory).
- Crash-path caveats still worth remembering:
  - **C14** bounds the GPF-handler heap re-entry rather than eliminating it;
    fully avoiding `delete` from a fault context needs an `ENGINE` redesign —
    deferred, out of scope.
  - **C16** left `States[i].Bitmap` unfreed on purpose — `States` is `static`,
    one app-lifetime allocation.

**Next step: none for stability.** Resume Tier 3 only from ISR_VOLATILE_NOTES
when a load-test environment is available.

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
