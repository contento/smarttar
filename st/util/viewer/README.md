# VIEWER — Interactive Receipt Database Browser

**Purpose:** Full-screen Zinc UI application for browsing, searching, and exporting transaction receipts.

## Overview

VIEWER is a read-only transaction database explorer. Technicians use it to audit call records, verify charges, answer customer inquiries, and export selected transactions for billing or reconciliation.

**Features:**
- **Search:** By receipt number, booth, date, phone number, charge amount
- **Sort:** By receipt number, date, amount, duration, booth
- **View:** Full receipt details (caller, dialed number, duration, charge, operator, timestamp)
- **Export:** Selected receipts or filtered result sets to text/CSV
- **Filter:** Date range, booth, receipt type, amount range
- **Statistics:** Totals for selected receipts (count, revenue, duration)

**UI:** Zinc framework with table widget displaying transaction rows, search form, and export buttons.

**Performance note:** Loads entire database into memory; suitable for booths with <100K active receipts. Larger installations should use STL for batch reporting.

**Status:** Active. Customer service and auditing tool. Read-only—cannot modify transactions.

**References:**
- DB_VIEW class — src/db/db_view.cpp (table rendering, sorting, filtering)
- DB_ENGINE — src/db/db_eng.cpp (database access)
- Zinc UI — include/ui_win.hpp, UIW_TABLE widget
