# SmartTar Stability Audit

**Date:** 2026-05-16
**Scope:** All `.cpp` / `.c` / `.h` under `st/src/` and `st/include/` (129 files, ~38k LOC).
**Goal:** Identify stability-relevant issues per project mandate ("no new features — stability only"). **Findings only — no edits applied.**
**Method:** Four parallel agents covering disjoint subsystems + mechanical greps. Spot-verification on highest-impact findings (results in §6).

---

## 1. How to resume from another machine

```sh
git pull
# Then in Claude Code, in the project directory:
```

Paste this prompt to Claude:

> Continue the stability audit from `STABILITY_AUDIT.md`. Status: findings from four subsystem agents are recorded; 5 of the CRITICAL items were spot-verified (see §6). The remaining unverified CRITICAL/HIGH findings still need source-level confirmation before any fixes. Next step is your choice: (a) finish verifying the remaining CRITICAL findings, (b) start fixing the **confirmed** CRITICALs in §6, or (c) prioritize differently. Confirm approach before broad changes (per CLAUDE.md Working Style).

The four sub-agent reports are summarized in §3–§5 below. They were one-shot — re-running them would produce different prose but similar findings.

---

## 2. Mechanical scan baseline

| Signal | Count | Note |
|---|---|---|
| `goto` | 0 | clean |
| Inline `asm` | 0 | clean |
| `gets`/`scanf` (highest risk) | 0 | clean |
| Unsafe stdlib calls (`strcpy`/`sprintf`/`strcat`) | 91 | concentrated: `bdisplay.cpp`(11), `prn_fmt.cpp`(11), `ctrl_rf.cpp`(10), `dstorage.cpp`(9), `w_statbr.cpp`(5), `dstatist.cpp`(5) |
| TODO/FIXME/HACK markers | 2 | `w_statbr.cpp:469,504` ("BUG.SUN") |
| `new` without matching `delete` (per-file imbalance) | 8 files | `view_ev.cpp` (47/0) — likely Zinc parent-owns pattern; `mb_print.cpp` (6/0), `tb_man.cpp` (9/2), `ph_query.cpp` (2/0), `ctrl_rf.cpp` (2/0), `rt_util.cpp` (1/0), `plain_pr.cpp` (1/0), `dstorage.cpp` (5/3) — worth case-by-case review |
| Functions > 200 lines | 7 | longest: `CONTROLLER::Event` at `ctrl_ev.cpp:23` (**765 lines**), `printStatData` at `ct_pr_st.cpp:19` (537 lines), `DB_STATISTICS::Add` at `dstatist.cpp:191` (490 lines), `DB_STATISTICS::Subtract` at `dstatist.cpp:683` (453 lines) |
| Largest files | `mb_ext.cpp` (1482), `mb_conf.cpp` (1377), `dstatist.cpp` (1273), `cfg.cpp` (1153), `ctrl_ev.cpp` (949) |

---

## 3. Findings by severity

Format: `SEVERITY | file:line | finding | why it matters`. **V** = spot-verified (§6).

### CRITICAL

