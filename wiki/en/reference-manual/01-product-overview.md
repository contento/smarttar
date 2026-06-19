---
title: "Product Overview"
lang: en
manual: reference-manual-en
order: 1
---

# Product Overview
SmartTar is a real-time telephone tariff management system designed for public telephone booth centers (*cabinas telefónicas*). It runs under MS-DOS 6.22 in 286 protected mode via the Pharlap DOS extender, and manages the complete call lifecycle from off-hook detection through tariff calculation, receipt printing, and transaction archival.

SmartTar was developed by MicroDiseño Ltda. (Gonzalo Contento Castañ) starting in July 1993. Version 2.34 supports up to 32 telephone booths across 4 hardware clusters of 8 booths each.

## Key Capabilities

- **Real-time call metering.** Monitors up to 32 telephone booths simultaneously. Each booth is polled in real time for hook state, DTMF digits, and answer signals.
- **Automatic call classification.** Resolves the dialed number prefix against a configurable numbering plan to determine the call type: local, national long-distance (DDN), international (DDI), border zone, cellular, or special service.
- **Tariff engine.** Applies time-of-day and day-type schedules to compute the call cost. Supports up to 16 DDN tariff bands and up to 20 DDI tariff bands, with up to 6 schedule slots and 3 day types (normal, Saturday, holiday).
- **Holiday calendar.** Configurable per year; used by the tariff scheduler to switch to holiday tariff rates on designated dates.
- **Receipt printing.** Issues numbered, tax-inclusive (IVA) receipts through a plug-in printer driver system. Supports 18-, 28-, 40-, and 80-column printers including thermal models.
- **Transaction database.** Indexed file store (RX.DAT / RX.IDX) with duplicate-call detection and multi-turn archiving.
- **Prepaid magnetic card support.** Up to 4 card denominations configurable.
- **Modem integration.** Remote reporting and configuration synchronization via the SmartTar Communicator (STC) companion application.
- **External booth display.** Optional serial customer-facing LED display per cluster.
- **Statistics module.** Daily, weekly, monthly, and annual call and revenue summaries.
- **Hardware copy protection.** Parallel-port EEPROM dongle (bypassable only in demo builds).

## Product Variants

**SmartTar Standard.** Supports public telephone booths only. Receipt database stores calls by booth number.

**SmartTar Pro.** Extends the standard edition with PBX extension line billing. Each booth can be designated as either a public booth or an internal extension. Extensions receive a configurable discount percentage and may be billed for installation and monthly line costs. A separate receipt database (RXX.DAT / RXX.IDX) records extension calls.
