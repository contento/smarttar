# Mini-SmartTar — Session Handoff

Pushed to `origin/mini-smarttar` (`decaade`).

## Current State

The build produces `st/bin/st.exe` (1.1 MB, demo_dos variant). `./build.sh --force` completes in ~40s. `./run.sh` launches SmartTar inside DOSBox-X.

## The Problem: UI Regression (FIXED)

The toolbar was misplaced and grid geometry was wrong on mini-smarttar. **Root cause found and fixed.**

`st/st.cfg` was missing `-D__BTN__`. On main it was `-D__FHEADER=3;__DEBUG=0;__BTN__`; the mini-smarttar refactoring stripped it to `-D__FHEADER=3`.

`b_button.h` uses `#ifdef __BTN__` to define custom `UIW_TBUTTON` and `UIW_GBUTTON` classes that apply a height-offset trick (`top + height` then `relative.top -= height`) forcing Zinc to allocate correct vertical space for toolbar and grid buttons. Without `__BTN__`, they collapse to plain `#define UIW_TBUTTON UIW_BUTTON` — no offset, broken layout.

**Fix:** `st/st.cfg` line 3: `-D__FHEADER=3` → `-D__FHEADER=3;__BTN__`

**Why it was missed:** The MAKEFILE defines (`-DDEMO_DOS` replacing `-D__DEMO__;-D__NO_DONGLE__`) and cfg.cpp refactoring were the suspected culprits, but the actual bug was in the BCC config file (`st.cfg`), not the MAKEFILE. `st.cfg` was simplified during the mini-smarttar work and `__BTN__` was accidentally dropped.

## Infrastructure Changes Made

| File | Change |
|------|--------|
| `dosbox-x.conf` | Mount path: removed `C:/` prefix (invalid on macOS) |
| `build.sh` | Replaced `eval` quoting hell with `cmd+=()` array; uses conf autoexec; bat writes status to `C:\build.log` directly |
| `run.sh` | Simplified to use conf autoexec |
| `st/mkdemos.bat` | New build bat for demo_dos variant; writes status to `C:\build.log` |
| `st/st.cfg` | Restored `-D__BTN__` define (UI regression fix) |
| `CLAUDE.md` | Added macOS vs Windows DOSBox-X behavioral differences section |

## macOS DOSBox-X Quirks (documented in CLAUDE.md)

1. **Mount path**: macOS needs `/Users/...`, not `C:/Users/...`
2. **`-exit` flag**: can skip `-c` commands; use `-c "exit"` instead
3. **Phar Lap stdout redirect**: `>`/`>>` cannot capture output from Phar Lap bound EXEs (bcc286, bind286, cfig286) on macOS ARM. Output only goes to the DOSBox-X window.
4. **`.bat` output redirect**: `-c "script.bat > file"` writes nothing. The bat must `echo > file` internally.
5. **`command /c`**: needs `z:\` in PATH (with `-noautoexec`), but `command /c` creates a new shell that breaks build path resolution on mini-smarttar.

## New Approach for Next Session: Do → Validate GUI → Repeat

The failure mode was making multiple changes without verifying the UI between them. Next time:

1. **Make ONE change** (add a feature, refactor a file, change a define)
2. **Build** (`./build.sh --force`)
3. **Launch and validate the GUI** (`./run.sh`)
4. Only then make the next change

Tiny, verifiable steps. If GUI breaks, you know exactly which step caused it.

## Priorities

1. **Fix the UI regression** — ✅ DONE. Missing `-D__BTN__` in `st/st.cfg`.
2. **Continue with Phase 1.5/2.1a/2.1b/2.2** — the remaining mini-smarttar features.

## Useful Commands

```sh
./build.sh --force         # full rebuild (~40s)
./run.sh                   # launch SmartTar
./run.sh --keep-open       # launch, keep DOS prompt after exit
git diff main..HEAD        # all changes on this branch
```
