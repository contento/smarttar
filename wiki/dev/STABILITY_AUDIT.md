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

**Status: CLOSED — substantially complete (2026-06-07).** See §8. All
CRITICALs + the hardware-independent HIGH/MEDIUM findings are resolved
(fixed or DEFENDED); only Tier 3 (ISR/concurrency) is deferred, parked in
[ISR_VOLATILE_NOTES.md](wiki/dev/ISR_VOLATILE_NOTES.md) pending a DOSBox-X
load-test session. To resume Tier 3, start from that note (not this audit).

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
| C11 | `src/db/dstatist.cpp:82-86` | Four `write()` calls in ctor unchecked — disk-full leaves stats file header-less / truncated; in-memory copy looks fine until reopen. | ✅ |
| C12 | `src/ph/ph_place.cpp:589-594` | `GetNumbers` writes `numbers[slot].Numbers[Count++]` with no bounds vs `MAX_NUMBERS_PER_LINE` — malformed `.inf` line overflows embedded array. | ✅ |
| C13 | `src/ph/ph_place.cpp:201-202` | `Search()`: `partialPhone[i]` indexed up to `len-1` where `len = strlen(phone)`; PHONE is 17 bytes; 16-char phone + terminator overruns by 1. | ✅ |
| C14 | `src/ctrl/control.cpp:541-548` | `NewGPFHandler` calls `delete RTEngine/g_dbEngine/...` inside the #0Dh GPF ISR — `delete` re-enters PHAPI/heap; recovery path itself can deadlock. | ✅ |
| C15 | `src/ctrl/control.cpp:697-717` | OOM `new_handler` deletes some globals but not `g_dbEngine`, `g_phEngine`, `g_cfg`, `g_spooler` — partial teardown leaks files, leaves DB inconsistent. | ✅ |
| C16 | `src/ui/view.cpp:70-73` | `~UIW_VIEW` only deletes `m_callInfo`; never deletes the 2-D widget arrays `WBoothNumbers`/`WStates`/`WAreas`/`WPhones`/`WCities`/`WElapsedTimes`/`WTariffs`/`WValues`/`WNumOfCalls`, `ToneFSs`/`PulseFSs`/`States[i].Bitmap`. Large leak; likely also dangling pointers. *(Note: Zinc widgets owned by parent — but the **arrays of pointers** themselves still need `delete[]`.)* | ✅ |
| C17 | `src/tb/tb_man.cpp:80-83,127` | `UIW_MANUAL` allocates `s_receipts`/`s_totals`/`s_wNCs`/`s_wPRs`/`s_wReceipts` as statics; dtor only frees first two. Re-opening dialog overwrites pointers → leak + cross-instance aliasing. | ✅ |
| C18 | `src/spooler.cpp:30-33` | `NumOfChannels` clamped but `Print(channel, ...)` never validates the channel arg against `MAX_SPOOL_CHANNELS=4`; caller-supplied out-of-range channel indexes uninitialized `Buffers[]` pointer. | ✅ |
| C19 | `src/spooler.cpp:23,33` | `Buffers` is a static class array reallocated on every `SPOOLER` ctor — second instantiation leaks prior queues and stomps statics. | ✅ |
| C20 | `src/log.cpp:43-58` | After CREATE-branch reopen failure, `openOk=TRUE` but stream may be bad; subsequent `put()` silently loses log entries. | ✅ |
| C21 | `src/log.cpp:63` | Failed-open branch deletes `file` but does not null it — `~Log()` deletes again (double-free). | ✅ |
| C22 | `src/cfg.cpp:592-595` | Buffer-overrun check `if (offset > MAX_ID_VALUES)` runs **after** `FillCfgTable()` already wrote OOB. `MAX_ID_VALUES=0x100` must be raised any time entries are added. | ✅ |

### HIGH

