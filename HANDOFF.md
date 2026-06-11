# Mini-SmartTar — Session Handoff

Pushed to `origin/mini-smarttar` (`decaade`).

## Current State

The build produces `st/bin/st.exe` (1.1 MB, demo_dos variant). `./build.sh --force` completes in ~28s. `./run.sh` launches SmartTar inside DOSBox-X.

## The Problem: UI Regression

The toolbar is misplaced and grid geometry is wrong on mini-smarttar. **Main branch works fine.**

Every Zinc-related source file is identical between branches. RES.DAT, Zinc libraries, UI link order — all the same. The 10 KB larger binary (new objects: `csv_stor`, `nullstm2`, `stm2fact`) is the only structural difference.

**Hypothesis**: the MAKEFILE define changes (`-DDEMO_DOS` replacing `-D__DEMO__;-D__NO_DONGLE__`) and cfg.cpp refactoring (zero config init the first time → no ST.CFG → reads ST.INI, but `STORAGE`/`AUTO_SIMULATE` fields grew the struct) introduced subtle behavioral differences. A stale `ST.CFG` from main with the old struct layout corrupts runtime config when loaded.

**What was tried:**
- Deleting stale `st/bin/ST.CFG` — didn't fix it
- Comparing every source diff — all Zinc files untouched
- Comparing MAP files — 15-line difference, minor segment changes
- Comparing build options — same libraries, same link order

## Infrastructure Changes Made

| File | Change |
|------|--------|
| `dosbox-x.conf` | Mount path: removed `C:/` prefix (invalid on macOS) |
| `build.sh` | Replaced `eval` quoting hell with `cmd+=()` array; uses conf autoexec; bat writes status to `C:\build.log` directly |
| `run.sh` | Simplified to use conf autoexec |
| `st/mkdemos.bat` | New build bat for demo_dos variant; writes status to `C:\build.log` |
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

1. **Fix the UI regression first** — identify what define/struct/config change on mini-smarttar breaks Zinc layout. Hypothesis: cfg.cpp changes or missing `__DEMO__` define.
2. **Then continue with Phase 1.5/2.1a/2.1b/2.2** — the remaining mini-smarttar features.

## Useful Commands

```sh
./build.sh --force         # full rebuild (28s)
./run.sh                   # launch SmartTar
./run.sh --keep-open       # launch, keep DOS prompt after exit
git diff main..HEAD        # all changes on this branch
```