| # | File:line | Finding | V |
|---|---|---|---|
| C1 | `src/ctrl/ctrl_ev.cpp:488` | UE_RNPP `while (it)` never advances iterator — preceding UE_RPP case at line 469 has `it++;`, this one omits it. Hangs the UI when "Pagar recibos por cabina" is selected. | ✅ |
| C2 | `src/rt/serial.cpp:191` | Ring-buffer overflow check has empty body (`;`) — writes at line 193 unconditional, overwrites `BufStart`, corrupts queue. Also requires `BufLen` power-of-two (precedence: `BufLen-1` binds tighter than `&`, works by accident). | ✅ |
| C3 | `src/dongle.cpp:34` | `!biosprint(...) & BIOS_PRINT_BUSY` — `!` binds before `&`; guard is `(!val) & 0x80` = always 0. Dongle-busy short-circuit is dead code. | ✅ |
| C4 | `src/ct/ct_util.cpp:30` | `"\x1B\x70\x00\x0A\0x0A\xFF"` — `\0x0A` is `\0` then literal `x0A`. Cash-drawer command sequence is malformed (9 bytes, not 6). | ✅ |
| C5 | `src/rt/rt_eng.cpp:123` | `OldIV09h = getvect(0x09)` inside `if (CHECK_PAUSE_KEY)` but `setvect(0x09, NewISR09h)` unconditional — destructor restores conditionally, leaks vector 09h. | — |
| C6 | `src/rt/rt_eng.cpp:125` | Missing semicolon after `setvect(0x09, ...)` in non-DOSX286 branch (dead/untested code path). | — |
| C7 | `src/db/dstorage.cpp:69,97` | `creat()` return values not checked — disk-full / permission denial leaves `DataFile`/`IndexFile` at `-1`; later writes go to fd `-1`, `Get/Add` silently corrupt. | — |
| C8 | `src/db/dstorage.cpp:262` | `RepairDataFile` ignores `write()` returns — partial writes during repair silently truncate; tmp file then renamed over real DataFile (line 282), destroying recoverable receipts. | — |
| C9 | `src/db/dstorage.cpp:343` | `RepairIndexFile` ignores `write()` return — index/`NumOfEntries` counter desync, persisted at 362. | — |
| C10 | `src/db/dstorage.cpp:433-447` | `Add()`: if `WriteIndexHeader` fails after entry written, data has the receipt but index header is stale; on next open, count rebuild differs. Non-atomic. | — |
| C11 | `src/db/dstatist.cpp:82-86` | Four `write()` calls in ctor unchecked — disk-full leaves stats file header-less / truncated; in-memory copy looks fine until reopen. | — |
| C12 | `src/ph/ph_place.cpp:589-594` | `GetNumbers` writes `numbers[slot].Numbers[Count++]` with no bounds vs `MAX_NUMBERS_PER_LINE` — malformed `.inf` line overflows embedded array. | — |
| C13 | `src/ph/ph_place.cpp:201-202` | `Search()`: `partialPhone[i]` indexed up to `len-1` where `len = strlen(phone)`; PHONE is 17 bytes; 16-char phone + terminator overruns by 1. | — |
| C14 | `src/ctrl/control.cpp:541-548` | `NewGPFHandler` calls `delete RTEngine/g_dbEngine/...` inside the #0Dh GPF ISR — `delete` re-enters PHAPI/heap; recovery path itself can deadlock. | — |
| C15 | `src/ctrl/control.cpp:697-717` | OOM `new_handler` deletes some globals but not `g_dbEngine`, `g_phEngine`, `g_cfg`, `g_spooler` — partial teardown leaks files, leaves DB inconsistent. | — |
| C16 | `src/ui/view.cpp:70-73` | `~UIW_VIEW` only deletes `m_callInfo`; never deletes the 2-D widget arrays `WBoothNumbers`/`WStates`/`WAreas`/`WPhones`/`WCities`/`WElapsedTimes`/`WTariffs`/`WValues`/`WNumOfCalls`, `ToneFSs`/`PulseFSs`/`States[i].Bitmap`. Large leak; likely also dangling pointers. *(Note: Zinc widgets owned by parent — but the **arrays of pointers** themselves still need `delete[]`.)* | — |
| C17 | `src/tb/tb_man.cpp:80-83,127` | `UIW_MANUAL` allocates `s_receipts`/`s_totals`/`s_wNCs`/`s_wPRs`/`s_wReceipts` as statics; dtor only frees first two. Re-opening dialog overwrites pointers → leak + cross-instance aliasing. | — |
| C18 | `src/spooler.cpp:30-33` | `NumOfChannels` clamped but `Print(channel, ...)` never validates the channel arg against `MAX_SPOOL_CHANNELS=4`; caller-supplied out-of-range channel indexes uninitialized `Buffers[]` pointer. | — |
| C19 | `src/spooler.cpp:23,33` | `Buffers` is a static class array reallocated on every `SPOOLER` ctor — second instantiation leaks prior queues and stomps statics. | — |
| C20 | `src/log.cpp:43-58` | After CREATE-branch reopen failure, `openOk=TRUE` but stream may be bad; subsequent `put()` silently loses log entries. | — |
| C21 | `src/log.cpp:63` | Failed-open branch deletes `file` but does not null it — `~Log()` deletes again (double-free). | — |
| C22 | `src/cfg.cpp:592-595` | Buffer-overrun check `if (offset > MAX_ID_VALUES)` runs **after** `FillCfgTable()` already wrote OOB. `MAX_ID_VALUES=0x100` must be raised any time entries are added. | — |

### HIGH

