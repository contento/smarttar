---
title: "Receipt Formats and Printer Drivers"
lang: en
manual: reference-manual-en
order: 9
---

# Receipt Formats and Printer Drivers
## Printer Driver DLLs

| DLL          | FORM value | Description                          |
|--------------|------------|--------------------------------------|
| pr_dr80.dll  | DR_80      | 80-column parallel double receipt    |
| pr_drpre.dll | DR_PRE     | 80-column pre-printed double receipt |
| pr_lin80.dll | LINEAL_80  | 80-column line printer (linear)      |
| pr_sr80.dll  | SR_80      | 80-column serial single receipt      |
| pr_dr40.dll  | DR_40      | 40-column parallel dual roll         |
| pr_sr40.dll  | SR_40      | 40-column serial single roll         |
| pr_dr18.dll  | DR_18      | 18-column parallel dual roll         |
| pr_sr28.dll  | SR_28      | 28-column serial single roll         |
| pr_dreme.dll | DR_EME     | 80-column EMETEL thermal             |
| pr_drhal.dll | DR_HALF    | 80-column half-page thermal          |

## Receipt Content

Each receipt includes:

- **Header:** company name, city, operator name, NIT/tax ID, receipt number, date, time.
- **Call detail:** booth identifier/name, call type, destination, duration, per-minute rate.
- **Financial block:** subtotal, tax name and amount, total, rounding adjustment.
- **Footer:** configurable P_FOOTER lines.

---

## Related help topics

The following in-app help topics (Spanish, compiled into `help.dat`) cover related functionality:

- [[es/ayuda/H_RECEIPT|H_RECEIPT]]
- [[es/ayuda/H_ZPRINT|H_ZPRINT]]
- [[es/ayuda/H_IPRINT|H_IPRINT]]
- [[es/ayuda/H_P_PORT|H_P_PORT]]
- [[es/ayuda/H_S_PORT|H_S_PORT]]
- [[es/ayuda/H_PPORT|H_PPORT]]
- [[es/ayuda/H_CASH|H_CASH]]
- [[es/ayuda/H_ADM_REC|H_ADM_REC]]
- [[es/ayuda/H_OPERATION|H_OPERATION]]
- [[es/ayuda/H_PRINT_MENU|H_PRINT_MENU]]
