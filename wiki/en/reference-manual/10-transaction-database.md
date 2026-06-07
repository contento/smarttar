---
title: "Transaction Database"
lang: en
manual: reference-manual-en
order: 10
---

# Transaction Database
## Record Structure

Each receipt record stored in RX.DAT includes:

- Receipt tag (TEL for telephony, EXT for extension).
- Booth number.
- Date and time of call start.
- Duration (seconds).
- Call type (local, DDN, DDI, cellular, border, special).
- Destination identifier (city/country name).
- Subtotal, tax amount, total cost.
- Receipt serial number.
- Paid / not-paid / toll-free flags.

## Statistics

DB_STATISTICS accumulates running totals from all records. Summaries are available at daily, weekly, monthly, and annual granularities. The TURN system supports multiple work shifts per day, each with its own statistics snapshot.
