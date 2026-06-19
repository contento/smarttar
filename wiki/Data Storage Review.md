# Data Storage Review

**Date:** 2026-06-16
**Branch:** `feat/review-data-storage`
**Scope:** Receipt database, statistics, extension statistics

---

## Current Architecture

The storage layer has three cooperating subsystems:

```
DB_ENGINE               ← orchestrator, owns everything below
  ├─ DB_STORAGE         ← receipt persistence (.DAT + .IDX)
  │   └─ IndexCache     ← LRU batching over index entries
  ├─ DB_STATISTICS      ← per-turn accumulators (.STA)
  ├─ DB_EXT_STATISTICS  ← STPro extension stats (.STA)
  └─ STM2               ← battery-backed SRAM (crash recovery bridge)
```

Each turn produces one set of file families. They live in the app directory and get renamed on `Archive()`:

| Turn | Files | Content |
|---|---|---|
| Current | `RX.DAT` + `RX.IDX` + `RX.STA` | Normal receipts + stats |
| Current (STPro) | `RXX.DAT` + `RXX.IDX` + `RXX.STA` | Extension receipts + stats |
| Archived | `RX<TURN>_<N>.DAT`/`.IDX`/`.STA` | Previous-turn data |

---

## Receipt Database (`DB_STORAGE`)

### Format: Dual flat binary files

**.DAT** — sequential record file.
```
[DataHeader] [Receipt_0] [Receipt_1] ... [Receipt_N]
```
`DataHeader` = `FILE_HEADER` (0x40 bytes, v3) + tax/round config (~26 bytes).
`Receipt` = 111-byte packed struct with `MagicNumber=0x6719`.

**.IDX** — sparse index file.
```
[IndexHeader] [IndexEntry_0] [IndexEntry_1] ...
```
`IndexHeader` = `FILE_HEADER` + `NumOfEntries`/`LowerNumber`/`HigherNumber` + 0x20 padding.
`IndexEntry` = `MagicNumber` + `Number`(long) + `BoothNumber`(int) + `SeekPos`(long).

### Receipt Struct (111 bytes)

```
Offset  Size  Field
------  ----  -----
 0      2     MagicNumber (0x6719)       ← validity check
 2      4     Number (long)
 6      2     Tag (enum: TEL/SPECIAL_TEL/TELEX/FAX/CARD/OTHER)
 8      2     Stat (bitfield: Cooked,Manual,Printed,Archived,Paid,CallAttr,Extension,Deleted)
10      2     ExtendedStat
12      4     Date (int, packed)
16      4     Time (int, packed)
20      2     BoothNumber
22     21     CityName (char[21])
43     17     Phone || Cards[4] || Minutes (union)
60      2     Amount || Tariff (union)
62      4     ElapsedTime (long)
66      8     ValuePerMin (double)
74      8     CeilMin (double)
82      2     Percent (int)
84      8     Value (double)
92      8     Tax (double)
100     8     Tax2 (double)
108     8     DDummy (double)
     = 111
```

### Index Cache

Not a full B-tree — a flat sliding window of `IndexEntry` records read from the `.IDX` file in configurable batches. `Find(number, boothNumber)` binary-searches the cache. Cache miss → read another batch from disk.

### Key observations

1. **sizeof-based serialization.** Every struct goes to disk via `write(fd, &struct, sizeof(struct))`. Any compiler change, alignment tweak, or field addition shifts binary offsets and corrupts the file. This is the exact same fragility we just eliminated from `st.cfg`.

2. **No cross-file checksums.** The `CheckSum` field in `FILE_HEADER` is declared but **never computed** — always written as zero, never validated.

3. **Read-only validity = magic number.** Each `Receipt` is considered valid if `MagicNumber == 0x6719`. No CRC on the record body. A single-bit flip in the Number field could silently produce a wrong receipt.

