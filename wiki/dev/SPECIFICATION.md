# SmartTar — Software Requirements Specification

> **Version:** 2.97.0  
> **Status:** In production. This document reconstructs the functional and architectural specification from the existing codebase, as if developing from scratch.  
> **Domain:** Colombian public telephone booth billing (*cabinas telefónicas*)

---

## 1. Introduction

### 1.1 Purpose

SmartTar is a real-time point-of-sale system for public telephone booth operators. It monitors up to 32 booths across 4 clusters (8 booths each), classifies calls by destination, applies configurable tariff schedules (time-of-day, holidays, weekend), prints tax-inclusive receipts, maintains a full transaction database, and produces per-turn/per-day/per-month statistics.

### 1.2 Target Hardware

| Component | Requirement |
|-----------|-------------|
| CPU | 286 or compatible (runs Pharlap 286 protected mode) |
| RAM | 4 MB minimum, 32 MB recommended |
| Storage | Hard disk with DOS filesystem |
| Display | VGA/BGI-compatible (SVGA_S3 under DOSBox-X) |
| Ports | 1 parallel port (printer), 1+ serial ports (booth clusters, modem, external display) |
| Dongle | Parallel-port copy-protection dongle (production builds) |

### 1.3 Operating Environment

- **OS:** DOS 6.22
- **Execution:** Pharlap 286 DOS extender v3.0 — 286 protected-mode instruction set, segmented memory model
- **UI:** Zinc Interface Library 3.5 (BGI graphics)
- **Build:** Borland C++ 3.1 + Borland MAKE 3.6 + Turbo Assembler + TLINK

### 1.4 Terminology

| Term | Definition |
|------|------------|
| Booth (*cabina*) | A payphone station, one of up to 32 |
| Cluster | A group of 8 booths sharing one I/O port block (0x10 ports) |
| Turn (*turno*) | An operator shift; produces one receipt database + one statistics file |
| Receipt (*recibo*) | A single call record with classification, timing, value, and tax |
| DDN | *Discado Directo Nacional* — national long-distance call |
| DDI | *Discado Directo Internacional* — international long-distance call |
| EDA | *Empresa de Acueducto* — a specific Colombian operator variant |
| CEIL | Ceiling rounding: minimum billable time unit (e.g., round up to 1 min) |
| IVA | *Impuesto al Valor Agregado* — Colombian VAT |

---

## 2. System Architecture

### 2.1 Subsystem Overview

```
┌─────────────────────────────────────────────────────────────┐
│                        UI (Zinc 3.5)                        │
│  ┌─────────┐ ┌───────────┐ ┌──────────┐ ┌───────────────┐  │
│  │  View    │ │ Toolbars  │ │ Menus    │ │ Widgets       │  │
│  │ (booth   │ │ (tb_*.cpp)│ │ (mb_*.cpp)│ │ (w_phone,    │  │
│  │  grid)   │ │           │ │          │ │  w_table, …) │  │
│  └────┬─────┘ └───────────┘ └──────────┘ └───────────────┘  │
│       │            Event Bus (Zinc UI_EVENT_MANAGER)        │
│       ▼                                                     │
├─────────────── CONTROLLER (ctrl/control.cpp) ───────────────┤
│  Event dispatch, print orchestration, refresh loop          │
├───────┬──────────┬───────────┬──────────────────────────────┤
│       │          │           │                              │
│       ▼          ▼           ▼                              │
│  ┌────────┐ ┌──────────┐ ┌──────────┐ ┌────────────────┐   │
│  │ENGINE  │ │CFG       │ │DB_ENGINE │ │SPOOLER         │   │
│  │(RT or  │ │(config)  │ │(receipt  │ │(printer queue) │   │
│  │ DEMO)  │ │          │ │ store,   │ │                │   │
│  │        │ │          │ │ stats,   │ │                │   │
│  │        │ │          │ │ archive) │ │                │   │
│  └───┬────┘ └──────────┘ └──────────┘ └────────────────┘   │
│      │                                                      │
│      ▼                                                      │
│  ┌────────┐                                                  │
│  │PH_ENG  │←── Telephony data (.inf files → ph_info.dat)    │
│  │(tariff,│                                                  │
│  │ place, │                                                  │
│  │ query) │                                                  │
│  └────────┘                                                  │
└──────────────────────────────────────────────────────────────┘
```

### 2.2 Engine Subsystem (Call Lifecycle)

The engine manages the booth hardware finite-state machine. Two implementations exist:

- **RT_ENGINE**: Production — reads hardware ports via `inportb`/`outportb` at `0x280+`, driven by IRQ0
- **DEMO_ENGINE**: Development/training — software call generator with Poisson arrivals, no hardware needed

**Booth FSM:**

```
ONHOOK → OFFHOOK → (BREAK/MAKE/INTERDIG)* → ANSWER → TALK → STORE → ONHOOK
                ↘ DIALERR/COMERR → ONHOOK
```

Each tick the ISR polls all clusters. The FSM transitions: dial pulses or DTMF tones are accumulated into a phone number buffer; once digits match a known call type and enough digits are collected, the engine sets `ANSWER` state; talk time accrues until on-hook triggers `STORE`, which enqueues a `DynamicReceipt` for the controller.

