---
title: "Liquidación Manual de Llamadas"
lang: es
manual: guia-usuario-es
order: 5
---

# Liquidación Manual de Llamadas
La liquidación manual permite al operador revisar, aprobar e imprimir recibos de llamadas completadas. Es la herramienta principal para entregar un recibo al cliente y registrar el pago.

## Apertura de la Ventana de Liquidación

Haga clic en el botón de liquidación manual en la barra de herramientas, o seleccione la cabina y presione el atajo de teclado. La ventana de liquidación se abre mostrando todas las llamadas completadas y sin cobrar de la cabina seleccionada.

## Distribución de la Ventana de Liquidación

La ventana de liquidación muestra una tabla con las siguientes columnas para cada llamada:

| Columna        | Descripción                                 |
|----------------|---------------------------------------------|
| \# Llamada     | Número de la llamada en la sesión.          |
| Tipo / Destino | Tipo y destino de la llamada.               |
| Duración       | Duración de la llamada.                     |
| Costo          | Costo individual de la llamada.             |
| NC             | Marcar llamada como “sin cobro” (gratuita). |
| PR             | Marcar llamada para reimpresión.            |

Campos de resumen en la parte inferior:

| Campo          | Descripción                                |
|----------------|--------------------------------------------|
| **Total**      | Suma de todos los costos de llamadas.      |
| **Sin cobrar** | Total de llamadas aún no cobradas.         |
| **Gratuito**   | Total de llamadas marcadas como sin cobro. |
| **Sub-total**  | Total a cobrar.                            |
| **Pagado**     | Monto entregado por el cliente.            |
| **Cambio**     | Vuelto a devolver al cliente.              |

## Proceso de Liquidación

1.  Abra la ventana de liquidación para la cabina.
2.  Revise la lista de llamadas. Marque las llamadas gratuitas haciendo clic en su botón **NC**.
3.  Ingrese el monto entregado por el cliente en el campo *Pagado*. El campo *Cambio* se calcula automáticamente.
4.  Haga clic en **Aceptar** (o presione **Enter**) para confirmar el pago. El recibo se imprime y los registros de la llamada se marcan como pagados en la base de datos.
5.  Si hay un cajón de billetes conectado y ACTIVATE_RELAY está habilitado, el cajón se abre automáticamente.

## Reimpresión de un Recibo

Para reimprimir desde la ventana de liquidación, haga clic en el botón **PR** de la fila de la llamada correspondiente antes de hacer clic en **Aceptar**. El recibo se pondrá en cola para una segunda impresión.

Para reimprimir desde la vista de base de datos, use **Imprimir \> Historial de cabina** para localizar la llamada y seleccionar la opción de reimpresión.