ISR / real-time (`rt/`):
- `src/rt/serial.cpp:46-53` — All `SERIAL` state is `static`; constructing a second instance clobbers ISR vector + buffer pointer, leaks prior `Buffer`.
- `src/rt/serial.cpp:87` — `inportb(IMR) | ~((Port==COM1)?IRQ4:IRQ3)` — `~` on promoted int produces `0xFFxx`; can mask off unrelated IRQs (timer, keyboard).
- `src/rt/serial.cpp:198` — Explicit `enable()` in ISR exit re-enables IF even if caller had IF=0; lets nested interrupts fire before IRET.
- `src/rt/rt_isr.cpp:28-34` — Reentrancy guard: on detected reentry, EOI issued but chain to `OldIV08h` skipped → BIOS tick (40:6Ch counter, disk motor) is suppressed.
- `src/rt/rt_isr.cpp:25-43` — ISR locals declared `static` — function-scope statics are not reentry-safe; if reentry slips past the guard window, outer state corrupts.
- `src/rt/rt_isr.cpp:91-100` — Carry-flag check `if (_FLAGS & 0x01)` to chain old IV08h depends on compiler not reordering; fragile.
- `include/rt_eng.h:111-212` — **No `volatile`** on any ISR/main-shared state (`Clusters`, `GeneralPort`, `SpyBooth`, `CurrentDate`, `CurrentTime`, booth error flags). With `-O`, reads may be cached in registers and miss ISR updates.
- `src/rt/rt_util.cpp:285-303` — `ElapsedCounts`/`FinalElapsedCounts` are `DWORD` (32-bit) read/written non-atomically in 16-bit code — torn reads possible.
- `src/rt/rt_store.cpp:13-123` — `StoreReceipt` called from ISR path, calls `strcpy`/`memcpy` (C runtime, not reentrant) and takes `RTReceiptQueueMutex` — taking a mutex inside a hardware ISR risks deadlock with main thread.
- `src/rt/rt_util.cpp:533` — `GetClusters` does `memcpy` under `RTBoothClustersMutex` but ISR writes `Clusters[]` without the mutex (one-way protection) — main can read mid-update.

DB / telephony:
- `src/ph/ph_place.cpp:374` — `GetPlace` strcat into `tmpPlace[512]` checks size before cat but doesn't track remaining space; long colon-less `.inf` lines can overflow.
- `src/ph/ph_query.cpp:240-241` — `strncpy(szPhone+nAccess, pszToken, nLen)` then null-term; if `nLen+nAccess >= sizeof(PHONE)=17`, both the strncpy and terminator overrun.
- `src/ph/ph_query.cpp:95-101` — `while (pszCP[n] != ':')` no bounds on `n` vs `CITY_NAME=21`; missing `:` walks until fault.
- `src/ph/parser.cpp:21` — `strlen(line)` O(n²) per loop iter; long `.inf` line >512 overflows `tmpLine[512]` via `tmpLine[iChar++]`.
- `src/ph/ph_place.cpp:259` — `rightNumber = numbers[i+1]` when leftNumber has RANGE flag, no check that `i+1 < numCount`.
- `src/db/dstorage.cpp:209-214` — `RepairDataFile` early-return leaks `tmpFile` descriptor; no `chmod` restore on DataFile.
- `src/db/dstatist.cpp:51-67` — Mixed handling of short reads — truncated `.STA` treated as valid for some fields, corrupts `Subtract` math.
- `src/ph/ph_eng.cpp:84-102` — `ok &= SaveXXX(file)` does **not** short-circuit; failed save midway leaves polluted `PH_INFO.DAT` marked written.
- `src/db/dstorage.cpp:399-403` — `Add()` rewinds `dfSeekPos` on remainder without log/warn — silent data overwrite.
- `src/db/dstorage.cpp:802-819` — `IndexCache::Load` returns TRUE on zero bytes read; callers can't distinguish empty index from successful load; `FindNextNumber` may spin forever returning 0.

UI / controller:
- `src/ctrl/control.cpp:494` — `static char msg[0x40]` + `sprintf("%d errores de comunicación en cabina: %s", N, Name)` — long Name overruns.
- `src/ui/w_statbr.cpp:240` — `UIW_STAT_BAR::setMsg` unchecked `strcpy` into `Msg[0x40]`; callers pass arbitrarily long messages.
- `src/ui/w_statbr.cpp:162-167` — `static char *helpText = HelpText` initialized once; if first call has `HelpText==NULL`, subsequent `strcmp(NULL, ...)` faults.
- `src/ui/w_statbr.cpp:269-284` — `setHelpInfo` derefs `HelpInfo` sentinel without NULL guard.
- `src/ui/view.cpp:44` — `new PH_ENGINE::CallInfo[NumOfClusters][CLUSTER_SIZE]` — multidimensional new with non-const first dim (Borland extension); on failure, dtor's `delete[] m_callInfo` faults.
- `src/ui/view.cpp:96-110` — `loadStatBMP` does `new UCHAR[w*h]` without checking that `bmpFile.Load` actually populated `w`/`h`; garbage dims → huge alloc.
- `src/tb/tb_tools.cpp:58,208` — `UIW_SPY`/`UIW_LOCK` validate `boothNum < 0 || boothNum > N*16` then `--boothNum` — accepts 0, decrements to `-1`, garbage cluster/booth downstream.
- `src/ctrl/ctrl_rf.cpp:584-587` — `RefreshBoothDisplay` strcpy(area)+strcat(phone) into PHONE-sized buffer without length check.
- `src/ctrl/ctrl_rf.cpp:319-332` — `sprintf` into bounded `watchDogMessage` with %f's can produce >64-char strings; flows into `setMsg` → strcpy into `Msg[0x40]`.
- `src/ctrl/control.cpp:60-67,207,702` — `errorMemory = new char[0x2000]` but freed with non-array `delete` — UB, possible heap corruption.