### 2.3 Database Engine

- **Current turn:** `IReceiptStorage *DBStorage` + `IStatisticsStorage *DBStatistics`
- **Archived turn:** `IReceiptStorage *ArcDBStorage` + `DB_STATISTICS *ArcDBStatistics`
- **Extensions:** `DB_STORAGE *DBExtStorage` + `DB_EXT_STATISTICS *DBExtStatistics`

Two storage backends implement `IReceiptStorage`:

| Backend | Implementation | When |
|---------|---------------|------|
| FlatFile | `DB_STORAGE` (`.dat` + `.ndx`) | `MINIDB=0` in config |
| MiniDB | `MiniDBReceiptStorage` (B-tree pages) | `MINIDB=1` in config |

Statistics are stored via `IStatisticsStorage` (`DB_STATISTICS` for FlatFile, `MiniDBStatistics` for MiniDB).

---

## 3. Functional Requirements

### 3.1 Call Classification

The system MUST classify each call by:
1. **Access code** dialed (0 for operator, 9 for outside line, etc.)
2. **Destination digits** — matched against a configurable numbering plan (`ph_info.dat`)
3. **Call type** — one of: LOCAL, SPECIAL, CELLULAR, DDN (national long-distance), DDI (international), BORDER

#### 3.1.1 Digit Collection

- Pulse dialing: BREAK/MAKE timing per config (`T_BREAK`, `T_MAKE_T_MAKE_MARGIN`)
- DTMF: digit flags read from port `PO_DTMF_FLAGS` + `PO_DTMF_DIGITS` (4 ports × 2 digits each)
- Inter-digit timeout (`T_INTERDIG`/`T_DTMF_INTERDIG`) triggers number submission
- An `on-hook` signal before answer aborts the call (no receipt)

#### 3.1.2 Numbering Plan (.inf files)

The numbering plan lives in three `.inf` source files compiled into `ph_info.dat`:

| File | Content |
|------|---------|
| `ddi.inf` | International destinations (country codes → city, tariff, percentage) |
| `ddn.inf` | National long-distance destinations (area codes → city, tariff) |
| `local.inf` | Local numbers, special services, prefixes |

Each `.inf` line: `CITY : TARIFF_NUM : MINUTES% : PHONE1 PHONE2 ...`

### 3.2 Tariff Engine

#### 3.2.1 DDN Tariffs (National)

- Configuration: `DDN_TAX` percentage, `DEFAULT_TARIFFS` (A1–A9) values
- Schedules: `DDN_SCHEDULE` — per day type (Mon–Fri / Weekend / Holiday), time ranges with percentage multipliers
- Ceiling rounding: `CEIL_NAL` (min rounding unit for national calls, e.g. 0.25 → rounds to nearest quarter-minute)
- Minimum charge: `MIN_NAL` (minimum billable minutes)

#### 3.2.2 DDI Tariffs (International)

- Configurable reduced schedules per destination group (USA / Other / Border / Submarine)
- `APPLY_DDI_SCHEDULE` flag to enable/disable time-of-day rates
- `MIN_INTER`, `CEIL_INTER` for minimum/ceiling

#### 3.2.3 Cellular

- Separate tariff with `CELLULAR_TAX` percentage override
- `MIN_CELLULAR`, `CEIL_CELLULAR`

#### 3.2.4 Pricing Formula

```
rawMin      = elapsedTime / 60000.0          // ms → minutes
ceilMin     = ceil(rawMin / CEIL) * CEIL      // ceiling rounding
value       = ceilMin * tariffValue           // base cost
valuePerMin = tariffValue                     // per-minute display
paidPercent = scheduleLookup(date, time, callAttr)
value      *= paidPercent / 100               // apply time-of-day discount
tax         = value * taxPercent / 100        // VAT
```

### 3.3 Receipt Management

#### 3.3.1 Receipt Structure

