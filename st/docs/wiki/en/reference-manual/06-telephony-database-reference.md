---
title: "Telephony Database Reference"
lang: en
manual: reference-manual-en
order: 6
---

# Telephony Database Reference
The telephony database is compiled from three source files by the SETUP utility into PH_INFO.DAT. It contains the numbering plan, tariff bands, and schedules for local, national long-distance (DDN), and international (DDI) calls.

## Source Files

- **LOCAL.INF.** Defines the local hometown numbering plan. Each entry maps a dialed prefix to a destination city name and call category. The sample file covers Antioquia, Colombia (434 entries).
- **DDN.INF.** Defines national long-distance (domestic) numbering plan and base tariff bands. Entries identify area codes, destination city names, and associated tariff indices.
- **DDI.INF.** Defines international telephony information. Entries identify country codes, country names, and associated DDI tariff indices.

## Tariff Structure

- **DDN tariffs.** Up to 16 tariff bands, each defining a per-minute rate for a destination group. Rates vary by time-of-day schedule (normal, reduced, super-reduced). Up to 6 schedule time slots per day type.
- **DDI tariffs.** Up to 20 tariff bands covering international destinations. Each band defines rates for normal, reduced, and super-reduced periods.
- **Local tariffs.** Defined per local zone using a configurable default tariff list (DEF_TARIFFS). Up to 9 named tariff levels (A1–A9).
- **Day types.** The scheduler distinguishes three day types: normal weekday, Saturday, and holiday. The holiday calendar is checked at call start to select the appropriate tariff set.
- **Schedules.** DDN and DDI schedules define up to 6 time windows per day type. APPLY_DDN_SCHEDULE and APPLY_DDI_SCHEDULE enable/disable schedule switching.

## Locked Numbers

PH_INFO.DAT also stores a list of locked (blocked) telephone numbers. Calls to locked numbers are intercepted before dialing completes. This is used to block premium-rate or unauthorized destinations.
