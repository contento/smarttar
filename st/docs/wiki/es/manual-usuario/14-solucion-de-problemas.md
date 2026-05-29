---
title: "Solución de Problemas"
lang: es
manual: guia-usuario-es
order: 14
---

# Solución de Problemas
## La Aplicación No Inicia

| Síntoma | Acción |
|----|----|
| Mensaje “Acceso Negado” | Verifique que la llave esté conectada en LPT1. Confirme que la versión de la llave coincide con esta versión. |
| Error con “RES.DAT” | Verifique que RES.DAT exista en el directorio de la aplicación. Reinstale desde el medio de distribución. |
| “Configuración no disponible” | ST.CFG falta o está corrupto. Ejecute la utilidad SETUP para reinstalar la configuración. |
| “Versión no válida” | Versión de EEPROM no coincide. Contacte a su distribuidor para una actualización de firmware. |
| Error DOS o fallo del programa | Memoria extendida insuficiente. Asegúrese de que ningún programa residente esté consumiendo memoria extendida. |

## Cabinas No Detectadas

| Síntoma | Acción |
|----|----|
| ACTIVE_CLUSTERS muestra 0 | Verifique que las placas STB-x estén correctamente asentadas en el bus ISA y que la alimentación esté conectada. |
| Menos clusters de los esperados | Una o más placas STB-x pueden haber fallado. El puerto de indicadores DTMF devuelve 0xFF para placas ausentes. |
| No se detectan llamadas en una cabina | Verifique que la línea telefónica esté conectada al puerto correcto de la placa STB-x. Revise CLUSTERS en ST.INI. |

## Problemas de Facturación

| Síntoma | Acción |
|----|----|
| La facturación inicia muy pronto o muy tarde | Ajuste T_TALK (período de gracia) y el método de señal de contestación en Configuración \> Señal de Contestación. |
| Llamadas clasificadas incorrectamente | Revise el plan de numeración en LOCAL.INF / DDN.INF. Verifique ACCESS, INTER_ACCESS, CELLULAR_ACCESS y parámetros de dígitos. |
| Se aplica tarifa incorrecta | Verifique las bandas tarifarias y horarios en Configuración \> Tarifa. Revise los ajustes APPLY_DDN_SCHEDULE / APPLY_DDI_SCHEDULE. |
| El total del recibo redondea inesperadamente | Revise M_ROUND en la configuración de la aplicación. Establézcalo en 0 para deshabilitar el redondeo. |

## Problemas de Impresora

| Síntoma | Acción |
|----|----|
| No se imprimen recibos | Verifique que la impresora esté en línea y conectada al puerto configurado. Revise P_PORT y FORM en Configuración \> Impresora. |
| Salida ilegible o desalineada | Verifique que el FORM seleccionado coincida con el ancho del papel y el modelo de impresora. |
| La cola de impresión se acumula | Revise si hay fallas de hardware en la impresora (atasco de papel, fuera de línea). La cola reintenta automáticamente cuando la impresora está lista. |

## Problemas de Cierre Anormal / Recuperación

- Siempre permita que la recuperación proceda (no la omita) a menos que un técnico de servicio lo indique.
- Después de la recuperación, verifique que el contador de recibos sea correcto revisando el último recibo en la vista de base de datos.
- Los cierres anormales frecuentes indican un problema de suministro de energía. Instale una fuente de alimentación ininterrumpida (UPS).

## Problemas de Base de Datos

- Si RX.DAT se corrompe, use la utilidad SETUP o UTIL para ejecutar una recuperación de base de datos. Haga una copia de seguridad de RX.DAT y RX.IDX antes de ejecutar cualquier herramienta de reparación.
- No elimine RX.DAT ni RX.IDX mientras SmartTar esté en ejecución.
- Si la base de datos supera su conteo máximo de registros, inicie un nuevo turno para archivar los datos actuales.

## Cómo Obtener Soporte

Al reportar un problema, proporcione la siguiente información:

- Versión y número de serie de SmartTar (desde **Información \> Acerca de**).
- El contenido del archivo ST.LOG del directorio de la aplicación.
- Una descripción de los pasos que llevaron al problema.
- Si se registró un evento de cierre anormal.


*Guía del Usuario SmartTar — Versión 2.34* *MicroDiseño Ltda. — Copyright © 1993–2003. Todos los derechos reservados.* *Gonzalo Contento Castañ*