```
┌──────────────┬──────────┬──────────────────────────────────────────┐
│ Field        │ Type     │ Description                              │
├──────────────┼──────────┼──────────────────────────────────────────┤
│ MagicNumber  │ UINT     │ Record validity marker (0x6719)          │
│ Number       │ long     │ Sequential receipt number                │
│ Tag          │ enum     │ TEL / SPECIAL_TEL / TELEX / FAX / CARD   │
│              │          │ / OTHER                                  │
│ Stat (bitfield):                                                  │
│  Cooked      │ bit      │ Engine populated raw; controller cooked  │
│  Manual      │ bit      │ Operator-entered manually                │
│  Printed     │ bit      │ Sent to printer                          │
│  Archived    │ bit      │ Moved to archive storage                 │
│  Paid        │ 4 bits   │ Payment status (enum: PENDING/PAID/etc)  │
│  CallAttr    │ 6 bits   │ Attributes (NOT_INCLUDED, DIALERR, etc)  │
│  Extension   │ bit      │ Is extension line (SmartTar Pro)         │
│  Deleted     │ bit      │ Marked for deletion                      │
│ ExtendedStat │ UCHAR    │ Reserved for future use (nonProcessed)   │
│ Date         │ int      │ Packed date (YYYYMMDD)                   │
│ Time         │ int      │ Packed time (HHMMSS)                     │
│ BoothNumber  │ int      │ Booth ID (0–31)                          │
│ City         │ char[21] │ Destination city name                    │
│ Phone        │ char[17] │ Dialed phone number / field union        │
│ Amount       │ int      │ Pulses / units / tariff code             │
│ ElapsedTime  │ long     │ Call duration in milliseconds            │
│ ValuePerMin  │ double   │ Tariff per minute (or UnitaryValue)      │
│ CeilMin      │ double   │ Ceiling-rounded minutes (or Base charge) │
│ Percent      │ int      │ Billed percentage (time-of-day discount) │
│ Value        │ double   │ Total billable value (including tax)     │
│ Tax          │ double   │ Primary tax (VAT)                        │
│ Tax2         │ double   │ Secondary tax (v2.30+, may be 0.0)       │
│ DDummy       │ double   │ Reserved for future expansion            │
└──────────────┴──────────┴──────────────────────────────────────────┘

**Invariant:** Exactly 111 bytes (enforced by `static_assert` in receipt.h).
This 111-byte layout is legacy on-disk invariant; both FlatFile and MiniDB backends store it verbatim.
```

#### 3.3.2 Receipt Lifecycle

**Raw Receipt → Cooked Receipt → Printed → Stored**

```
[HARDWARE / ENGINE]
  Call completes → Booth ISR generates raw Receipt
    (MagicNumber=0x6719, STAT.Cooked=0, fields set from call data)
    
[ENGINE → CONTROLLER]
  Raw Receipt enqueued → DynamicReceipt wrapper created
    (adds non-storable fields: Area, Total, PreValue, MoneyBack, Attr flags)
    
[CONTROLLER]
  DynamicReceipt processed:
  1. Classify call (access code, digit match against ph_info.dat)
  2. Apply tariff schedule (time-of-day, holiday, weekend discounts)
  3. Calculate value + tax
  4. Set STAT.Cooked=1
  5. Enqueue to SPOOLER (print)
  
[DB_ENGINE]
  Controller calls DBStorage::Add(receipt)
  → Backend (FlatFile or MiniDB) writes binary receipt
  → Receipt assigned sequential Number (from counter, auto-increment)
  → MagicNumber verified (0x6719 = valid)
  
[SPOOLER]
  Format receipt via printer DLL (pr_*.dll)
  → Output to port (parallel, serial, PDF, modem)
  → Set STAT.Printed=1
  → Flush to printer
  
[STATISTICS ENGINE]
  DBStatistics::Add(receipt) aggregates:
  → Count by call type (TEL/SpecialTel/FAX/etc)
  → Accumulate TalkMin, PaidMin, Value, Tax by destination
  → Track DialErrors, ComErrors
  
[TURN CLOSE]
  Archive files:
  → Current DB (RX.DAT + RX.IDX or RX.DB) → RX_YYMMDD_N.DAT
  → Current stats (RX.STA) → RX_YYMMDD_N.STA
  → Reset receipt counter (optional)
  → Create fresh empty DB + stats
```

**DynamicReceipt** (non-persistent wrapper):
- Wraps a `Receipt` object
- Adds runtime-only fields: `nAttr_` (flags), `Area_`, `Total_`, `PreValue_`, `MoneyBack_`, `bFromTurn`
- Used during classification and pricing; discarded after storage
- Enables separation of "raw from engine" and "cooked by controller" without modifying the 111-byte Receipt

#### 3.3.3 Receipt Numbering

- Sequential counter `N_RECEIPT` in config
- Supports leading zeros, configurable digit count, label prefix
- Stored per-turn; rollover at turn boundary
- Resettable via password-protected config

### 3.4 Statistics

#### 3.4.1 Aggregation Levels

| Level | Scope | Key |
|-------|-------|-----|
| Turn | Single operator shift | `.STA` file |
| Day | Calendar day | Sum of turns |
| Week | ISO week | Sum of days |
| Month | Calendar month | Sum of weeks |
| Year | Calendar year | Sum of months |

#### 3.4.2 Statistics Entry (`DS_ENTRY`)

Per aggregation level, the system tracks:

**By call type:**
- TEL.Nal (national), TEL.Inter (international)
- SpecialTel.Nal, SpecialTel.Inter
- Fax.Nal, Fax.Inter
- Internet.Nal
- Cards (prepaid magnetic card consumption)
- Other

**Per sub-item:** Receipts count, TalkMin, PaidMin, Value, Tax

**Special counters:** DialErrors, ComErrors, NotPaid, TollFree

**Cellular** tracked separately via `DS_CELLULARENTRY` (Tel + SpecialTel).

**Dual-printer stats** via `DS_DOUBLEPRNENTRY` for systems with two receipt printers.

### 3.5 Turn Management

#### 3.5.1 Turn Lifecycle