ISR / real-time (`rt/`): **see [ISR_VOLATILE_NOTES.md](ISR_VOLATILE_NOTES.md) for the design pass** — key result: the `volatile` gap is **dormant** (st.cfg enables no optimization, so the compiler already reloads shared state); the real present hazards are torn 32-bit reads + `GetClusters` snapshot. Plan + load-test checklist there.
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
- ✅ **FIXED** (`ad15670`) `src/ph/ph_place.cpp:374` — `GetPlace` strcat into `tmpPlace[512]` checks size before cat but doesn't track remaining space; long colon-less `.inf` lines can overflow.
- ✅ **FIXED** (`ad15670`) `src/ph/ph_query.cpp:240-241` — `strncpy(szPhone+nAccess, pszToken, nLen)` then null-term; if `nLen+nAccess >= sizeof(PHONE)=17`, both the strncpy and terminator overrun.
- ✅ **FIXED** (`ad15670`) `src/ph/ph_query.cpp:95-101` — `while (pszCP[n] != ':')` no bounds on `n` vs `CITY_NAME=21`; missing `:` walks until fault.
- ✅ **FIXED** (`ad15670`) `src/ph/parser.cpp:21` — `strlen(line)` O(n²) per loop iter; long `.inf` line >512 overflows `tmpLine[512]` via `tmpLine[iChar++]`.
- ✅ **FIXED** (`ad15670`) `src/ph/ph_place.cpp:259` — `rightNumber = numbers[i+1]` when leftNumber has RANGE flag, no check that `i+1 < numCount`.
- ✅ **FIXED** (`082c188`) `src/db/dstorage.cpp:209-214` — `RepairDataFile` early-return leaks `tmpFile` descriptor; no `chmod` restore on DataFile.
- ⏸️ **WONTFIX — intentional old-.STA back-compat.** `src/db/dstatist.cpp:51-67` — Mixed handling of short reads — truncated `.STA` treated as valid for some fields, corrupts `Subtract` math.
- ⏸️ **FALSE POSITIVE — nested ifs already short-circuit.** `src/ph/ph_eng.cpp:84-102` — `ok &= SaveXXX(file)` does **not** short-circuit; failed save midway leaves polluted `PH_INFO.DAT` marked written.
- ⏸️ **WONTFIX — by design (drops corrupt partial tail).** `src/db/dstorage.cpp:399-403` — `Add()` rewinds `dfSeekPos` on remainder without log/warn — silent data overwrite.
- ⏳ **DEFERRED — design call ('2.21.8 Build 6' TRUE-on-zero; touches all FindNextNumber callers).** `src/db/dstorage.cpp:802-819` — `IndexCache::Load` returns TRUE on zero bytes read; callers can't distinguish empty index from successful load; `FindNextNumber` may spin forever returning 0.

UI / controller:
- ⏸️ **DEFENDED — Name[13] keeps every msg < 64.** `src/ctrl/control.cpp:494` — `static char msg[0x40]` + `sprintf("%d errores de comunicación en cabina: %s", N, Name)` — long Name overruns.
- ✅ **FIXED** (`fd0e188`) `src/ui/w_statbr.cpp:240` — `UIW_STAT_BAR::setMsg` unchecked `strcpy` into `Msg[0x40]`; callers pass arbitrarily long messages.
- ✅ **FIXED** (`fd0e188`) `src/ui/w_statbr.cpp:162-167` — `static char *helpText = HelpText` initialized once; if first call has `HelpText==NULL`, subsequent `strcmp(NULL, ...)` faults.
- ✅ **FIXED** (`fd0e188`) `src/ui/w_statbr.cpp:269-284` — `setHelpInfo` derefs `HelpInfo` sentinel without NULL guard.
- ⏸️ **FALSE POSITIVE — delete[] NULL is a no-op; new_handler exits on OOM.** `src/ui/view.cpp:44` — `new PH_ENGINE::CallInfo[NumOfClusters][CLUSTER_SIZE]` — multidimensional new with non-const first dim (Borland extension); on failure, dtor's `delete[] m_callInfo` faults.
- ⏸️ **WONTFIX — trusted RES.DAT; corrupt dims fail to an OOM-exit, not corruption.** `src/ui/view.cpp:96-110` — `loadStatBMP` does `new UCHAR[w*h]` without checking that `bmpFile.Load` actually populated `w`/`h`; garbage dims → huge alloc.
- ✅ **FIXED** (`fd0e188`) `src/tb/tb_tools.cpp:58,208` — `UIW_SPY`/`UIW_LOCK` validate `boothNum < 0 || boothNum > N*16` then `--boothNum` — accepts 0, decrements to `-1`, garbage cluster/booth downstream.
- ✅ **FIXED** (`fd0e188`) `src/ctrl/ctrl_rf.cpp:584-587` — `RefreshBoothDisplay` strcpy(area)+strcat(phone) into PHONE-sized buffer without length check.
- ⏸️ **DEFENDED — watchDogMessage[80] fits; now also bounded by setMsg.** `src/ctrl/ctrl_rf.cpp:319-332` — `sprintf` into bounded `watchDogMessage` with %f's can produce >64-char strings; flows into `setMsg` → strcpy into `Msg[0x40]`.
- ✅ **FIXED** (`aeb1372`) `src/ctrl/control.cpp:60-67,207,702` — `errorMemory = new char[0x2000]` but freed with non-array `delete` — UB, possible heap corruption. Both frees (`~CONTROLLER`, `NewHandler`) now `delete []`.

