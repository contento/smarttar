---
title: "Base de Datos de Transacciones"
lang: es
manual: manual-referencia-es
order: 10
---

# Base de Datos de Transacciones
## Estructura del Registro

Cada registro de recibo almacenado en RX.DAT incluye:

- Etiqueta de recibo (TEL para telefonía, EXT para extensión).
- Número de cabina.
- Fecha y hora de inicio de la llamada.
- Duración (segundos).
- Tipo de llamada (local, DDN, DDI, celular, frontera, especial).
- Identificador de destino (nombre de ciudad/país).
- Subtotal, importe de impuesto, costo total.
- Número de serie del recibo.
- Indicadores de pagado / sin cobrar / gratuito.

## Estadísticas

DB_STATISTICS acumula totales de todos los registros. Los resúmenes están disponibles con granularidad diaria, semanal, mensual y anual. El sistema de turnos soporta múltiples turnos de trabajo por día, cada uno con su propia instantánea de estadísticas.

---

## Temas de ayuda relacionados

Los siguientes temas de ayuda interna (compilados en `help.dat`) cubren funcionalidad relacionada:

- [[es/ayuda/H_RECEIPT|H_RECEIPT]]
- [[es/ayuda/H_ACCUM|H_ACCUM]]
- [[es/ayuda/H_SACCUM|H_SACCUM]]
- [[es/ayuda/H_SDAYS|H_SDAYS]]
- [[es/ayuda/H_VIEWER|H_VIEWER]]