```
[ Close Turn ]
  │
  ├─ Archive current DB (move to archive directory)
  ├─ Archive current stats (move to archive directory)
  ├─ Reset receipt counter (if configured)
  ├─ Increment turn number
  └─ Create fresh empty DB + stats
```

- Turn number resets daily (`TURN_DAY`, `TURN_NUMBER`)
- Each turn produces: receipt DB (`.db`/`.dat` + `.ndx`), statistics (`.sta`)
- Archived turns loadable for historical queries

#### 3.5.2 Archive & Repair

- Archive: renames current data files with archive date/turn suffix
- Repair: validates index entries against data records, rebuilds if corrupted
- Manual `Repair` operation available from UI menu

### 3.6 Printer Subsystem

#### 3.6.1 Printer Drivers

Plug-in DLL architecture (`SPOOLER` + `pr_*.dll`):

| DLL | Columns | Type |
|-----|---------|------|
| PR_SR80 | 80 | Serial dot matrix |
| PR_DR80 | 80 | Dual serial |
| PR_LIN80 | 80 | Line printer |
| PR_DRPRE | 80 | Dual pre-formatted |
| PR_DR40 | 40 | Dual, 40-col |
| PR_SR40 | 40 | Serial, 40-col |
| PR_DR18 | 18 | Dual, 18-col |
| PR_SR28 | 28 | Serial, 28-col |
| PR_DREME | — | Epson thermal/emergency |
| PR_DRHAL | — | Half-size receipt |

Each DLL exports `WriteString`, `WritePChar`, `GetFormatName`. The spooler calls through function pointers loaded at runtime.

#### 3.6.2 Receipt Format

Configurable header lines (1–4), tax name display, receipt number, footer lines. Form selection via `P_FORM` config key.

#### 3.6.3 PDF Output (v2.80+)

When `P_PORT=pdf`, output is redirected to PDF 1.4 writer (`pdf_wr.c`):
- Receipts stacked on letter pages (~7 per page at LINEAL_80)
- Auto page-break on overflow
- File: `PDF\RXYYMMDD.pdf` (one file per day)
- ESC/P control codes stripped; uniform 8pt Courier
- xref table and trailer written on spooler shutdown

### 3.7 Config System

#### 3.7.1 Configuration Sources

| Source | Format | Usage | Location |
|--------|--------|-------|----------|
| `st.ini` | INI text (ISO-8859-1) | Loaded directly at runtime | `st/cfg/st.ini` (tracked in git) |
| `*.inf` | Text files (ISO-8859-1) | Telephony numbering plan, loaded at runtime | `st/cfg/` |
| `phones.csv` | CSV | Demo engine call generation | `st/cfg/` |

**Runtime Loading:**
1. Application loads `st.ini` directly at startup via `CFG::Load()`
2. `PH_ENGINE` loads `.inf` files directly (local.inf, ddn.inf, ddi.inf)
3. No compilation step needed — text files are parsed at runtime
4. Legacy compiled formats (`st.cfg`, `ph_info.dat`) exist in source tree but are NOT used

#### 3.7.2 Config Groups

| Group | Sections |
|-------|----------|
| System | Country, currency, company, tax rate, clusters, printer |
| Application | Call handling, receipt format, viewer, magnetic cards |
| Telephony | Timings, signal type, on-hook/off-hook thresholds |
| Dialing | Access codes, digit counts, area codes |
| Extensions | Extension lines (SmartTar Pro), discount, install cost |
| Critical | Pause key, false answer detection, spy mode |
| Modem | Port, init string, timeouts |
| Display | External booth display, serial config |
| Passwords | Backdoor, supervisor, user, operator, utility |

### 3.8 External Display

Optional RS-232 serial display on each booth. Shows:
- Call status (on-hook, dialing, talking)
- Elapsed time
- Call cost
- Default message when idle

Configurable via `DISPLAY_*` settings (port, baud rate, default message).

### 3.9 Modem Integration

Optional modem for remote reporting:
- Call progress monitoring
- Configurable dial string, speaker, delays
- Supports Greenleaf Communications Library types

### 3.10 Screen Saver

- Timeout configurable via `SS_TIME`
- Mode selectable via `SS_ID`
- Auto-activates after keyboard idle period

### 3.11 Prepaid Magnetic Cards

- Up to 4 card values configurable (`MCARDS`)
- Per-call deduction from card balance
- `GENERATE_PREPAID_RECEIPT` flag
- `MULTIPLE_PREPAID_CALLS` — allow multiple calls on one card
- `DOUBLE_PREPAID_RECEIPT` — print duplicate

---

## 4. Storage Formats

### 4.1 FlatFile Backend (`DB_STORAGE`)

Two files per database:
- `XXXX.dat` — fixed-size record data (111 bytes/receipt) with `DataHeader`
- `XXXX.ndx` — index entries: `{ MagicNumber, Number, BoothNumber, SeekPos }`

Index cached in memory (configurable `CACHE_SIZE`). Index entries stored sequentially; data records appended and never moved individually (compacted on repair).

### 4.2 MiniDB Backend

B-tree page-based storage via `MiniDBReceiptStorage` and `MiniDBStatistics`:

