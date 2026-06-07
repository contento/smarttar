# Testbeds

Standalone test harnesses for verifying individual SmartTar subsystems
in isolation. These are manual diagnostic programs -- not automated
tests. Run them, interact with them, and visually/empirically confirm
the subsystem works correctly.

**Status:** Historical development artifacts. Kept for reference and
occasional manual verification. Not part of the main build.

## Directory Structure

### Core Subsystem Testbeds

| Directory | Subsystem | Type | Description |
|-----------|-----------|------|-------------|
| `db_eng/` | Database Engine | Interactive | Full menu-driven DB harness: insert, get, modify, list, repair, flush, archive records |
| `db_eng/dstorage/` | DB Storage Layer | Interactive | Tests the raw storage layer separately |
| `db_eng/dbcursor/` | DB Cursor | Interactive | Tests cursor-based iteration over DB records |
| `db_eng/enum/` | DB Enumeration | One-shot | Tests enumeration of DB records |
| `rt_eng/` | Real-Time Engine | Diagnostic | Live display of booth state (phones, timers, prepaid flags) |
| `ph_eng/` | Phone Engine | Interactive | Loads phone data, prompts for number entry |
| `spooler/` | Print Spooler | Interactive | Zinc UI app: creates window, types text, prints via spooler |
| `bdisplay/` | Booth Display | Interactive | Sends hardware commands to booth displays |
| `serial/` | Serial Port | One-shot | Sends 3 bytes to COM2 -- basic serial driver test |
| `cfg/` | Configuration | One-shot | Loads config, prints status, saves |
| `log/` | Logging | Interactive | Writes random log entries, reads them back, archives |
| `pr_fmt/` | Print Formatter | One-shot | Loads printer formatter DLL, prints format string |
| `dll/` | DLL Loading | One-shot | Tests dynamic DLL loading and function calls |
| `stm2/` | STM2 Data | Diagnostic | Prints memory layout/offsets of STM2 structures |
| `calendar/` | Calendar Widget | Interactive | Zinc UI calendar widget test |
| `testpwd/` | Password | One-shot | Encrypts password, prints hex output |

### Utility Testbeds

| Directory | Description |
|-----------|-------------|
| `st_util/ceil/` | Tests `_Ceil` integer ceiling function |
| `st_util/round/` | Tests `_Round` integer rounding function |
| `st_util/encrypt/` | Tests encryption routine |
| `st_util/chserial/` | Tests serial port reconfiguration |
| `st_util/patch/` | Tests binary patching |

### Experimental / Special Purpose

| Directory | Description |
|-----------|-------------|
| `trace/` | Intentional NULL dereference -- tests Phar Lap stack traces (supposed to crash) |
| `bt/` | Tests x86 bit-test/set/reset instructions (hardware mutex primitives) |
| `misc/` | Experiments with Borland's `BI_SDoubleListImp` template |

## Shared Files

| File | Purpose |
|------|---------|
| `test.cfg` | Shared BCC compiler configuration for all testbeds |
| `test.def` | Shared DEF file for Phar Lap executable linking |
| `test.cfg` (pr_fmt/) | Local config override for printer formatter tests |

## Building

Testbeds are built independently from the main SmartTar application.
Each subdirectory has its own `makefile` that references the shared
`test.cfg` and `test.def` via relative paths.

To build a specific testbed:
```dos
cd st\testbeds\<subsystem>
make
```

**Note:** Requires Borland C++ 286 compiler and Phar Lap DOS extender
(see main project build instructions).

## Encoding

All source files in this directory use **CP850** encoding (DOS code
page), not Latin-1. When editing in VSCode, use "Change File Encoding"
-> "DOS (CP 850)" before saving.

## History

These testbeds were written during SmartTar development (1993-2003 era)
by the original author (Gonzalo Contento, "GCC" in source comments).
They served as manual verification tools for each subsystem before
integration into the main application.

The directory was originally named `test/` and renamed to `testbeds/`
in 2026 to better reflect their nature as test environments rather
than automated test suites.
