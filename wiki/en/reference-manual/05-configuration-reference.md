---
title: "Configuration Reference"
lang: en
manual: reference-manual-en
order: 5
---

# Configuration Reference
Configuration is stored in two files: **ST.CFG** (binary, encrypted, written by SETUP) and **ST.INI** (plain text, editable by the application at runtime). When the application saves configuration from the GUI, it writes both files.

## \[Sistema\] — System Parameters

| Parameter | Type | Description |
|----|----|----|
| COUNTRY | String (32) | Country name where the system is installed. |
| CURRENCY | String (16) | Currency symbol (e.g., $`,US`$, S/.). |
| CITY | String | City name where the center is located. |
| COMPANY | String | Company name of the telephone center operator. |
| OPERATOR_NAME | String | Name of the telephony service provider. |
| ID | String (32) | Tax identification number (NIT, CC, etc.). |
| TAX_NAME | String (32) | Name of the applicable tax (e.g., IVA). |
| TAX_PERCENT | Double | Tax percentage applied to standard calls. |
| DDN_TAX | Double | Tax percentage applied to national long-distance calls. |
| DDI_TAX | Double | Tax percentage applied to international calls. |
| CLUSTERS | Integer | Number of hardware clusters configured (1–4). |
| FORM | Enum | Printer form type. See Section 9. |
| P_PORT | String | Printer port type: `lpt`, `com`, or `pdf`. With `pdf`, receipts are written to a PDF file (`PDF\RXYYMMDD.pdf`, one per day) instead of a physical printer. |
| LPT | Integer | Parallel port number (1 or 2) when P_PORT=lpt. |
| COM | String | Serial port settings when P_PORT=com (port, baud, bits, parity, stop). |
| DOUBLE_PRN | Boolean | Enable dual-printer operation. |
| P_FOOTER | String (144) | Footer text printed at the bottom of each receipt. |
| P_FOOTER1 | String (64) | Receipt footer line 1. |
| P_FOOTER2 | String (64) | Receipt footer line 2. |
| HEADER_LINE | String (256) | Full header line printed at the top of receipts. |
| HEADER_LINE1 | String (64) | Receipt header line 1. |
| HEADER_LINE2 | String (64) | Receipt header line 2. |
| HEADER_LINE3 | String (64) | Receipt header line 3. |
| HEADER_LINE4 | String (64) | Receipt header line 4 (may include %s or %d format). |
| HEADER_PRINT_TAXNAME | Boolean | Print tax name on receipt header. |
| HEADER_PRINT_RECNO | Boolean | Print receipt number on receipt header. |
| RECNO_LABEL | String (64) | Label prefix for receipt number on printed receipt (e.g., `Recibo`). |
| RECNO_DIGITS | Integer | Number of digits for receipt number display. |
| RECNO_LEADING_ZEROS | Boolean | Pad receipt number with leading zeros. |
| SHORT_SERIAL | String (32) | Default serial number shown in receipt header `<serial>` when EEPROM serial is absent. Default: `AA52048`. |
| SS_TIME | Integer | Screen saver timeout (minutes). |

## \[Aplicacion\] — Application Parameters

| Parameter | Type | Description |
|----|----|----|
| N_RECEIPT | Long | Current receipt counter (0–999999). Preserved in EEPROM. |
| M_ROUND | Double | Rounding granularity for invoice totals. |
| VIEW_DECIMALS | Integer | Decimal places shown for call cost in the main view. |
| MIN_NAL | Integer | Minimum billed minutes for national calls. |
| MIN_INTER | Integer | Minimum billed minutes for international calls. |
| MIN_USA | Integer | Minimum billed minutes for calls to the USA. |
| MIN_BORDER | Integer | Minimum billed minutes for border zone calls. |
| MIN_CELLULAR | Integer | Minimum billed minutes for cellular calls. |
| CEIL_NAL | Double | Minute ceiling fraction for national calls (1.0 = whole minute, 0.5 = half). |
| CEIL_INTER | Double | Minute ceiling fraction for international calls. |
| CEIL_USA | Double | Minute ceiling fraction for USA calls. |
| CEIL_BORDER | Double | Minute ceiling fraction for border zone calls. |
| CEIL_CELLULAR | Double | Minute ceiling fraction for cellular calls. |
| APPLY_DDN_SCHEDULE | Boolean | Apply reduced-rate schedule for DDN calls. |
| APPLY_DDI_SCHEDULE | Boolean | Apply reduced-rate schedule for DDI calls. |
| GENERATE_PREPAID_RECEIPT | Boolean | Automatically generate a prepaid receipt. |
| DOUBLE_PREPAID_RECEIPT | Boolean | Print two copies of prepaid receipts. |
| MULTIPLE_PREPAID_CALLS | Boolean | Allow multiple calls within a single prepaid session. |
| CALL_ACTUAL_COST | Boolean | Show actual computed call cost rather than prepaid balance consumed. |
| MAX_COM_ERR | Integer | Maximum communication errors before alarm. |
| MAX_DIAL_ERR | Integer | Maximum dialing errors before alarm. |

## \[Telefonia\] — Telephony Timing Parameters

All timing values marked (ms) are in milliseconds.

