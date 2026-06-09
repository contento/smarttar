# SmartTar — Handoff

Status snapshot for resuming on another machine.
Branch: `main` — at `b405280`, **pushed** to `origin/main`. Latest work is
the **v2.70.0** release (Zinc booth-grid geometry fix). Tag `v2.70.0` is
pushed but the **GitHub release did not publish** — the CI EXE build is
blocked (see "BLOCKED" below).

---

## BLOCKED: v2.70.0 release-artifact CI (DOSBox-X crash)

The v2.70.0 **code is fully released in git** (merged to `main`, tag
`v2.70.0` → `b405280`). What is **not** done is the downloadable `.exe`
artifact: `.github/workflows/release.yml` fails on every run.

**Root issue (diagnosed, not fixed):** DOSBox-X crashes ~40-50 s into the
build on the `windows-latest` runner, mid-compile. It is **not** a timeout
(15-min limit, dies at ~41 s) and **not** a source/script bug — the
identical build runs to completion on macOS DOSBox-X and produces
`bin/st.exe`. Same DOSBox-X version (`2026.05.02`) / runner / timeout as the
last *successful* release (v2.50.0, ~1m35s), so DOSBox-X *can* run for
minutes here. The current build is the first CI attempt since the
demo-engine era (v2.60.0 was never released); the new element is **running
the Pharlap-286 helper apps** (`ini2cfg`, `inf2dat`) before the main
compile — the leading theory is that running those destabilizes DOSBox-X,
which then crashes a few operations into `make`.

**Progress made (each fix peeled back a layer; all pushed):**
- `5b12bd9` — helper-build bootstrap in `mk_cfg.bat`/`mk_ph.bat`; fixed a
  latent compile bug where `"C\xF3digo"` parsed `\xF3d` as one hex escape
  (0xF3D → "Numeric constant too large"), split via `"C\xF3" "digo"` in
  `util_cfg.h` + `defpwd.cpp`; host-side creation of `build/`, `bin/`,
  `util/*/obj/` (gitignored, absent on fresh checkout) in `build.sh`/`.ps1`.
- `1b6f326` / `b405280` — committed the prebuilt Pharlap-bound helper exes
  (`INI2CFG.EXE`, `INF2DAT.EXE`, with `.gitignore` negations under their
  real 8.3 uppercase names). CI now **runs** the helpers successfully
  (`Listo.`) and reaches the main `make` — then DOSBox-X crashes.

**Next steps (need the Windows env — not reproducible on macOS):**
1. Run `.\build.ps1 demo` locally on Windows — does DOSBox-X crash ~40 s in?
   Confirms it's DOSBox-X, not CI.
2. Read DOSBox-X stderr for the crash cause: the workflow already sets
   `MAKE_HEADLESS_DEBUG=1` (tees to `dosbox-x-build.log`) and uploads a
   `build-failure-<run_id>` artifact.
3. Try splitting the build into separate DOSBox-X launches per phase
   (`mk_cfg` / `mk_ph` / `make`) so a crash in one phase doesn't kill the
   run. (The long `make` alone may still exceed the crash window.)
4. Fallback: run the helpers once and also commit their outputs
   (`st.cfg`, `ph_info.dat`) so CI's DOSBox-X only does the main compile.

To re-trigger CI after a fix: commit, then `git tag -f -a v2.70.0 ...` and
`git push --force origin v2.70.0` (plain `vX.Y.Z` tags fire `release.yml`).

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

## Other open items (from TODO.md)

- **DEMO_ENGINE Phase 3** (partially done):
  - Graceful stop (drain, not freeze) — `[~]` in TODO
  - Graceful drain on exit — `[~]` in TODO
  - Total simulation time limit — `[~]` in TODO
- **Documentation wiki** — substantially built at `wiki/`; remaining:
  - Vault layout decision
  - `.docx` manual conversion
  - README simplification
- **Toolchain portability** — not started (Investigate Open Watcom / DJGPP)