4. **16-bit DOS assumptions baked in.** `sizeof(UINT)=2`, `sizeof(int)=2`, `sizeof(long)=4`. Enums are 2 bytes. `double` alignment is Borland's default (no `-a` flag). All little-endian, no endian marker.

5. **Logical deletes.** `Stat.Deleted` is set to true. Records are never physically removed except during `Repair()`. This means file sizes grow monotonically.

6. **Repair() is the sole recovery path.** Linear-scans the entire `.DAT`, validates `MagicNumber` per record, deduplicates via a 5000-entry sliding book, writes a clean `.DAT` and rebuilds `.IDX` from scratch. Works, but on 100M-record files this is slow.

7. **Crash recovery via STM2.** The STM2 battery-backed SRAM holds a circular queue of recent receipts + statistics snapshots. On `BAD_SHUTDOWN`, `DB_ENGINE::Recover()` replays missing receipts from STM2 into storage. This is reliable on real hardware but untested in DOSBox-X (no STM2 emulation).

8. **MAX_RECEIPTS = 100,000,000.** An artifact of turning over a 32-bit Number range. `long` wraps at ~2.1 billion, but `MAX_RECEIPTS` is `100000000`. In practice, files will hit filesystem size limits (2GB FAT32) well before the record count limit.

---

## Statistics (`DB_STATISTICS`)

### Format: Single flat binary file

**.STA** layout:
```
[FILE_HEADER (0x40)] [DS_ENTRY[5]] [DS_DOUBLEPRNENTRY[2]] [DS_CELLULARENTRY[5]]
```

### DS_ENTRY structure

Five entries, each holding per-type accumulation at a period level:

```
DS_ENTRY[5]   →   YEAR, MONTH, WEEK, DAY, TURN
└─ ITEM[6]    →   TEL, SPECIAL_TEL, FAX, TELEX, CARD, OTHER
   ├─ Receipts (int)
   ├─ TalkMin (double)
   ├─ PaidMin (double)
   ├─ Value (double)
   └─ Tax (double)
```

Plus: `DS_DOUBLEPRNENTRY[2]` (per-printer-channel costs), `DS_CELLULARENTRY[5]` (cellular breakdown, same hierarchy).

### Key observations

1. **In-memory accumulation, periodic flush.** Statistics are accumulated in RAM during the turn. `Flush()` does `lseek + write` overwriting the entire `.STA` in place. If the app crashes between `Add()` and `Flush()`, the `.STA` file shows stale data.

2. **STM2 mirrors live-turn stats.** Every 5 ticks, the current DS_ENTRY[DS_TURN] snapshot is written to STM2. On recovery, `Repair()` rebuilds all 5 periods from the full receipt database, so the STM2 mirror is mostly a safety net.

3. **No time-series, only period-boundary snapshots.** Statistics reset at date boundaries (day/week/month/year rollover in `Update()`). You cannot answer "how many calls in the last 3 hours" — only "how many in the current turn/day/week."

4. **Week boundary heuristic is culture-specific.** Week reset uses a `DOW - 1 >= 4` condition derived from Thu-Sun vs Mon-Wed. Hardcoded.

5. **File is fragile.** Same sizeof issues as the database. Backward-compat sections (`Dummy` fields) are silently zero-initialized on truncation.

---

## Risks Summary

| Risk | Severity | Impact | Mitigation |
|---|---|---|---|
| `sizeof`-based serialization | HIGH | Any toolchain change corrupts all data | Packed structs + static asserts only help against C++ ABI drift, not field reordering |
| No checksums | MEDIUM | Silent data corruption on bit rot | CRC field exists but is never computed |
| 16-bit DOS lock-in | HIGH | Cannot port to 32/64-bit without migration | `int=4` on modern compilers doubles every enum and int field size |
| Flat index (no B-tree) | LOW | Linear scan on cache miss; fine for 50K records | Pre-existing limit, not hitting today |
| Logical deletes | LOW | Unbounded file growth on heavy edits | Repair() compacts, but must be run explicitly |
| No transaction log | MEDIUM | Inconsistent .DAT/.IDX/.STA after hard crash | STM2 bridges the gap on real hardware; DOSBox-X has none |
| Statistics loss on crash | MEDIUM | Current turn stats may be stale | Repair() rebuilds from receipt database |