| Parameter | Unit | Description |
|----|----|----|
| T_ON_HOOK | ms | On-hook detection time. |
| T_OFF_HOOK | ms | Off-hook detection time. |
| T_BREAK | ms | Circuit break time for pulse (decadic) dialing. |
| T_MAKE | ms | Circuit make time for pulse dialing. |
| T_INTERDIG | ms | Inter-digit gap for pulse dialing. |
| T_DTMF_FLAG | ms | DTMF digit flag settling time. |
| T_DTMF_INTERDIG | ms | DTMF inter-digit gap. |
| T_ANSWER | ms | Answer timeout. |
| T_TALK | ms | Grace period after answer before billing starts. |
| T_BIAS | ms | Polarity inversion signal duration (when ASIGNAL = S_BIAS). |
| T_DIAL | ms | Maximum time allowed for dial sequence completion. |
| T_COM | ms | Communication start timeout (when ASIGNAL = S_TIME). |
| T_LOCK | ticks | Post-hangup lock time before booth is cleared in the view. |
| T_INTER_RING | ms | Maximum gap between rings; longer gap treated as on-hook. |
| ASIGNAL | Enum | Answer signal detection method. |

**Answer signal methods (ASIGNAL):**

- **S_BIAS (polarity inversion).** Answer detected by polarity inversion. T_BIAS specifies whether the inversion is permanent or lasts 150 ms.
- **S_TIME (time).** Answer assumed after T_COM seconds from off-hook.
- **S_THREAD (C-wire).** Answer detected by C-wire earth or −48 V signal.
- **S_TONE (tone).** Answer detected by tone recognition.

## \[Marcacion\] — Dialing Parameters

| Parameter | Type | Description |
|----|----|----|
| ACCESS_LEVELS | Integer | Number of access levels (2 or 3) in the dialing plan. |
| ACCESS | Integer | Trunk access digit (e.g., 0 or 9). |
| INTER_ACCESS | Integer | Second access digit for international calls. |
| CELLULAR_ACCESS | Integer | Second access digit for cellular calls. |
| BORDER_ACCESS | Integer | Second access digit for border zone calls. |
| SPECIAL_ACCESS | Integer | Access digit for special service calls. |
| OPERATOR_ACCESS | Integer | Access code for operator-assisted calls. |
| EDA_ACCESS | Integer | Access digit for EDA operator calls. |
| LOCAL_DIGITS | Integer | Number of digits expected for local calls. |
| NAL_DIGITS | Integer | Number of digits expected for national calls. |
| INTER_DIGITS | Integer | Number of digits expected for international calls. |
| BORDER_DIGITS | Integer | Number of digits for border zone calls. |
| CELLULAR_DIGITS | Integer | Number of digits for cellular calls. |
| SPECIAL_DIGITS | Integer | Number of digits for special service calls. |
| IGNORE_EXTRA_DIGITS | Boolean | Ignore digits beyond the configured count. |

## \[Extensiones\] — Extension Parameters (SmartTar Pro)

| Parameter | Type | Description |
|----|----|----|
| E_DISCOUNT | Double | Discount percentage applied to extension call costs. |
| E_INSTALL_COST | Double | One-time installation charge for extension lines. |
| E_LINE_COST | Double | Monthly line rental cost for extension lines. |
| E_FIRST_EXT | Integer | First extension number assigned. |
| E_SHOW_PHONE | Boolean | Show dialed phone number in the view for extension calls. |
| E_MIN_AVAILABLE | Double | Minimum account balance required for an extension call. |
| E_APPLY_ROUND | Boolean | Apply rounding to extension call totals. |

## \[Modem\] — Modem Parameters

| Parameter     | Type    | Description                                      |
|---------------|---------|--------------------------------------------------|
| MODEM_COM     | Integer | COM port number used by the modem.               |
| MODEM_IRQ     | Integer | IRQ line for the modem COM port.                 |
| MODEM_BAUDS   | Long    | Baud rate for modem communication.               |
| MODEM_DIAL    | Integer | Dial type: tone or pulse.                        |
| MODEM_SPEAKER | Integer | Modem speaker mode.                              |
| MODEM_PHONE   | String  | Last successfully connected remote phone number. |
| MODEM_ACKTIME | Integer | Acknowledge timeout for STC protocol messages.   |
| MODEM_MAXTIME | Long    | Maximum connection time.                         |

## \[Display\] — Booth Display Parameters

| Parameter | Type | Description |
|----|----|----|
| DISPLAY_ENABLE | Boolean | Enable the customer-facing booth display. |
| DISPLAY_COM | Integer | COM port connected to the display unit. |
| DISPLAY_BAUDS | Integer | Baud rate for display communication. |
| DISPLAY_DEFAULT_MESSAGE | String (64) | Message shown when no call is in progress. |

---

## Related help topics

The following in-app help topics (Spanish, compiled into `help.dat`) cover related functionality:

- [[es/ayuda/H_CONFIG_MENU|H_CONFIG_MENU]]
- [[es/ayuda/H_GENERAL|H_GENERAL]]
- [[es/ayuda/H_TIME_DATE|H_TIME_DATE]]
- [[es/ayuda/H_ALARM|H_ALARM]]
- [[es/ayuda/H_LOCK|H_LOCK]]
- [[es/ayuda/H_LOCK_NUM|H_LOCK_NUM]]
- [[es/ayuda/H_ROUND|H_ROUND]]
- [[es/ayuda/H_SPY|H_SPY]]
- [[es/ayuda/H_FOOTER|H_FOOTER]]
- [[es/ayuda/H_ALIAS|H_ALIAS]]
- [[es/ayuda/H_OP_ID|H_OP_ID]]
- [[es/ayuda/H_SP_SERV|H_SP_SERV]]
- [[es/ayuda/H_FORMS|H_FORMS]]
