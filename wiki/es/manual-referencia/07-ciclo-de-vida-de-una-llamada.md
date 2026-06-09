---
title: "Ciclo de Vida de una Llamada"
lang: es
manual: manual-referencia-es
order: 7
---

# Ciclo de Vida de una Llamada
Cada cabina en RT_ENGINE mantiene una máquina de estados independiente.

## Máquina de Estados

| Estado | Descripción |
|----|----|
| LIBRE | Cabina en cuelgue y disponible. |
| DESCOLGADO | Auricular levantado. Inicia la acumulación de dígitos DTMF/decádicos. Arranca el temporizador de marcación (T_DIAL). |
| MARCANDO | Se están acumulando dígitos. El analizador de prefijos los coteja contra el plan de numeración. Se verifican los números bloqueados. |
| LLAMANDO / EN ESPERA | Marcación completa; esperando señal de contestación. Corre el temporizador T_ANSWER. |
| HABLANDO | Señal de contestación detectada. Transcurre el período de gracia (T_TALK), luego inicia la facturación. El motor PH_ENGINE entrega la tarifa. |
| COLGADO | Auricular puesto. Se detiene la facturación. Se calcula el costo con redondeo y mínimos. Se crea el registro en RX.DAT/RX.IDX. La cola de impresión recibe el trabajo. |
| BLOQUEADO | Período de bloqueo post-cuelgue (T_LOCK). La cabina regresa a LIBRE tras su expiración. |

## Terminación Anormal de Llamada

Si la llamada es abandonada antes de contestación (T_ANSWER expirado sin señal), no se genera recibo y no se realiza cobro.

Si los errores de comunicación (N_COM_ERR \> MAX_COM_ERR) o de marcación (N_DIAL_ERR \> MAX_DIAL_ERR) superan los umbrales configurados, se señaliza una alarma en la vista.

## Colas de Recibos

- **Receipts (MAX_RECEIPTS = 256).** Contiene recibos de llamadas completadas y no cobradas. Usada por la ventana de liquidación manual.
- **PRNReceipts (MAX_PRN_RECEIPTS = 64).** Contiene recibos en cola para reimpresión o copia adicional.

---

## Temas de ayuda relacionados

Los siguientes temas de ayuda interna (compilados en `help.dat`) cubren funcionalidad relacionada:

- [[es/ayuda/H_OPERATION|H_OPERATION]]
- [[es/ayuda/H_SIGNAL|H_SIGNAL]]
- [[es/ayuda/H_MANUAL|H_MANUAL]]
