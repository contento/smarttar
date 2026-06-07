---
title: "Manual Call Settlement"
lang: en
manual: users-guide-en
order: 5
---

# Manual Call Settlement
Manual settlement allows the operator to review, approve, and print receipts for completed calls. This is the primary tool used to hand a receipt to a customer and record payment.

## Opening the Settlement Window

Click the manual settlement toolbar button, or select the booth and press the keyboard shortcut. The settlement window opens showing all completed, unpaid calls for the selected booth.

## Settlement Window Layout

The settlement window shows a table with the following columns for each call:

| Column             | Description                           |
|--------------------|---------------------------------------|
| Call \#            | Call number in the session.           |
| Type / Destination | Call type and destination.            |
| Duration           | Call duration.                        |
| Cost               | Individual call cost.                 |
| NC                 | Mark call as “no charge” (toll-free). |
| PR                 | Mark call for reprint.                |

Summary fields at the bottom:

| Field          | Description                         |
|----------------|-------------------------------------|
| **Total**      | Sum of all call costs.              |
| **Sin cobrar** | Total of calls not yet settled.     |
| **Gratuito**   | Total of calls marked as no charge. |
| **Sub-total**  | Billable total.                     |
| **Pagado**     | Amount tendered by the customer.    |
| **Cambio**     | Change to return to the customer.   |

## Settling a Call

1.  Open the settlement window for the booth.
2.  Review the call list. Mark any toll-free calls by clicking their **NC** button.
3.  Enter the amount tendered by the customer in the *Pagado* field. The *Cambio* field calculates automatically.
4.  Click **Aceptar** (or press **Enter**) to confirm payment. The receipt is printed and the call records are marked as paid in the database.
5.  If a cash drawer is connected and ACTIVATE_RELAY is set, the drawer opens automatically.

## Reprinting a Receipt

To reprint from the settlement window, click the **PR** button on the corresponding call row before clicking **Aceptar**. The receipt will be queued for a second print.

To reprint from the database view, use **Imprimir \> Historial de cabina** to locate the call and select the reprint option.