---

## Improvement Proposals

### Proposal A: Structural improvements to the current format (minimal risk)

Keep the existing binary format but harden it:

1. **Compute the CheckSum field.** A simple CRC-32 per file header catches accidental corruption and gives confidence on cross-platform copies.

2. **Add a per-record CRC trailer.** Append 2 bytes of CRC-16 after each `Receipt` and `IndexEntry`. Validate on read. Adds ~2% overhead. Backward-compatible: old files read as CRC-0 (skip check).

3. **Physical deletes on Archive.** Compact `.DAT` on `Archive()` instead of just renaming files. Removes logically-deleted records.

4. **Statistics snapshot at every boundary.** Not just flush-on-request — auto-flush `.STA` when `Update()` detects a period boundary.

5. **Endianness marker in FILE_HEADER.** A magic byte (`0x0E 0x0B` or similar) lets a future cross-platform reader detect byte-order mismatches.

**Effort:** 3-5 days. **Risk:** Low. Backward-compatible with existing .DAT/.STA files.

### Proposal B: Abstract storage layer with pluggable backends (medium risk)

Introduce a virtual `IStorage` interface so the receipt lifecycle is decoupled from the binary format:

```
ReceiptStorage   ← interface (Add/Get/Update/Delete/Enum/Repair)
  ├─ FlatFileStorage    ← current binary format (default)
  └─ MiniDBStorage     ← B+tree on .db (implemented)
```

This is the same pattern used to decouple `RT_ENGINE` / `DEMO_ENGINE` in the Template Method hierarchy. Benefits:

- Testable: mock `ReceiptStorage` in unit tests without touching real files
- Observable: a logging proxy between interface and implementation

**Effort:** 5-8 days. **Risk:** Medium. Touches every receipt operation in control/engine.

### Proposal C: Replace with embedded SQLite ~~(high effort, high payoff)~~ **REJECTED**

~~Port receipt storage + statistics to SQLite (via `sqlite3` amalgamation compiled under Open Watcom). One `.db` file replaces `.DAT` + `.IDX` + `.STA`:~~

~~- Atomic transactions — no .DAT/.IDX/.STA inconsistency possible~~
~~- Real time-series queries — "revenue last 3 hours" without Repair()~~
~~- Indexed — no IndexCache, no batch-size tuning~~
~~- Schema migration — `ALTER TABLE` replaces zero-init Dummy fields~~
~~- CRUD is ACID, not logical-delete-and-repair~~

~~**Statements the current logic would map to:**~~

~~```sql~~
~~-- Receipts (replaces .DAT + .IDX)~~
~~CREATE TABLE receipts (~~
~~    number      INTEGER PRIMARY KEY,~~
~~    tag         INTEGER NOT NULL,~~
~~    stat        INTEGER NOT NULL,~~
~~    date        INTEGER NOT NULL,~~
~~    time        INTEGER NOT NULL,~~
~~    booth       INTEGER NOT NULL,~~
~~    city        TEXT,~~
~~    phone       TEXT,~~
~~    elapsed     INTEGER,~~
~~    ceil_min    REAL,~~
~~    percent     INTEGER,~~
~~    value       REAL NOT NULL,~~
~~    tax         REAL,~~
~~    tax2        REAL,~~
~~    created_at  TEXT DEFAULT (datetime('now'))~~
~~);~~
~~
~~-- Statistics (replaces .STA)~~
~~CREATE TABLE statistics (~~
~~    period_type INTEGER NOT NULL,  -- YEAR/MONTH/WEEK/DAY/TURN~~
~~    period_key  TEXT NOT NULL,      -- '2026', '2026-06', '2026-W25', etc.~~
~~    service     INTEGER NOT NULL,   -- TEL/FAX/etc.~~
~~    receipts    INTEGER DEFAULT 0,~~
~~    talk_min    REAL DEFAULT 0,~~
~~    paid_min    REAL DEFAULT 0,~~
~~    value       REAL DEFAULT 0,~~
~~    tax         REAL DEFAULT 0,~~
~~    PRIMARY KEY (period_type, period_key, service)~~
~~);~~
~~```~~

