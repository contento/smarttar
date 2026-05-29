---
title: "Tariff Engine"
lang: en
manual: reference-manual-en
order: 8
---

# Tariff Engine
## Call Classification

PH_ENGINE classifies each call into one of the following types:

| Type | Classification rule |
|----|----|
| Local | Matches a prefix in LOCAL.INF. |
| National DDN | Matches a DDN area code in DDN.INF. |
| International DDI | Matches a DDI country code in DDI.INF. |
| USA | Routed to USA destination; DDI tariff with USA-specific minimum minutes. |
| Border zone | Dialed with BORDER_ACCESS code. |
| Cellular | Dialed with CELLULAR_ACCESS code. |
| Special services | Dialed with SPECIAL_ACCESS code. Toll-free or special billing. |
| Telex | National telex rate via NA_TLX_BASE. |

## Cost Calculation

At call termination, cost is computed in the following steps:

1.  **Duration.** Raw elapsed seconds from answer signal to hangup (minus T_TALK grace period).
2.  **Ceiling.** Duration rounded up to the nearest CEIL\_\* fraction (1.0 = whole minute, 0.5 = half minute).
3.  **Minimum minutes.** If ceiled duration \< MIN\_*, billed duration is promoted to MIN\_*.
4.  **Rate lookup.** Per-minute rate selected from tariff band based on call type, current hour, day type, and schedule state.
5.  **Subtotal.** Billed minutes × per-minute rate.
6.  **Tax.** TAX_PERCENT (or DDN_TAX / DDI_TAX) applied to subtotal.
7.  **Rounding.** Final total rounded to nearest M_ROUND unit.
