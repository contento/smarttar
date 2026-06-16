# STC — SmartTar Telephony Calculator

**Purpose:** Interactive tariff rate calculator and rate table editor for technicians.

## Overview

STC is a Zinc UI application that allows field technicians to preview and test tariff rate calculations without modifying the live system. It loads the current tariff data, accepts call parameters (location, duration, type), and displays the calculated cost.

**Features:**
- **Rate lookup:** Search by exchange code, location code, or phone prefix
- **Cost calculation:** Enter call duration and type; STC calculates the charge
- **Tariff browsing:** View all configured rates and rate tables
- **Modem setup:** Configure serial port/modem settings for test calls
- **Server/client mode:** Supports remote testing via modem

**Components:**
- `modemcfg.cpp` — Serial port and modem device configuration
- `client.cpp` / `server.cpp` — Remote testing over modem connection
- `console.cpp` — User interface for rate entry and display
- `activate.cpp` — License/activation code handling

**Status:** Active. Field technician tool. Useful for tariff verification, customer service rate quotes, and rate table validation.

**References:**
- PH_ENGINE — tariff lookup and cost calculation (src/ph/ph_tar.cpp, src/ph/ph_place.cpp)
- Zinc UI framework — UIW_WINDOW, UIW_EDIT, UIW_LISTBOX
- Modem device class — include/modemdev.h