~~**Effort:** 15-20 days. **Risk:** High. Requires Open Watcom toolchain + sqlite3 amalgamation compile. Crunch time if the goal is "working product" vs "learning exercise."~~


**Decision:** MiniDB (implemented below) provides the same benefits — atomic commit, indexed lookups, single `.db` file — without the SQLite dependency and Open Watcom toolchain requirement. SQLite is not needed.
### Proposal D: CSV/JSON as canonical + binary cache ~~(exotic)~~ **REJECTED**

~~Store receipts as append-only CSV lines (one line = one receipt). Periodically rebuild a binary index. SQLite already does this better — not recommended.~~

**Decision:** MiniDB provides structured, indexed storage without the complexity of maintaining a separate binary index over CSV. CSV is not needed.

---

## Files Touched by Each Proposal

### Phase 1 — Structural hardening
- `src/db/dstorage.cpp` — compute/write/verify CRC; compact on Archive
- `src/db/dstatist.cpp` — auto-flush on period boundary
- `include/filehdr.h` — CRC-32 field (already exists, just populate it)
- `src/db/filehdr.cpp` — compute CRC on header write
- `include/receipt.h` — add CRC-16 trailer (optional extra field)
- `include/dstorage.h` — new `IndexEntry` field for CRC

### Phase 2 — Abstraction
- `include/istorage.h` — new `IReceiptStorage` / `IStatisticsStorage` interfaces
- `src/db/flatfile_storage.cpp` — current impl moved behind interfaces
- `src/db/mock_storage.cpp` — in-memory impl for testbeds
- `src/db/db_eng.cpp` — wire through factory instead of owning concrete
- `include/db_eng.h` — replace `DB_STORAGE *` with `IReceiptStorage *`

---

## MiniDB: Embedded C89 Database Backend

MiniDB is a back-pocket replacement for FlatFileStorage that implements the
same `IReceiptStorage` / `IStatisticsStorage` interfaces but with a single
`.db` file, B-tree index, and rename-based atomic commit. It is **feasible**
under BCC 3.1 / Phar Lap 286 — every required language and OS feature is
available.

### Feasibility check

| Requirement | Available in BCC 3.1 / Phar Lap? | Notes |
|---|---|---|
| `long` (32-bit) | YES | Receipt numbers and seek positions fit in 32 bits. MAX_RECEIPTS = 100M < 2^31 |
| `double` (64-bit) | YES | Currency and tariff values stored as-is |
| `malloc`/`new` for page cache | YES | Phar Lap heap accessible via `malloc` (extended DOS, up to 16 MB) |
| POSIX `open`/`read`/`write`/`lseek` | YES | Phar Lap provides all DOS file I/O via `sys/types.h` / `sys/stat.h` / `fcntl.h` |
| `rename()` atomic on same volume | YES | DOS `rename()` is file-system-atomic on FAT32 and NT redirectors |
| `unlink()` | YES | Standard |
| No `long long` needed | YES | 32-bit `long` wraps at 2.1B; receipts max out at 100M |
| No `off_t` needed | YES | `lseek` takes `long`, file size < 2 GB |

### What MiniDB is not

MiniDB is **not** a relational database. There are no SQL strings, no query
planner, no schemas beyond a fixed set of record types. It is a **page-oriented
key-value store with B+tree indexing** — roughly the equivalent of a bespoke
Berkeley DB (dbm) for the 16-bit DOS target.

### Architecture

