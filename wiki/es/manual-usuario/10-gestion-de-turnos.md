---
title: "Gestión de Turnos"
lang: es
manual: guia-usuario-es
order: 10
---

# Gestión de Turnos
Un *turno* representa un turno de trabajo o período de cierre de caja. SmartTar soporta múltiples turnos por día, cada uno con su propio contador de recibos e instantánea de estadísticas.

## Inicio de un Nuevo Turno

Al comienzo de cada turno:

1.  Vaya a **Archivo \> Nuevo Turno**.
2.  Confirme la acción en el diálogo que aparece.
3.  El número de turno se incrementa, las estadísticas se reinician para el nuevo período y el contador de recibos continúa desde donde quedó.

## Cierre de Turno

Al finalizar cada turno:

1.  Verifique que todas las llamadas activas hayan finalizado (todas las cabinas deben aparecer como Libre en la vista).
2.  Vaya a **Archivo \> Cerrar Turno**.
3.  Imprima el recibo de turno si se requiere (**Imprimir \> Recibo de Turno**).
4.  La base de datos actual se archiva y se prepara una nueva base de datos vacía para el siguiente turno.

**Nota:** No cierre un turno mientras haya llamadas activas.

## Consulta de Turnos Archivados

Los turnos anteriores se almacenan en archivos de histórico. Use el visor de base de datos (**Archivo \> Ver Archivo**) para navegar y reimprimir recibos de turnos archivados.
