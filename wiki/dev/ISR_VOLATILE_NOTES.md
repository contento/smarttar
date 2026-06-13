# ISR / main-loop concurrency — design pass (volatile group)

> **STATUS: this is the parked Tier 3 of the stability milestone.**
> The rest of [STABILITY_AUDIT.md](STABILITY_AUDIT.md) is CLOSED; this
> ISR/real-time group is **deferred — needs a DOSBox-X build + load-test loop
> on (ideally) real telephony hardware** before it can be confidently closed.
> Start any Tier 3 session here. The P0 guard-rail below is in force *now*:
> **do not enable compiler optimization** without first doing the
> `volatile`/atomicity audit in this note.

Read-only analysis of the HIGH "ISR / real-time" findings (audit §3 HIGH,
§4.1). **No code changed.** This is the plan for a session that has the
DOSBox-X build + load-test loop. Date: 2026-06-07.

## TL;DR — the headline reframes the audit

The audit calls the missing `volatile` "the single biggest stability risk."
That is true *in principle* but **currently dormant**, because:

> `st/st.cfg` enables **no optimization** (`-ml -k- -H` + include/lib paths;
> no `-O`/`-Z`/`-G`/`-Or`), and no build variant adds any. Borland C++ 3.1
> does **not** cache memory operands in registers across statements without
> those flags — so today the compiler already reloads `Clusters[]` / shared
> state on every access, which is exactly what `volatile` would force.

So the `volatile` gap only *activates* if someone later enables optimization.
The highest-value action is therefore a **guard-rail**, not a sweep.

Two *other* hazards in this group are **optimization-independent** and real
(below), but their practical impact is low (transient wrong value for one
poll cycle, self-correcting — not corruption, not crash).

## The shared state

The ISR/main boundary is essentially the **entire `BoothCluster`**
(`include/bcluster.h`), held in `ENGINE::Clusters[]`:

- **Writers (ISR, 100 Hz):** `NewISR08h` (`rt_isr.cpp`) → `OnTimerTick` +
  `EvalToneState`/`EvalPulseState` mutate nearly every field (DataPort,
  StateCounts, DialCounts, ElapsedCounts, Tariffs, ToneFSs/PulseFSs,
  CallAttrs, NumOfDigits, Found, Locked, …). `OnTimerEnd` writes
  `GeneralPort`.
- **Readers (main loop):** `CONTROLLER::Poll` → `UnloadRTEngine`,
  `CookViewData`, `RefreshBoothDisplay`, `GetClusters`, `GetReceipt`, and the
  scalar accessors in `rt_util.cpp` (GetElapsed, GetTariff, …).

The mutex (`mutex.cpp`, `RTBoothClustersMutex`) is a `bts`/`btr` spin lock.
**It does not synchronize the ISR/main pair** and cannot be made to:

- The ISR never takes it (the FSM writes `Clusters[]` directly).
- It *can't* take it — if main holds the lock when the timer fires, an ISR
  spin-wait would deadlock (main is suspended inside the ISR). Confirms audit
  `rt_util.cpp:533` (one-way protection) and `rt_store.cpp:13`
  (mutex-from-ISR). On 16-bit DOS the only correct ISR/main mechanism is a
  brief `cli`/`sti` window on the main side.

## Present hazards (optimization-independent)

1. **Torn 32-bit reads.** `DWORD` fields read/written as two 16-bit accesses:
   `ElapsedCounts`, `FinalElapsedCounts`, `PreTime`, and the
   `DTMFDigits`/`U_DTMFDigits[2]` union. The ISR does
   `Clusters[c].ElapsedCounts[b] += T_EVAL` (`rt_util.cpp:302`, read-mod-write
   of two words); main reads it at `rt_util.cpp:287`. A read landing between
   the two-word update returns a torn value. Window ~microseconds; effect is a
   one-tick-wrong elapsed time (display flicker / at most a one-tick billing
   wobble), corrected on the next poll.

2. **Torn snapshot in `GetClusters`** (`rt_util.cpp:530`). The bulk
   `memcpy(clusters, Clusters, sizeof(BoothCluster)*N)` runs under the (ineffective)
   mutex while the ISR mutates `Clusters[]` mid-copy → an internally
   inconsistent snapshot. Consumer is the view refresh, so impact is transient
   visual inconsistency, not data loss.

