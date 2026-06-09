# SmartTar — Handoff

Status snapshot for resuming on another machine.
Branch: `main` — at HEAD. v2.70.0 release CI no longer blocked.

---

## v2.70.0 CI status

**Previously blocked:** DOSBox-X `2026.05.02` (mingw64) crashed mid-compile
on the `windows-latest` runner after ~40 s. **Fix applied:** upgraded DOSBox-X
to `2026.06.02` (the version that builds locally). Also added `st/lib` to
the output directory creation in `build.ps1` (required by the DLL `.lib`
move rule).

Tag `v2.70.0` currently points to `5b12bd9` (pre-fix). To re-trigger CI on the
fixed code: `git tag -f -a v2.70.0 -m "v2.70.0" && git push --force origin v2.70.0`.
The `v[0-9]+.[0-9]+.[0-9]+` tag pattern fires `release.yml`.

If `2026.06.02` also crashes, fallback tactics:
1. Split the build into separate DOSBox-X launches per phase (`mk_cfg` /
   `mk_ph` / `make`).
2. Commit `st.cfg` and `ph_info.dat` so DOSBox-X only does the main compile.

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

## Completed & merged: DEMO_ENGINE milestone (all phases)

The runtime engine-selection arc is **complete and merged into `main`**; the
`demo-engine` branch no longer exists. What shipped:

- **Phases 1--2:** Template-Method `ENGINE` base with `RT_ENGINE` /
  `DEMO_ENGINE` subclasses; `MakeEngine` factory; Poisson call generator +
  real-number `phones.csv` dataset.
- **Phase 3 — operator controls, graceful drain, time limit:**
  - `TogglePaused()` with graceful drain (`_draining`/`DrainTick`) — stops new
    arrivals, drives active calls to settlement, latches `_paused` when idle.
  - `ForceStoreActiveCalls()` enqueues receipts for TALK calls before exit.
  - `total_minutes` cap in `demo.ini` (0 = unlimited); triggers the same drain.
- Build pipeline repair (`inf2dat`/`ini2cfg` sub-makefiles bootstrap), scripts
  renamed `make-headless` → `build` and `run-headless` → `run` (`.sh` + `.ps1`).
- Demo quit-confirmation (`st.cpp::Exit()`) and operator start/stop menu.

Architecture lives in the code (`st/src/rt/engine.cpp`, `rt_eng.cpp`,
`demo_eng.cpp`, `eng_fact.cpp`) and is summarized in `README` /
`GRAPH_REPORT.md`. `pre-simula-trash` tag at `e64284a` is the rollback point
if the whole arc ever needs discarding.

### Remnant: the last `__DEMO__` gates

Down from 58 references to **5 functional gates**: `st.cpp` lines 16 / 117 /
159 / 193, plus the `cfg.cpp` default-selector (intentional). Interleave
`__NO_DONGLE__` with `DONGLE` class scope. Not blocking — engine selection is
already runtime.

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

---

## Fixed: Zinc Grid row-tear (branch `fix/zinc-grid-geometry`)

The Zinc 3.5 grid bug (screenshot `wiki/assets/zinc-gui-bug.png`,
commit `351eda4`) is **diagnosed and fixed** on `fix/zinc-grid-geometry`.

- **Root cause (measured, not theorized):** stock Zinc 3.5
  `RegionConvert` (`vendor/zinc/SOURCE/Z_WIN2.CPP:390-395`) floors each
  booth row's pixel top *independently* of the row height. With the
  runtime stride `GBUTTON_HEIGHT(8) * miniNY(1) * cellHeight(24) /
  miniDY(10) = 19.2 px`, the 0.2 px/row fraction accumulates to a whole
  pixel every 5 rows, so the pitch jumps 19->20 at booths 6/11/16 and the
  grid tears there (the screenshot's 5/6 boundary). Confirmed byte-for-byte
  that this geometry code was *never* patched (`source` == `osource`); the
  only historical Zinc patches were `D_BUTTON.CPP` + Spanish localization.
  Zinc fixed it in v4 (different geometry engine) — out of reach on 3.5.
- **Same drift on the X axis + bottom border.** Columns are positioned with
  a 1-minicell overlap to share borders, but the same independent floor
  (0.7 px/minicell) opened 1 px gaps at Tel|Loc and Tar|Val -> two adjacent
  border lines = thick separators. And removing the Y drift shortened the
  grid, detaching the table's bottom border from the last row.
- **Fix (app-side, pixel-space, no Zinc rebuild).** In `st/src/ui/view_ev.cpp`
  / `st/include/view.h`:
  - `NormalizeRowPitch()` snaps rows to the uniform integer pitch from the
    driftless first gap (preserving each cell's height) and pulls the table
    bottom up to the last row.
  - `NormalizeColumnBorders()` extends the left column's right edge at any
    gapped boundary so every separator is one shared line.
  - Both run from `UIW_VIEW::Event` on `S_CREATE`/`S_SIZE` -- *before* the
    first paint. (An earlier approach ran them post-paint via
    `CONTROLLER::RefreshView` + `S_REDISPLAY`, which left white repaint
    notches at the Est|Are boundary; fixing geometry before the first paint
    removed those.)
- **Verified:** a temporary `GEO_DEBUG` region-dump probe (now removed)
  measured pixel geometry at each step -- post-fix every booth sits at
  `122 + 19*n` (uniform 19 px), all column overlaps are 0, and the table
  bottom is flush. Confirmed visually in DOSBox-X
  (`wiki/assets/zinc-gui-bug-03.png`); screenshots `-00`..`-03` document the
  before/after progression.

**Next step:** decide on merge to `main` (branch is unpushed).

---

## Other open items (from TODO.md)

- **Documentation wiki** — substantially built at `wiki/`; remaining:
  - Vault layout decision
  - `.docx` manual conversion
  - README simplification
- **Toolchain portability** — not started (Investigate Open Watcom / DJGPP)
- **SVGA display** — Zinc 3.5 supports higher modes via BGI; investigate
  `machine = svga_s3` + font rework for 800×600+.
