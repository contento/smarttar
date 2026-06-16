# STL — SmartTar Tariff Batch Processor

**Purpose:** Command-line batch processor for bulk tariff updates, rate calculations, and data transformations.

## Overview

STL processes large tariff and transaction files through custom C++ plugins, enabling bulk operations that would be too slow or cumbersome in the interactive UI. It reads input files (RX.DAT, RX.LST), applies transformations (rate recalculation, format conversion), and writes results.

**Command line:**
```
stl [options] input_file output_file
```

**Features:**
- **Batch rate recalculation:** Recalculate charges on large transaction sets with new tariff tables
- **Format conversion:** RX.DAT ↔ RX.LST (binary ↔ text)
- **Data filtering:** Extract transactions by date range, booth, or call type
- **Statistics:** Generate summaries (total revenue, call counts, error logs)
- **Custom pipelines:** User-supplied C++ modules for domain-specific transforms

**Plugins:**
- `rxproces.cpp` — Transaction processing engine (parsing, filtering, aggregation)
- `rxdat.lst` — Definition file for input format
- `rxsta.lst` — Definition file for statistics output

**Included examples:**
- `1998/`, `1999/` — Historic processing jobs (reference implementations)
- `test.txt` — Sample data
- `leame.txt` — Spanish-language documentation

**Status:** Active. Backend processing tool for large-scale operations. Not end-user facing; vendor and technician use only.

**References:**
- RXPROCES — transaction processing pipeline (rxproces.cpp, rxproces.h)
- CFG and STM2 — configuration and session management
- DB_ENGINE — database access layer
