---
title: "Runtime Files Reference"
lang: en
manual: reference-manual-en
order: 4
---

# Runtime Files Reference
| File | Purpose |
|----|----|
| ST.EXE | Main application executable (Pharlap protected-mode) |
| ST.CFG | System configuration (binary, encrypted). Written by SETUP utility. |
| ST.INI | Operator-editable runtime settings (plain text). Subset of ST.CFG. |
| RES.DAT | Zinc UI resource file (windows, dialogs, bitmaps, strings). |
| HELP.DAT | In-application help database (generated from DOC.TXT). |
| PH_INFO.DAT | Compiled telephony database (binary). Generated from DDI.INF + DDN.INF + LOCAL.INF. |
| DDI.INF | International telephony numbering plan and tariff source (text). |
| DDN.INF | National long-distance telephony numbering plan and tariff source (text). |
| LOCAL.INF | Local hometown numbering plan source (text). |
| RX.DAT | Transaction record file. Sequential binary records. |
| RX.IDX | Transaction index file. |
| RX.STA | Statistics snapshot file. |
| RXX.DAT | Extension call records (SmartTar Pro only). |
| RXX.IDX | Extension call index (SmartTar Pro only). |
| ST.LOG | System event log (text, appended). |
| PR\_\*.DLL | Printer driver DLLs (Pharlap dynamic libraries). |