**Receipt Storage:**
- Fixed page size: 4096 bytes per page
- **Receipt data pages** store 4 receipts per page (111 bytes each, packed)
- **B-tree index** maps `(Number, BoothNumber)` tuples to `(pageNum, slot)` pairs
  - Encoding: `DataSeek = (pageNum << 8) | slot`, recoverable via bitmask
- Receipts append-only within data pages; no in-place record moves
- Atomic writes: changes staged to `.db.new`, then renamed to `.db` on commit

**Statistics Storage:**
- Dedicated **statistics anchor page** within the same `.db` file
- Stores **7 pages** per turn (multi-page buffer for `DS_ENTRY`, `DS_DOUBLEPRNENTRY`, `DS_CELLULARENTRY` arrays)
- Shared **MiniDBCache** between receipt storage and statistics (no separate cache overhead)
- In-memory copies of statistics arrays (cached from disk pages on load)

**Page Cache:**
- LRU eviction policy, configurable by `CACHE_SIZE` in config (default 1024 entries)
- Lazy loading: pages fetched on first access
- Flush on shutdown or explicit request

**File Structure:**
- `NAME.db` — single unified file containing:
  - Root page (metadata, B-tree root, stats anchor page number)
  - B-tree index pages (internal nodes, leaf nodes)
  - Data pages (receipt records)
  - Statistics pages (aggregated data for turn/day/week/month/year)

**Backend Selection:**
Set `MINIDB=1` (default in v2.97.0+) in `st.ini`/`st.cfg` to enable MiniDB. Set `MINIDB=0` to fall back to FlatFile storage (`DB_STORAGE`). Both backends implement `IReceiptStorage` and `IStatisticsStorage` interfaces; the choice is transparent to the controller.

### 4.3 File Header

Every data file starts with `FILE_HEADER` (v3.0):
```
ID[4]   = "GCC"
Title[38] = "SmartTar file 3.0"
Version = 0x03
CheckSum (CRC-32 placeholder)
Attrib  (COMPRESSED | CRYPTED | CHECKABLE)
AppVersion (MAJOR, MINOR, UPGRADE)
Serial  (20 bytes)
Time/Date of creation
```

---

## 5. Storage Backend Selection

As of v2.97.0, SmartTar supports two pluggable storage backends. Both implement identical `IReceiptStorage` and `IStatisticsStorage` interfaces; the choice is transparent to the application controller.

| Backend | Setting | Use Case | Recommendation |
|---------|---------|----------|-----------------|
| **FlatFile** | `MINIDB=0` | Legacy `.dat` + `.ndx` files | Stable, proven; use for production if stability critical |
| **MiniDB** | `MINIDB=1` (default) | B-tree page-based `.db` file | Default v2.97.0+; better scalability, unified file, concurrent access |

Set in `st.ini` `[Aplicacion]` section; restart app (no recompile needed).

## 6. Build Variants

| Variant | Define Flags | Behavior |
|---------|-------------|----------|
| **Production** (default) | `-DRUN` | Full dongle check via `DONGLE` class; RT_ENGINE only |
| **Demo** | `-DDEMO -DRUN -DNODONGLE` | No dongle check; DEMO_ENGINE (Poisson call generator) |
| **EDA** | `-DEDA -DRUN` | Colombian EDA operator call classification variant; requires dongle |
| **Debug** | `-DDEBUG -DRUN` | Debug symbols (`-v` flag); Pharlap TDP debugger support |

**Shorthand build commands** (from `st/` directory):
- `makedbg` — debug build with symbols
- `makedemo` — demo build (default for dev)
- `makeeda` — EDA production variant
- `makeprod` — production build with dongle check

---

## 7. Configuration File Reference

### 7.1 `st.ini` Section: [Sistema]

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| COUNTRY | string | "Colombia" | Installation country |
| CURRENCY | string | "$" | Currency symbol |
| CITY | string | "Medellin" | Installation city |
| COMPANY | string | — | Company operator name |
| ID | string | — | Company ID (NIT, etc.) |
| OPERATOR_NAME | string | — | Telecom operator name |
| TAX_NAME | string | "IVA" | Tax name displayed on receipt |
| TAX_PERCENT | float | 16.00 | Primary VAT percentage |
| CLUSTERS | int | 1 | Number of booth clusters (1–4); each cluster = 8 booths |
| ENGINE_KIND | string | "demo" | Engine type: "demo" or "real" (RT_ENGINE) |
| P_PORT | string | "pdf" | Printer port: "lpt1" / "com1" / "pdf" |
| P_FORM | string | "80 col. lineal" | Receipt format name (loaded from DLL) |
| P_OPERATION | string | "automatica" | Print operation mode |
| P_FOOTER1 / P_FOOTER2 | string | — | Optional footer lines on receipt |
| CASH | string | "prn" | Cash/payment tracking mode |
| M_ROUND | float | 50.00 | Manual entry rounding unit (currency) |
| MCARDS | string | — | Prepaid card values (comma-separated) |
| CELLULAR_TAX | float | 20.00 | Cellular call VAT percentage |
| INTERNET_TAX | float | 16.00 | Internet call VAT percentage |
| INTERNET_TARIFF | float | 800.00 | Internet flat charge per unit |
| USA | string | "Estados Unidos" | Name for USA country code |
| MINIDB | int | 1 | Use MiniDB backend (1=yes, 0=FlatFile) |
| SS_ID | int | 8 | Screen saver type ID |
| SS_TIME | int | 0 | Screen saver timeout (0=disabled) |
| COM | string | "1 2400 none 8 1" | Serial port config (port, baud, parity, bits, stops) |
| LPT | int | 1 | Parallel port number (1=LPT1, 2=LPT2) |
| DOUBLE_PRN | int | 0 | Dual printer support (1=enabled) |
| DEALER | int | 0 | Dealer mode flag |

