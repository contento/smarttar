---
title: "Call Lifecycle"
lang: en
manual: reference-manual-en
order: 7
---

# Call Lifecycle
Each telephone booth in RT_ENGINE maintains an independent call state machine.

## State Machine

| State | Description |
|----|----|
| IDLE | Booth is on-hook and available. |
| OFF_HOOK | Handset lifted. DTMF/pulse digit accumulation begins. Dial timeout timer (T_DIAL) starts. |
| DIALING | Digits are being accumulated. Prefix parser matches against the numbering plan. Locked numbers are checked. |
| RINGING / WAITING | Dialing complete; waiting for answer signal. Answer timeout timer (T_ANSWER) runs. |
| CONNECTED | Answer signal detected. Grace period (T_TALK) elapses, then billing starts. Tariff rate looked up from PH_ENGINE. |
| ON_HOOK | Handset replaced. Billing stops. Cost calculated with ceiling/rounding. Receipt written to RX.DAT/RX.IDX. Print spooler queues a print job. |
| LOCKED | Post-hangup lock period (T_LOCK). Booth returns to IDLE after expiry. |

## Abnormal Call Termination

If the call is abandoned before answer (T_ANSWER expires), no receipt is generated and no charge is incurred.

If communication errors (N_COM_ERR \> MAX_COM_ERR) or dialing errors (N_DIAL_ERR \> MAX_DIAL_ERR) exceed configured thresholds, an alarm is flagged in the view.

## Receipt Queues

- **Receipts (MAX_RECEIPTS = 256).** Holds completed, unpaid receipts. Used by the manual settlement window.
- **PRNReceipts (MAX_PRN_RECEIPTS = 64).** Holds receipts queued for reprinting or second-copy printing.