Infra:
- `src/cfg.cpp:1083` — `E_FIRST_EXT` clamps via `CLUSTERS*CLUSTER_SIZE` but raw INI input is not clamped before `AdjustHeader/Adjust`.
- `src/cfg.cpp:681-684` — `_DelSpaces` overlapping `strcpy(&strLine[i], &strLine[i+1])` is UB per ANSI; should be `memmove`.
- `src/cfg.cpp:198-220` — `AdjustFooter` writes into `P_FOOTER[0x90]` at fixed offsets without checking that `P_FOOTER1/2` ≤ 64 chars.
- `src/spooler.cpp:213-214` — `Serial->GetStatus()` dereferenced **before** the `if (Serial && ...)` null check on the same line.
- `src/spooler.cpp:52,86,93,104,163` — `SpoolerQueueMutex` commented out everywhere — queue head/tail unsynchronized between `Print()` (callers) and `Poll()` (main loop).
- `src/spooler.cpp:54-71` — `strlen(s)` called before the 0xFF-scan; if `s` lacks NUL within 0xFF, walks off-end.
- `src/stm2.cpp:50-67` — `check()` probes banks then restores; power loss mid-probe never writes back. No backup mechanism.
- `src/stm2.cpp:200-203` — Partial read sets `ret=FALSE` but buffer already partially overwritten; caller can't detect corrupt data.
- `src/mutex.cpp:14-18` — Spin uses `btr`/`bts` busy-wait, no timeout, no interrupt-disable; document the invariant.
- `src/st.cpp:97-114` — Borland `new` returns NULL on OOM, not throws — `defaultStorage->storageError` deref will fault if alloc failed.

### MEDIUM / LOW

Full lists in the agent transcripts (file references below). Highlights:

- `src/eeprom.cpp:93-101` — Loop `for (i=0; i<=nBytes; i++) { ...; i++; ... }` accesses `wrBytes[80]/[81]` when `nBytes==80` (OOB; buffer is BYTE[80]).
- `src/eeprom.cpp:30` — `strlen(bytes)` on non-NUL-terminated byte buffer.
- `src/ui/bdisplay.cpp:103-160` — `sprintf` into `STR16` of `%0.2f` floats with `totalCost`; 7+ digits + decimal overflows 16-byte buffer.
- `src/ui/w_table.cpp:527-553` — `delete vScroll;` without nulling; re-entry of `ScrollCompute` checks non-null → use-after-free.
- `src/ui/w_table.cpp:84` — `Event()` derefs `object` (Current()) without null check.
- `src/ctrl/ctrl_rf.cpp:370,620` — `static WORD maxBooth = g_cfg->ACTIVE_CLUSTERS*CLUSTER_SIZE;` captured once; config change post-init makes refresh skip booths.
- `src/st_util.cpp:316-329` — `_CopyFile`: leaks source file handle on failed creat / failed alloc.
- `src/cfg.cpp:69` — `_Decrypt(this, file.gcount())` runs before short-read check at line 66; decrypts wrong size on short read.
- `src/calc.cpp` — Single unsafe-string usage; verify it.
- `src/ssaver.cpp:24-36` — `bmpFile->Load()` return ignored; garbage Bmp painted.
- `src/db/db_eng.cpp:248-309` — `Recover()` puts `LSDList[MAX_BOOTH]` on stack (8 KB total stack) — risk of stack exhaustion.
- `src/db/db_view.cpp:178,228` — `&UI_BIGNUM(fromNumber)` — address of temporary, UB.
- `MAKEFILE:148-155` — `DLLS` list duplicates `pr_*.c` filenames; single `.PATH.c` blocks future C sources elsewhere.
- `MAKEFILE` — `ptclassl pbidsl` libs included; only `calc.cpp` is an obvious classlib user.
- `st.cfg:2` — Defines `__DEBUG=0` but MAKEFILE uses `$d(DEBUG)` presence test — `__DEBUG=0` macro is unused/misleading.