### 7.2 Section: [Aplicacion]

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| MANUAL_ANSWER | int | 0 | Operator can force answer state (1=enabled) |
| MIN_NAL | int | 0 | Minimum billable minutes (national calls) |
| MIN_INTER | int | 1 | Minimum billable minutes (international calls) |
| MIN_BORDER | int | 1 | Minimum billable minutes (border calls) |
| MIN_USA | int | 1 | Minimum billable minutes (USA calls) |
| MIN_CELLULAR | int | 1 | Minimum billable minutes (cellular) |
| CEIL_NAL | float | 0.00 | Ceiling rounding unit (national); 0=disabled |
| CEIL_INTER | float | 1.00 | Ceiling rounding unit (international) |
| CEIL_BORDER | float | 0.00 | Ceiling rounding unit (border) |
| CEIL_USA | float | 1.00 | Ceiling rounding unit (USA) |
| CEIL_CELLULAR | float | 1.00 | Ceiling rounding unit (cellular) |
| N_RECEIPT | long | 0 | Current receipt counter (auto-increment per call) |
| N_DIAL_ERR | int | — | Dial error count (current turn) |
| N_COM_ERR | int | — | Communication error count (current turn) |
| MAX_DIAL_ERR | int | 4 | Max dial errors before booth lockout |
| MAX_COM_ERR | int | 4 | Max comm errors before booth lockout |
| DEFAULT_TARIFFS | string | — | A1–A9 tariff matrix (semicolon-separated) |
| APPLY_DDN_SCHEDULE | int | 0 | Apply time-of-day schedule to national calls |
| APPLY_DDI_SCHEDULE | int | 1 | Apply time-of-day schedule to international calls |
| TURN_DAY | int | — | Day of last turn close (YYMMDD packed) |
| TURN_NUMBER | int | 1 | Current turn number (resets daily) |
| CACHE_SIZE | int | 1024 | Index cache entries (for MiniDB page cache) |
| CHECK_DUPS | int | 1 | Check for duplicate receipt numbers (1=enabled) |
| VIEW_REFRESH_TIME | int | 500 | UI refresh interval (milliseconds) |
| VIEW_PHONE | int | 1 | Display dialed phone number in booth grid (1=yes) |
| VIEW_DECIMALS | int | 0 | Show decimal places in UI values (1=yes) |
| CORRECTION_TIME | int | 0 | Time correction offset (milliseconds) |
| GENERATE_PREPAID_RECEIPT | int | 1 | Print receipt for prepaid magnetic card use |
| DOUBLE_PREPAID_RECEIPT | int | 0 | Print duplicate receipt for prepaid (1=yes) |
| MULTIPLE_PREPAID_CALLS | int | 1 | Allow multiple calls on same prepaid card |
| CALL_ACTUAL_COST | int | 1 | Display actual cost (vs. displayed cost) |
| NO_SOUND_WHILE_SPY | int | 1 | Mute on-screen call tones in spy mode |
| ACTIVATE_RELAY | int | 1 | Activate relay signal on call answer |
| RELAY_NUMBER | int | 1 | Relay port number |
| RECNO_LEADING_ZEROS | int | 1 | Pad receipt number with leading zeros (1=yes) |
| RECNO_DIGITS | int | 8 | Receipt number field width (digits) |
| RECNO_LABEL | string | "Recibo" | Receipt label text on printed receipt |
| HEADER_LINE1 / LINE2 / LINE3 / LINE4 | string | — | Custom header lines (printed at top of receipt) |
| HEADER_PRINT_TAXNAME | int | 1 | Print tax name (IVA) on receipt (1=yes) |
| HEADER_PRINT_RECNO | int | 0 | Print receipt number in header (vs. footer) |

### 7.3 Section: [Telefonia]

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| T_ON_HOOK | int | 150 | On-hook detection time (ms) |
| T_OFF_HOOK | int | 300 | Off-hook debounce (ms) |
| T_ANSWER | int | 38000 | Answer detection timeout (ms) |
| T_TALK | int | 3000 | Minimum talk time before store (ms) |
| SIGNAL | string | "inversion" | Answer signal type |

### 7.4 Section: [Marcacion]

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| ACCESS | int | 0 | Outside-line access digit |
| NAL_DIGITS | int | 10 | Expected digits for national calls |
| LOCAL_DIGITS | int | 7 | Expected digits for local calls |
| CELLULAR_DIGITS | int | 10 | Expected digits for cellular calls |
| INTER_DIGITS | int | 14 | Expected digits for international calls |

