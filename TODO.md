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
- [ ] **Theme switching** — Zinc 3.5 has a built-in palette / "scheme"
      mechanism. Expose it as a runtime toggle (config option +
      menu-bar entry) so the operator can switch between palettes
      without recompiling.
- [ ] **Improve SIMULA mode** — the existing simulation mode
      (`-DAUTO` / `makeauto`) is rough. Better simulation algorithm
      (realistic call distribution, configurable rates), clearer
      presentation, and a parameters block so an operator can drive
      it (number of booths, call frequency, tariff mix, etc.) without
      code changes.

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
      Fresh clones rely on `HELP=1` (passed by `make-headless.sh / .ps1`
      unconditionally) to regenerate it before first link.
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