---

## 4. Cross-cutting themes

1. **No `volatile` on ISR-shared state** (`rt_eng.h`) — single biggest stability risk; with `-O`, main thread can miss ISR updates entirely. Adding `volatile` is mechanical; **but** stack-checking is off and the ISR is dense, so test carefully.
2. **`write()`/`creat()` return values systematically ignored** in `db/dstorage.cpp`, `db/dstatist.cpp` — disk-full or quota events silently corrupt data files. There is **no atomic data+index update** in `Add()` — crash mid-write yields phantom or orphan receipts.
3. **String buffer assumptions** — many `strcpy`/`sprintf`/`strcat` calls into fixed buffers (`PHONE=17`, `Msg[0x40]`, `STR16`, `STR128`, `STR512`) without bounds checks. Parser/query paths trust well-formed `.inf` input; corrupted files can crash.
4. **Static state in instance classes** — `SERIAL`, `SPOOLER`, `UIW_MANUAL` keep state in statics; second instantiation clobbers/leaks prior. Fragile but only crashes if/when re-instantiation happens.
5. **Fault-recovery handlers (`NewGPFHandler`, `new_handler`) themselves crash-prone** — calling `delete` (heap, PHAPI) from a GPF ISR or partial teardown of globals on OOM. The crash handler can cause a worse crash.
6. **Two large duplicated structures** that aren't refactor targets per "stability only" mandate, but worth knowing: 10 `pr_*.c` printer DLLs share ~420 lines of identical boilerplate each; `CONTROLLER::Event` is 765 lines.

---

## 5. Subsystem report files (raw agent output)

The four agent reports above are summarized inline. If you want the raw text, the agents are no longer running — re-running them is possible but would produce different prose. Findings are stable; severity ratings reflect my best judgment after seeing all four reports together.

---

## 6. Verification status

Five CRITICAL findings spot-verified against source:

| # | Status |
|---|---|
| **C1** `ctrl_ev.cpp:488` | ✅ Confirmed. `DB_STORAGE::Iterator::Current()` does not advance; no `++it` in loop body. Preceding case UE_RPP at 469 has it; UE_RNPP copy-paste omitted it. |
| **C2** `serial.cpp:191` | ✅ Confirmed. Empty `;` body, writes at line 193 unconditional. |
| **C3** `dongle.cpp:34` | ✅ Confirmed. `!` precedence makes guard always 0. |
| **C4** `ct_util.cpp:30` | ✅ Confirmed. `\0x0A` parses as `\0` + literal `x0A`. |
| `ctrl_ev.cpp:523` (Agent C had this as CRITICAL) | ❌ **False positive.** `break;` at line 520 exits the switch correctly. Removed from this list. |

All other CRITICAL/HIGH findings remain **unverified** — they should be confirmed by reading the cited file:line before applying a fix.

---

## 7. Recommended next actions

Per CLAUDE.md Working Style: confirm approach before broad changes.

1. **Quick wins (verified, low blast radius):**
   - `ctrl_ev.cpp:488` — add `it++;` before closing `}` of UE_RNPP while-loop (copy from sibling case at line 469).
   - `dongle.cpp:34` — add parens: `if (!(biosprint(...) & BIOS_PRINT_BUSY))`.
   - `ct_util.cpp:30` — fix to `"\x1B\x70\x00\x0A\x0A\xFF"` (or whatever the intended sequence was — verify against device manual).
   - `serial.cpp:191` — change `;` to `return;` (or whatever discard semantics the ring buffer should have).

2. **Verify-then-fix CRITICALs:** C5–C22. Read each cited file:line first.

3. **Strategic (not quick):**
   - Add `volatile` to ISR-shared state in `rt_eng.h` (must be tested under load).
   - Audit `write()`/`creat()` return value handling across `dstorage.cpp` / `dstatist.cpp` — adopt a consistent error path (likely setting a "DB unhealthy" flag + refusing further writes).
   - Decide whether `~UIW_VIEW` leak (C16) is genuine or whether Zinc deletes the 2-D arrays via parent ownership.

4. **Do NOT do in this pass:**
   - Refactor `pr_*.c` duplication — out of scope (stability only).
   - Refactor 765-line `CONTROLLER::Event` — out of scope.
   - Remove pre-existing dead code (per CLAUDE.md §3 — flag, don't delete).