```
MiniDB (.db file)
├── Page 0:  DB Header (magic, page_size, root_page, sequence counter)
├── Page 1+: B+tree leaf pages (ordered receipt index)
├── Page N:  B+tree internal pages
├── Page M:  Statistics pages (fixed-location rows)
└── Tail:    Write-ahead area (rename-based commit frontier)
```

```
IReceiptStorage   ← interface (already exists)
  ├─ FlatFileStorage  ← existing, hardened (Phase 1)
  └─ MiniDBStorage   ← C89 B+tree on .db

IStatisticsStorage ← interface (already exists)
  ├─ FlatFileStatistics  ← existing, hardened
  └─ MiniDBStatistics   ← inline in same .db
```

### Page format (512 bytes = 1 DOS sector)

```
PAGE:
  OFFSET  SIZE  FIELD
  0       2     Magic (0xDBDB)
  2       2     PageType (0=internal, 1=leaf, 2=stats)
  4       2     NumEntries
  6       2     FreeSpace (offset of next free byte)
  8       2     RightSibling (page number, 0 = none)
  10      6     Reserved
  ─────────────────────────────────
  16     496    Payload (keys + data pointers)

LEAF ENTRY (each):
  key:     4 bytes (long receipt number)
  booth:   2 bytes (int)
  seek:    4 bytes (page number within .db, not file offset)
  size:  106 bytes (Receipt minus MagicNumber, inline?)
          → OR: 10 bytes (key + page ref), Receipt on dedicated data pages
```

**Two storage strategies for receipts:**

*In-row:* Store the full 111-byte `Receipt` directly in the leaf page entry.
~3-4 receipts per 512-byte page (after header + entry overhead). B-tree is
wider but data is single-seek.

| Receipts | Pages (in-row) | Pages (ref-only) |
|---|---|---|
| 1,000 | ~330 | 6 (refs) + 130 (data) |
| 10,000 | ~3,300 | 51 (refs) + 1,300 (data) |
| 100,000 | ~33,000 | 501 (refs) + 13,000 (data) |