---

## 8. UI Specification

### 8.1 Main Screen Layout

```
┌──────────────────────────────────────────────────────┐
│ [Toolbar: Config | Manual | Print | Tools | Help]   │
├──────────────────────────────────────────────────────┤
│ Menu: Archivo · Configuración · Informes · Utilidades│
│       Simulador · Extensiones · Ayuda               │
├──────────────────────────────────────────────────────┤
│ Booth Grid (4×8 max) — each cell shows:            │
│ ┌─────────────────────────────────────────┐         │
│ │ Booth # | Status | Phone | Time | Cost  │         │
│ │ [icon] city name / tariff info          │         │
│ └─────────────────────────────────────────┘         │
├──────────────────────────────────────────────────────┤
│ Status bar: receipts count, errors, time, date      │
└──────────────────────────────────────────────────────┘
```

### 8.1.1 Demo Mode Indicator

When running in demo mode (`ENGINE_KIND=demo`), the UI provides visual
indicators:

- **Background color**: Shifts from `GREEN` (production) to `CYAN` (demo)
- **Window title**: Shows `[DEMO]` prefix (e.g., `[DEMO] SmartTar - Company City`)
- **Exit dialog title**: Shows `[DEMO] SmartTar`
- **About dialog**: Build string appends `" Demo"` (e.g., `"2.98.0 Demo"`)

These indicators are set in `Prolog()` (background), `UIW_VIEW::UIW_VIEW()`
(main title), `Exit()` (exit dialog), and `UIW_ABOUT()` (about dialog).

### 8.2 Booth Cell Display

Each booth cell shows real-time:
- Booth alias/number
- Call status icon (idle, dialing, ringing, talking, error, locked)
- Dialed phone number (in progress or last called)
- Elapsed time
- Current cost
- Tariff/destination info

### 8.3 Menu Reference

| Menu | Items |
|------|-------|
| Archivo | Print receipt by number, Reprint last, Archive, Repair |
| Configuración | System config, Passwords, Holidays, Formats, Extensions |
| Informes | Turn report, Daily, Monthly, Yearly, Cellular, Operator |
| Utilidades | Calculator, Booth test, Modem, Setup |
| Simulador | Start/Stop, Pause/Resume (demo mode) |
| Extensiones | Extension management, Reports (SmartTar Pro) |
| Ayuda | Contents, About |

---

## 9. Error Handling

### 8.1 Error Categories

| Error | Cause | Handling |
|-------|-------|----------|
| Dial error | No dial tone / invalid number | Booth locked, receipt tagged `DIALERR`, counter incremented |
| Comm error | Line dropped mid-call | Booth locked, receipt tagged `COMERR` |
| NOT_INCLUDED | Number matched exclusion list | Call blocked before connect, no receipt |
| File corrupt | Disk write failure / bad sector | Repair tool, data recovery |
| No dongle | Production build without hardware key | Application refuses to start |
| Config error | Corrupt `st.cfg` | Falls back to hardcoded defaults |

### 8.2 Recovery

- On abnormal exit, `DB_ENGINE::Recover()` runs at next startup to reconcile pending receipts
- Emergency repair accessible from menu (password-protected)
- `mdbdump` / `lsmdb.exe` tools for offline data inspection

---

## 10. Security

### 10.1 Password Levels

| Level | Access |
|-------|--------|
| Backdoor | Universal master password |
| Supervisor | Full configuration access |
| User 1/2 | Operator-level config changes |
| Operator | View-only, extension services |
| Utility | Command-line tools |

### 10.2 Copy Protection

- Production builds require parallel-port dongle
- `CheckHardware()` probes for dongle presence at startup
- Demo builds skip dongle check (`__NO_DONGLE__`)
- Application serial number embedded via `SERIAL_NUMBER` at build time
- EEPROM stores serial and calibration data in production

---

## 11. Extensions (SmartTar Pro)

SmartTar Pro variant adds extension lines (secondary internal lines per booth) as billable endpoints:

- **Separate receipt counter:** `E_N_RECEIPT` (independent of main `N_RECEIPT`)
- **Per-extension tariffs:** Discount percentage applied to base tariff
- **Configuration:** Extension setup per booth in config UI
- **Call tracking:** `STAT.Extension` bit set for extension calls
- **Statistics:** Aggregated separately; reports can combine or isolate extension data
- **Method:** Extension digit detection via extended dialing code routing to secondary handler

**Status:** Active feature. Code path present in `ph_eng.cpp` (destination matching), `db_eng.cpp` (receipt tagging), and `cfg.h` (extension tariff table). UI support via `mb_conf` menu handlers.

---

## 12. Utilities & Tools

