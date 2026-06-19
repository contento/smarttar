---
title: "Architecture Overview"
lang: en
manual: reference-manual-en
order: 3
---

# Architecture Overview
SmartTar is a single-process, event-driven application structured in layered subsystems.

## Subsystem Layers

    +-----------------------------------------------------+
    |  Zinc UI layer  (menus, dialogs, toolbar, views)    |
    +------------------+----------------------------------+
    |  Controller      |  Real-time engine (rt_eng)       |
    |  (control.cpp)   |  -- per-booth call state machine |
    |                  |  -- ISR-driven timing (rt_isr)   |
    +------------------+----------------------------------+
    |  Tariff engine   |  Database engine (db_eng)        |
    |  (ph_eng)        |  -- RX.DAT / RX.IDX              |
    |                  |  -- receipt lifecycle            |
    +------------------+----------------------------------+
    |  Serial driver  Print spooler  Dongle  Modem        |
    |  Config (cfg)   Logger (log)   Screen saver         |
    +-----------------------------------------------------+
              Pharlap 286 DOS Extender
              MS-DOS 6.22 / PC hardware

## Core Components

- **CONTROLLER (control.cpp).** Central coordinator. Owns references to all major subsystems. Drives the Zinc event loop and dispatches events to subsystems. Manages receipt queues (Receipts, PRNReceipts).
- **RT_ENGINE (rt_eng.cpp).** Real-time engine. Scans hardware ports for booth state changes at regular intervals. Maintains a BoothCluster array with the per-booth call state machine. Driven by a software timer ISR (rt_isr.cpp).
- **PH_ENGINE (ph_eng.cpp).** Telephony and tariff engine. Loads PH_INFO.DAT containing local, DDN, and DDI place tables, tariff bands, and schedule tables. Provides call classification and cost calculation.
- **DB_ENGINE (db_eng.cpp).** Transaction database engine. Wraps DB_STORAGE (indexed file access to RX.DAT/RX.IDX) and DB_STATISTICS (running totals). In SmartTar Pro builds also manages RXX.DAT/RXX.IDX.
- **CFG (cfg.cpp).** Configuration manager. Reads ST.CFG (binary, encrypted) and ST.INI (text). Writes changes back to both files on demand.
- **SPOOLER (spooler.cpp).** Print spooler. Queues print jobs and dispatches them to the active printer driver DLL. Supports single or double printer configuration.
- **SERIAL (serial.cpp).** Serial port driver. Used by the optional booth display (BoothDisplay) and the modem subsystem.
- **STM2 (stm2.cpp).** Non-volatile state manager. Stores critical values (receipt counters, last shutdown time) in the EEPROM dongle. Detects and recovers from abnormal shutdowns.
- **Log (log.cpp).** Event logger. Appends structured log entries to ST.LOG for startup, shutdown, access denial, and other system events.

## Hardware Interface

Each STB-x hardware cluster occupies a 16-byte I/O port block starting at APP_PORT_BASE. The port layout within a cluster is:

- **PO_GENERAL (0x00):** General output port (relays, cash drawer).
- **PO_DTMF_FLAGS:** DTMF digit availability flags. All bits set (0xFF) indicates no STB board present.

ACTIVE_CLUSTERS is determined at startup by reading the DTMF flag ports of all configured clusters. Any cluster returning 0xFF is considered absent.