*Ref-only:* Store just (key, booth, page#) in leaf entries. Receipts live on
dedicated 16-receipt data pages. 2 seeks for lookup (index → data), but the
index is much smaller — stays cached in memory longer. **Recommended.**

### Statistics storage

Allocate one dedicated page (or a small block of pages at a known page number)
for the five period accumulators. Stats are fixed-sized — `DS_ENTRY[5]` +
`DS_DOUBLEPRNENTRY[2]` + `DS_CELLULARENTRY[5]`. On `Flush()`, write the
in-memory copy to the stats page. On `Update()` boundary, write.

### Atomic commit sequence

MiniDB avoids torn-write corruption without a write-ahead log by using
**rename-based commit**:

1. Open `.db.new` for writing
2. Serialize every dirty page (header, changed index pages, stats page,
   new/receipt data pages) to `.db.new`
3. `Flush()` + `close()` `.db.new`
4. `rename(".db.new", ".db")` — atomic on DOS FAT32
5. Delete old `.db` (already replaced by the rename)

If the app crashes during step 2: `.db.new` is partial (harmless), `.db` is
untouched. Recovery: delete `.db.new` on next open.

If the app crashes during step 4: `.db.new` is complete, `.db` may or may not
have been replaced. Recovery: delete `.db`, rename `.db.new` → `.db`.

### B-tree parameters

| Parameter | Value | Rationale |
|---|---|---|
| Page size | 512 bytes | DOS sector size — single `read()`/`write()` |
| Order (max keys) | 50 | Leaf: 50 × 10-byte entries = 500 bytes ≤ 496 payload |
| Min keys | 25 | = order/2 |
| Split strategy | First-fit, 50/50 split | Simplest, no rebalancing logic needed |
| Cache | LRU, 128 pages (64 KB) | Stays within ~50 KB for 10K records (ref-only) |
| File growth | Append-only (no free-page list) | Simplifies code; compaction on Archive |

### DB_ENGINE factory change

Current (after Phase 2):
```cpp
DB_ENGINE() {
    DBStorage    = new DB_STORAGE(...);
    DBStatistics = new DB_STATISTICS(...);
}
```

MiniDB:
```cpp
DB_ENGINE() {
    DBStorage    = new MiniDBStorage("rx.db");
    DBStatistics = new MiniDBStatistics("rx.db"); // same file
}
```

Or via a config flag:
```cpp
DB_ENGINE() {
    if (g_cfg->MINIDB) {
        DBStorage    = new MiniDBStorage("rx.db");
        DBStatistics = new MiniDBStatistics("rx.db");
    } else {
        DBStorage    = new DB_STORAGE(...);
        DBStatistics = new DB_STATISTICS(...);
    }
}
```

### Implementation plan

#### Phase M1: Infrastructure (3-4 days)

| Step | File | What |
|---|---|---|
| 1 | `include/minidb/page.h` | `PageType`, `PageHeader` struct, slot layout |
| 2 | `include/minidb/btree.h` | B+tree split/insert/search/delete signatures |
| 3 | `src/db/minidb/btree.cpp` | B+tree core: insert, search, split, compact |
| 4 | `src/db/minidb/cache.cpp` | LRU page cache (128-slot, page-fault → disk read) |
| 5 | `src/db/minidb/commit.cpp` | Atomic commit sequence (write dirty → flush → rename) |
| 6 | `src/db/minidb/recover.cpp` | Startup recovery (delete orphaned `.db.new`) |

#### Phase M2: Storage implementation (5-7 days)

| Step | File | What |
|---|---|---|
| 7 | `src/db/minidb/receipt_storage.cpp` | `IReceiptStorage` impl over B+tree + data pages |
| 8 | `src/db/minidb/statistics.cpp` | `IStatisticsStorage` impl over fixed stats pages |
| 9 | `include/minidb_storage.h` | `MiniDBStorage : IReceiptStorage` |
| 10 | `include/minidb_statistics.h` | `MiniDBStatistics : IStatisticsStorage` |

#### Phase M3: Integration (2-3 days)

| Step | File | What |
|---|---|---|
| 11 | `src/db/db_eng.cpp` | Factory: config flag selects MiniDB vs FlatFile |
| 12 | `src/cfg.cpp` or `.ini` | Add `MINIDB=1` config key (default 0) |
| 13 | `util/migrate/` | One-shot converter: .DAT+.IDX → .db |
| 14 | Build + test | Build under all variants; run in DOSBox-X |

**Total estimated effort: 10-14 days** (one developer, BCC 3.1 target).

### Should we do it?

| Consideration | Verdict |
|---|---|
| Is it technically feasible? | **Yes.** BCC 3.1 can do everything required. |
| Will existing data migrate? | Yes — one-shot converter reads .DAT+.IDX, writes .db. |
| Is it backward-compatible? | Yes — switchable via MINIDB=0|1 in config. FlatFile is still default. |
| Does it fix the biggest remaining risk? | The Phase 1 CRC already closes the integrity gap. MiniDB adds atomic commit (replaces STM2 for recovery) and O(log n) lookups. |
| Does it add maintenance burden? | Yes — another bespoke format. The current FlatFile is simple to debug by hexdump; MiniDB's paged format is not. |
| Is it worth the effort vs. value? | The current system works for thousands of calls per turn. MiniDB only pays off if data volumes grow significantly or crash recovery without STM2 becomes critical. |

**Recommendation:** Implement only if either:
1. DOSBox-X users need crash-proof operation without STM2 emulation, OR
2. Receipt volumes exceed ~50,000 per turn (where flat index cache becomes slow)

Otherwise, the Phase 1 CRC + auto-flush + compact Archive already give integrity
equivalent to a simple DB without the maintenance cost of a paged B-tree format.
The abstraction layer (Phase 2) means MiniDB can be dropped in later with zero
caller changes — the interface seam is already cut.