| Tool | Purpose | Location |
|------|---------|----------|
| `mdbdump` | Inspect/export MiniDB `.db` files (receipts + stats) | `st/util/lsmdb/mdbdump.exe` or `mdbdump.sh` / `mdbdump.ps1` at project root |
| `inf2dat` | Compile `.inf` files → `ph_info.dat` (legacy, not used at runtime) | `st/util/inf2dat/inf2dat.exe` |
| `ini2cfg` | Compile `st.ini` → `st.cfg` (legacy, not used at runtime) | `st/util/ini2cfg/ini2cfg.exe` |
| `SETUP.EXE` | Interactive config / install utility | `st/util/setup/setup.exe` |
| `defpwd` | Password reset utility | `st/util/defpwd/defpwd.exe` |
| `chkrx` | Receipt database checker / repair | `st/util/chkrx/chkrx.exe` |
| `gen` | Receipt generator (test data) | `st/util/gen/gen.exe` |

**mdbdump Details** (v2.97.0+):
- Reads both FlatFile (`.dat` + `.ndx`) and MiniDB (`.db`) formats
- Exports: `--csv-receipts`, `--csv-stats` for analysis
- Python (`mdbdump.py`) and C++ (`mdbdump.exe`) versions
- Supported by `mdbdump.sh` / `mdbdump.ps1` host-side launchers

---

## 13. Performance Requirements

| Metric | Requirement |
|--------|-------------|
| Booth polling latency | < 10 ms for all 32 booths |
| Receipt storage | ≤ 10 ms per `IReceiptStorage::Add()` |
| UI refresh | Configurable 100–1000 ms interval (default 500 ms) |
| Page cache | 1024 entries minimum (configurable via `CACHE_SIZE`) |
| Max receipts per turn | 999,999 (limited by `long` counter field) |
| Max booths | 32 (4 clusters × 8 booths per cluster) |
| Index lookup time | O(log n) via B-tree (MiniDB) or linear scan (FlatFile cache) |

---

## 14. Constraints & Invariants

### Filesystem & Naming

- **DOS 8.3 filenames:** All filesystem operations MUST respect the 8.3 limit (8-char name, 3-char extension). DOS 6.22 does not support long filenames. This includes `.cpp`, `.h`, `.prj`, directories, and all build outputs.
- **Line endings:** Enforced via `.gitattributes` — git auto-converts at checkout/commit.
  - **CRLF** for DOS-toolchain-consumed files: `*.def`, `*.bat`, `*.cfg`, `*.ini`, `*.mak`, `MAKEFILE`, `*.c`, `*.cpp`, `*.h`, `*.hpp`, `*.asm`, `*.inf`, `*.txt`
  - **LF** for host-side files: `*.sh`, `*.md`, `*.conf`, `dosbox-x.conf`

### Compiler & Runtime

- **CPU:** 286 or compatible (executes Pharlap 286 protected-mode instructions)
- **Memory model:** Large model (`-ml` in `st.cfg`) — multiple code + data segments; all pointers implicitly `far`
- **286 protected mode via Pharlap:** Segmented memory, not flat. Use `PHAPI` for extended memory access.
- **Stack checking:** DISABLED (`-k-` flag in `st.cfg`) — do not re-enable; causes instability
- **Precompiled headers:** `stdst.h` → `st.sym` (`-H=st.sym` in `st.cfg`)
- **Always-on defines:** `__FHEADER=3`, `__DEBUG=0`, `__BTN__` in `st.cfg`

### Critical Definitions

- **`__BTN__` MUST NEVER be removed from `st.cfg`.** Activates custom `UIW_TBUTTON` / `UIW_GBUTTON` Zinc classes in `b_button.h` with height-offset trick. Without it, toolbar is misplaced and grid cell geometry is broken.

### Data Invariants

- **Receipt size:** Exactly 111 bytes. Enforced by `static_assert` in `receipt.h` line 159. This is a legacy on-disk invariant; both FlatFile and MiniDB backends store the raw 111-byte `Receipt` struct verbatim.
- **Magic number:** `0x6719` — validity marker for all receipts
- **Page size (MiniDB):** 4096 bytes per page; 4 receipts per data page (111 bytes × 4 = 444 bytes + 3652 bytes overhead/index)

### Encoding Dual-System

- **Application strings (Zinc UI):** ISO-8859-1 (Latin-1) — Zinc renders via bitmap font, bypasses DOS code page. E.g., `ñ` = `0xF1`, `ó` = `0xF3`
- **DOS channels (console, printer, file I/O):** CP850 (DOS code page). E.g., `ñ` = `0xA4`, `ó` = `0xA2`
- **Bridge function:** `_ISO2ASCII()` in `st_util.cpp` translates Latin-1 → CP850 when app code emits text through DOS channels
- **Editor configuration:** VSCode is set to `files.encoding: iso88591` in `.vscode/settings.json`
- **Tooling gotcha:** UTF-8-native tools (some Node formatters, smart IDEs) silently re-encode on save, replacing high-bit Latin-1/CP850 bytes with UTF-8 replacement character (`0xEF 0xBF 0xBD`). After any edit on a Latin-1/CP850 file, verify: `file <path>` should report **`ISO-8859 text`** or **`Non-ISO extended-ASCII text`**, never **`UTF-8 text`**. Repair with `perl -i -pe 's/\xef\xbf\xbd/\xf1/g'` (ñ in Latin-1) or `s/\xef\xbf\xbd/\xa4/g'` (ñ in CP850)
