---
title: "Turn Management"
lang: en
manual: users-guide-en
order: 10
---

# Turn Management
A *turn* (turno) represents a work shift or accounting period. SmartTar supports multiple turns per day, each with its own receipt counter and statistics snapshot.

## Starting a New Turn

At the beginning of each shift:

1.  Go to **Archivo \> Nuevo Turno**.
2.  Confirm the action in the dialog that appears.
3.  The turn number increments, statistics are reset for the new period, and the receipt counter continues from where it left off.

## Closing a Turn

At the end of each shift:

1.  Ensure all active calls have completed (all booths idle in the view).
2.  Go to **Archivo \> Cerrar Turno**.
3.  Print the turn receipt if required (**Imprimir \> Recibo de Turno**).
4.  The current database is archived and a new empty database is prepared for the next turn.

**Note:** Do not close a turn while calls are active.

## Viewing Archived Turns

Past turns are stored in archive files. Use the database viewer (**Archivo \> Ver Archivo**) to browse and reprint receipts from archived turns.
