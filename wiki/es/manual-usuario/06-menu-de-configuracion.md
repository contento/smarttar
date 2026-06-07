---
title: "Menú de Configuración"
lang: es
manual: guia-usuario-es
order: 6
---

# Menú de Configuración
El menú Configuración (**Configuración**) provee diálogos para ajustar los parámetros del sistema. La mayoría de los diálogos requieren acceso de nivel Supervisor.

> **Activación del menú.** El menú Configuración está oculto por omisión y **no
> tiene tecla de acceso rápido**. Para mostrarlo: pulse [ALT] para entrar a la
> barra de menús, abra **Información** y elija **Activar menú de configuración**.
> Una vez visible, se abre con su mnemónico **Alt+C**. Al terminar, ocúltelo de
> nuevo con **Configuración > Desactivar este menú**.

## Fecha y Hora

Establece la fecha y hora del sistema. Los cambios tienen efecto inmediato y se reflejan en la barra de estado. Este diálogo escribe la nueva hora en el reloj en tiempo real del DOS.

**Ruta:** Configuración \> Fecha y Hora

## Señal de Contestación

Selecciona el método utilizado para detectar cuándo una llamada es contestada. Hay cuatro métodos disponibles:

| Método | Descripción |
|----|----|
| Por inversión | Contestación detectada por inversión de polaridad de la línea. Seleccione **Permanente** si es sostenida, o **Durante 150 ms** si es un pulso breve. |
| Por tiempo | Se asume contestación después de una cantidad de segundos configurable. Ingrese el tiempo de espera en el campo *Tiempo*. |
| Por hilo C | Contestación detectada por señal de hilo C (tierra o −48 V). |
| Por tono | Contestación detectada por reconocimiento de tono. |

**Ruta:** Configuración \> Señal de Contestación

**Nota:** Seleccione el método que corresponde a la central telefónica de su empresa prestadora de servicio. Una configuración incorrecta causará inicios de facturación prematuros o perdidos.

## Tiempos

Ajusta los parámetros de temporización telefónica. Los campos corresponden a T_TALK, T_DIAL, T_LOCK, T_ANSWER y T_COM. Todos los valores se ingresan en segundos y se almacenan internamente en milisegundos.

**Ruta:** Configuración \> Tiempos

**Precaución:** Cambiar los parámetros de temporización afecta todas las cabinas de inmediato. Use los valores predeterminados a menos que un técnico de servicio lo indique.

## Nombres de Cabinas

Asigna un nombre visible (hasta 12 caracteres) a cada cabina. Estos nombres aparecen en la vista principal, en los recibos y en los reportes. En SmartTar Pro, cada cabina también puede designarse como línea de extensión mediante el indicador de Extensión Activa.

**Ruta:** Configuración \> Nombres de Cabinas

## Impresora

Configura la conexión de la impresora y el tipo de formato. Seleccione el formato apropiado en la lista desplegable. Establezca el puerto a LPT1, LPT2 o un puerto COM según corresponda con su hardware.

**Ruta:** Configuración \> Impresora

## Tarifa

Abre el editor de configuración tarifaria. Desde aquí, los supervisores pueden ver y editar:

- Bandas tarifarias DDN y sus tarifas.
- Bandas tarifarias DDI y sus tarifas.
- Horarios por franjas del día (normal, reducida, super-reducida) para DDN y DDI.
- Niveles de tarifa local (A1–A9).
- Lista de números bloqueados.

**Ruta:** Configuración \> Tarifa

Después de editar, use **Base de datos telefónica \> Compilar** para recompilar PH_INFO.DAT y que las nuevas tarifas queden activas.

## Días Festivos

Define el calendario de festivos para el año actual y los siguientes. Las fechas de festivos son usadas por el planificador de tarifas para aplicar tarifas de día festivo. Ingrese las fechas como pares día/mes.

**Ruta:** Configuración \> Días Festivos

## Claves

Permite al supervisor cambiar las contraseñas de acceso. Ingrese la contraseña actual, luego ingrese y confirme la nueva contraseña. Las contraseñas son de 1 a 8 caracteres.

**Ruta:** Configuración \> Claves

**Importante:** Conserve un registro de la contraseña de supervisor en un lugar seguro. Si se pierde, se requiere un técnico de servicio con acceso de puerta trasera para restablecerla.