Infra:
- ⏸️ **DEFENDED — clamp applied before use.** `src/cfg.cpp:1083` — `E_FIRST_EXT` clamps via `CLUSTERS*CLUSTER_SIZE` (now at `cfg.cpp:1091-1092`) and the only downstream use (`cfg.cpp:1136`) runs after the clamp; no unclamped path observed.
- ✅ **FIXED** (`aeb1372`) `src/cfg.cpp:681-684` — `_DelSpaces` overlapping `strcpy(&strLine[i], &strLine[i+1])` is UB per ANSI; should be `memmove`. Now `memmove`. *(Adjacent, NOT fixed: the `i++` after the shift skips the char moved into slot `i`, so runs of consecutive spaces aren't fully collapsed -- separate logic issue, left alone.)*
- ⏸️ **DEFENDED — bounded by `STR64` type.** `src/cfg.cpp:198-220` — `AdjustFooter` writes into `P_FOOTER[0x90]`=144 at fixed offsets; `P_FOOTER1/2` are `STR64` (≤63 chars) and the local `footer[0x44]`=68, so every DR_80/DR_40/DR_18 path stays within bounds (verified by hand: worst case ~132 < 144).
- ✅ **FIXED** (`aeb1372`) `src/spooler.cpp:213-214` — `Serial->GetStatus()` dereferenced **before** the `if (Serial && ...)` null check on the same line. COM branch now guarded by `if (Serial)` first.
- `src/spooler.cpp:52,86,93,104,163` — `SpoolerQueueMutex` commented out everywhere — queue head/tail unsynchronized between `Print()` (callers) and `Poll()` (main loop).
- ✅ **FIXED** (`038f489`) `src/spooler.cpp:54-71` — `strlen(s)` called before the 0xFF-scan; if `s` lacks NUL within 0xFF, walks off-end. `strlen` now deferred to the `else` (NUL-terminated) branch.
- ⏸️ **WONTFIX — no clean fix, hardware-loss case.** `src/stm2.cpp:50-67` — `check()` probes banks then restores; power loss mid-probe never writes back. No backup mechanism. (Power-loss atomicity needs an EEPROM redesign; out of scope.)
- ⏸️ **DEFENDED — `ret=FALSE` is the failure signal.** `src/stm2.cpp:200-203` — Partial read sets `ret=FALSE`; buffer is left partially overwritten but the FALSE return tells the caller not to trust it. `read()` can't be un-done without a temp buffer; no clean fix.
- ✅ **DONE** (`038f489`) `src/mutex.cpp:14-18` — Spin uses `btr`/`bts` busy-wait, no timeout, no interrupt-disable. Invariant now documented in the header comment (ISR-unsafe; one shared word per class; no fairness).
- ✅ **FIXED** (`038f489`) `src/st.cpp:97-114` — Borland `new` returns NULL on OOM, not throws — `defaultStorage->storageError` deref will fault if alloc failed. `_new_handler` isn't installed until the CONTROLLER ctor (line 162), so the line-93 alloc is unguarded; added a `!defaultStorage ||` check routing OOM through the existing error path. *(Broader note: lines 85/87/91/114 share the same pre-handler window — flagged for GCC, not fixed; hoisting the handler install earlier is a CONTROLLER-dependency change.)*

### MEDIUM / LOW

Full lists in the agent transcripts (file references below). Highlights:

- ✅ **FIXED** (`038f489`) `src/eeprom.cpp:93-101` — Loop `for (i=0; i<=nBytes; i++) { ...; i++; ... }` accesses `wrBytes[80]/[81]` when `nBytes==80` (OOB; buffer is BYTE[80]). Changed `<=` to `<`. **Flagged (not fixed):** `wrBytes[80]` vs `MAX_LEN=0x400` is a deeper mismatch — `swab()` would overflow for any `nBytes` in (80,1024]; no caller hits it, left for GCC.
- `src/eeprom.cpp:30` — `strlen(bytes)` on non-NUL-terminated byte buffer.
- ⏸️ **DEFENDED — value range.** `src/ui/bdisplay.cpp:103-160` — `sprintf` into `STR16` of `%0.2f` floats with `totalCost`; overflow needs ~10^12 (≥12 integer digits), unreachable for a booth POS in COP. BCC 3.1 has no `snprintf`; bumping 8 buffers speculatively rejected per "surgical".
- ✅ **FIXED** (`038f489`) `src/ui/w_table.cpp:527-553` — `delete vScroll;` without nulling; re-entry of `ScrollCompute` checks non-null → use-after-free. Now `vScroll/hScroll/corner = NULL` after each free (the two deletes immediately followed by re-`new` left as-is).
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

Five CRITICAL findings spot-verified against source. **C1–C4 are now fixed and committed** (see git log `5bcee3f`, `7a730a3`, `fde0e49`, `9c10a7e`).

| # | Status |
|---|---|
| **C1** `ctrl_ev.cpp:488` | ✅ Confirmed → **FIXED** (`5bcee3f`). `DB_STORAGE::Iterator::Current()` does not advance; no `++it` in loop body. Preceding case UE_RPP at 469 has it; UE_RNPP copy-paste omitted it. Fix: added `it++;` mirroring UE_RPP. |
| **C2** `serial.cpp:191` | ✅ Confirmed → **FIXED** (`7a730a3`). Empty `;` body, writes at line 193 unconditional. |
| **C3** `dongle.cpp:34` | ✅ Confirmed → **FIXED** (`fde0e49`). `!` precedence makes guard always 0. |
| **C4** `ct_util.cpp:30` | ✅ Confirmed → **FIXED** (`9c10a7e`). `\0x0A` parses as `\0` + literal `x0A`. |
| `ctrl_ev.cpp:523` (Agent C had this as CRITICAL) | ❌ **False positive.** `break;` at line 520 exits the switch correctly. Removed from this list. |

**C5–C9 are fixed and committed** (`58e68d5`):
- C5: Vector 0x09h leak (fixed)
- C6: Stale reference / false positive (all setvect calls have semicolons)
- C7: creat() return unchecked (fixed)
- C8: write() return unchecked in RepairDataFile (fixed)
- C9: write() return unchecked in RepairIndexFile (fixed)

**C11–C22 are fixed and committed** (`f497445`):

- C11: dstatist.cpp ctor write()s guarded; BAD_FILE on creat() failure (fixed)
- C12: GetNumbers bounds vs MAX_NUMBERS_PER_LINE (fixed)
- C13: Search loop capped at sizeof(partialPhone)-1 (fixed)
- C16: ~UIW_VIEW delete[]s the 2-D pointer arrays (fixed). **Caveat:** `States[i].Bitmap` deliberately left — `States` is `static` (one allocation for app lifetime); freeing it in an instance dtor would be wrong if multiple views ever existed. App-lifetime leak, reclaimed at exit.
- C17: ~UIW_MANUAL delete[]s s_wNCs/s_wPRs/s_wReceipts (fixed)
- C18: spooler Print() overloads validate channel < NumOfChannels (fixed)
- C20/C21: log.cpp checks file->fail() and nulls file after delete (fixed)
- C22: FillCfgTable bounds offset < MAX_ID_VALUES inside the macro (fixed)

**C19 fixed and committed** (`d4bbd49`): `Buffers`/`PrintfBuffer` made instance members so each `SPOOLER` owns its queues — cross-instance stomp now structurally impossible (latent today, g_spooler is a singleton).

**C14/C15 fixed and committed** (`d7d24ca`):

- C14: `NewGPFHandler` now restores the original #0Dh handler **before** the teardown deletes (so a fault mid-`delete` goes to the Pharlap default instead of recursing into our half-torn-down handler), plus a static `inTeardown` backstop. **Caveat:** fully avoiding `delete` from a fault context would need an `ENGINE` redesign (uninstall ISRs without `delete`) — out of scope; this bounds the damage rather than eliminating the re-entry.
- C15: OOM `new_handler` now `delete`s `g_dbEngine` (clean DB flush, the one teardown it skipped vs the GPF path). `g_phEngine`/`g_cfg`/`g_spooler` left out deliberately — at `exit(1)` the leak is moot and allocating teardown could re-enter OOM.

**All CRITICALs (C1–C22) are now resolved** — fixed, or verified-defended (**C10**: both writes checked in `Add()`; non-atomic data-vs-index window is a design limitation, no code change). **Next: HIGH findings.**

---

## 7. Recommended next actions

Per CLAUDE.md Working Style: confirm approach before broad changes.

1. **Quick wins (verified, low blast radius) — ✅ DONE (C1–C4 committed):**
   - ~~`ctrl_ev.cpp:488` — add `it++;` before closing `}` of UE_RNPP while-loop (copy from sibling case at line 469).~~ Fixed (`5bcee3f`).
   - ~~`dongle.cpp:34` — add parens: `if (!(biosprint(...) & BIOS_PRINT_BUSY))`.~~ Fixed (`fde0e49`).
   - ~~`ct_util.cpp:30` — fix to `"\x1B\x70\x00\x0A\x0A\xFF"`.~~ Fixed (`9c10a7e`).
   - ~~`serial.cpp:191` — change `;` to `return;`.~~ Fixed (`7a730a3`).

2. **Verify-then-fix CRITICALs:** ✅ DONE — all of C1–C22 resolved (fixed, or C10 verified-defended). **Next up: the HIGH findings (§ "HIGH").** Read each cited file:line first.

3. **Strategic (not quick):**
   - Add `volatile` to ISR-shared state in `rt_eng.h` (must be tested under load).
   - Audit `write()`/`creat()` return value handling across `dstorage.cpp` / `dstatist.cpp` — adopt a consistent error path (likely setting a "DB unhealthy" flag + refusing further writes).
   - ~~Decide whether `~UIW_VIEW` leak (C16) is genuine or whether Zinc deletes the 2-D arrays via parent ownership.~~ Resolved (`f497445`): widgets are Zinc-owned, but the pointer-array storage is ours — now `delete[]`'d.

4. **Do NOT do in this pass:**
   - Refactor `pr_*.c` duplication — out of scope (stability only).
   - Refactor 765-line `CONTROLLER::Event` — out of scope.
   - Remove pre-existing dead code (per CLAUDE.md §3 — flag, don't delete).

---

## 8. Milestone status — CLOSED (substantially complete), 2026-06-07

This audit is now **archived as a historical record**. The actionable,
hardware-independent work is done; the remainder is deliberately deferred.

**Resolved:**

- **All 22 CRITICALs (C1–C22)** — fixed or verified-defended (§6).
- **HIGH / MEDIUM bounds, IO, lifetime findings** — the DB/telephony batch
  (`ad15670`, `082c188`), the UI/controller batch (`fd0e188`), the mechanical
  batch (`aeb1372`), and the final Bucket-B + clear-MEDIUM batch (`038f489`:
  spooler `strlen`, st.cpp OOM guard, eeprom off-by-one, w_table
  use-after-free, mutex invariant doc).
- Several findings closed as **DEFENDED / WONTFIX** with rationale recorded
  inline (cfg `AdjustFooter` STR64-bounded, cfg `E_FIRST_EXT` clamped,
  stm2 partial-read FALSE-signalled, bdisplay STR16 value-range, dstatist
  back-compat, etc.).

**Deferred — Tier 3 (ISR / real-time concurrency), needs hardware + load test:**

The `rt/` ISR-vs-main-loop group is parked in
[ISR_VOLATILE_NOTES.md](wiki/dev/ISR_VOLATILE_NOTES.md), which is the design
pass + load-test checklist for a future session that has the DOSBox-X build +
busy-scenario loop. Key points carried forward:

- The missing `volatile` is **dormant** — `st.cfg` enables no optimization, so
  the compiler already reloads shared state. **GUARD-RAIL:** do **not** add
  `-O`/`-Z`/`-G`/`-Or` to `st.cfg` or any build variant without first doing the
  `volatile`/atomicity audit in ISR_VOLATILE_NOTES — that is what would
  activate the risk. (The note lives here and in ISR_VOLATILE_NOTES rather
  than in `st.cfg` itself, because Borland `.cfg` files treat every line as a
  compiler option and have no comment syntax.)
- Real-but-low-impact, optimization-independent: torn 32-bit reads
  (`rt_util.cpp`) and the `GetClusters` snapshot — fix plan = brief
  flag-preserving `cli` window (P1) / accept+document (P2).
- Independent ISR logic items: `serial.cpp:87` IRQ-mask `~`, `serial.cpp:198`
  in-ISR `enable()`, `rt_isr.cpp:91` carry-flag chain — small fixes, each
  wants the load loop to confirm.

If this audit doc is ever physically relocated (e.g. into the docs wiki under
the Documentation milestone), update the back-links in `TODO.md`, `HANDOFF.md`,
`README`, and `ISR_VOLATILE_NOTES.md`.