## Prioritized plan

**P0 — guard-rail (cheap, do first, no behavior change).**
Add a comment block to `st/st.cfg` (and a note in the audit) stating that
optimization must NOT be enabled (`-O`/`-Z`/`-G`/`-Or`) without first doing a
`volatile`/atomicity audit of the ISR/main shared state, because the code
relies on the compiler reloading `Clusters[]` from memory. This neutralizes
the "biggest risk" by making the dependency explicit. Testable by inspection.

**P1 — protect the torn scalar DWORD reads (real, low-impact).**
Wrap the *scalar* multi-word reads (`GetElapsed`, `GetFinalElapsed`,
`GetPreTime`, DTMF digit reads) in a short interrupt-off window:
```c
WORD f = _FLAGS;        // or use disable()/enable() ONLY if caller IF==1
disable();
DWORD v = Clusters[cNum].ElapsedCounts[bNum];   // single 2-word read, no ISR between
if (f & 0x200) enable();                         // restore IF, don't force-enable
```
Do **not** naively `enable()` unconditionally — that is the same bug as audit
`serial.cpp:198` (clobbers a caller's IF=0). Save/restore the flag. The window
is two `mov`s, negligible for tick timing.

**P2 — `GetClusters` snapshot (judgment call, likely ACCEPT).**
Options: (a) accept the transient inconsistency and document it — the consumer
is display-only and self-corrects; (b) double-snapshot and compare a version
counter; (c) chunked `cli`/`sti`. A full-struct `cli` memcpy is **too long**
(hundreds of bytes × N clusters with interrupts off drops timer ticks) — do
NOT do that. Recommendation: **(a) accept + comment**, revisit only if a
non-display consumer appears.

**Do NOT do now:**
- Blanket `volatile` sweep — dormant (no optimization), large codegen-
  affecting change, untestable without the load loop. Revisit only if/when
  optimization is ever enabled (and then it is mandatory).
- Mutex changes — the spin lock can't be made ISR-safe; it is near-useless but
  harmless. Leave it.

## The non-volatile ISR items (status)

These were grouped under "ISR/real-time" but are independent of the volatile
theme:

- `rt_isr.cpp:25` static locals (`cNum/bNum/cycles`) — **low risk**: the
  `isInside` guard plus no in-ISR interrupt-enable means IRQ0 never re-enters
  the body, so the statics are never reentered.
- `rt_isr.cpp:28` reentry path — on detected reentry it deliberately EOIs and
  triggers a GP (`_ES=0xB0B0`) to panic-recover; it is not a silent BIOS-tick
  drop. Reentry shouldn't occur anyway. **Low risk.**
- `rt_isr.cpp:91` carry-flag chain (`cycles += DIVISOR; if (_FLAGS & 0x01)`)
  to chain the BIOS 18.2 Hz tick — **works but fragile**: depends on no code
  landing between the add and the flag test. Leave + add a "do not insert code
  here" comment. Real risk only if a future edit splits them.
- `serial.cpp:87` `inportb(IMR) | ~(...)` IRQ mask — **real logic bug**
  (`~` on a promoted int → `0xFFxx`, can mask unrelated IRQs), but runs once
  at serial init and is independent of the volatile work. Worth a separate
  small fix + test.
- `serial.cpp:198` in-ISR `enable()` — re-enables IF regardless of caller;
  same save/restore pattern as P1. Separate serial fix.
- `serial.cpp:46` all-static `SERIAL` state — like C19 but **load-bearing**:
  the static ISR needs static state to reach. Defended by single instantiation.
  Lower priority; do not "fix" by de-static-ing without rethinking the ISR.

## Load-test checklist (for P1, and any future volatile work)

Build `./build.sh demo`, then under DOSBox-X drive a busy scenario
(`demo.ini` high `calls_per_minute`, all booths active) and watch for:

1. Elapsed-time display advancing monotonically per booth (no backwards jumps
   = no torn reads surfacing).
2. Receipt totals/billing consistent with call durations across a long run.
3. No `0xB0B0` GP (ISR reentry) or watchdog trips under load.
4. BIOS clock (DOS `TIME`) still advances correctly (carry-flag chain intact)
   after a multi-minute run.
5. If optimization is ever enabled experimentally: repeat 1–4 — this is where
   a missing `volatile` would first surface as frozen/stale booth state.
