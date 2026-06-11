# Mini-SmartTar — Session Handoff

Last pushed: `3470779`. Branch: `mini-smarttar`.

## Current State

**All phases complete.** The build produces `st/bin/st.exe` (1.1 MB, demo_dos variant). `./build.sh --force` completes in ~40s. `./run.sh` launches SmartTar inside DOSBox-X.

| Phase | Status |
|-------|--------|
| P0 – Baseline | DONE |
| 1.1 – Remove 15 utils | DONE |
| 1.2 – core/demo_dos/real_dos split | DONE |
| 1.3 – Null-object hw mocks | DONE |
| 1.4 – Config from ST.INI | DONE |
| 1.4b – inf2dat eliminated | DONE |
| fix – UI regression (`__BTN__`) | DONE (`29e59ab`) |
| 1.5 – Two build variants | DONE |
| 2.1a – BinStorage behind interface | DONE |
| 2.1b – CsvStorage, default csv | DONE |
| 2.2 – PORTABILITY.md seam catalogue | DONE |

## Infrastructure Notes

- **`-D__BTN__` MUST NEVER be removed from `st.cfg`** — breaks toolbar/grid layout.
- **Real engine (`real_dos/`) requires physical PC cards** (telephony boards, EEPROM, dongle, STM2). Only `demo_dos` runs.
- macOS DOSBox-X quirks documented in CLAUDE.md §macOS vs Windows.

## Useful Commands

```sh
./build.sh --force         # full rebuild (~40s)
./run.sh                   # launch SmartTar
./run.sh --keep-open       # launch, keep DOS prompt after exit
git diff main..HEAD        # all changes on this branch
```
