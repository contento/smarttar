---
title: "Descripción General del Producto"
lang: es
manual: manual-referencia-es
order: 1
---

# Descripción General del Producto
SmartTar es un sistema de administración de tarifas telefónicas en tiempo real diseñado para centros de cabinas telefónicas. Funciona bajo MS-DOS 5.0 en modo protegido 286 a través del extensor de DOS Pharlap, y gestiona el ciclo completo de una llamada: desde la detección de descolgado hasta el cálculo de tarifas, la impresión de recibos y el archivo de transacciones.

SmartTar fue desarrollado por MicroDiseño Ltda. (Gonzalo Contento Castañ) desde julio de 1993. La versión 2.34 soporta hasta 32 cabinas telefónicas distribuidas en 4 clusters de hardware de 8 cabinas cada uno.

## Capacidades Principales

- **Medición de llamadas en tiempo real.** Monitorea hasta 32 cabinas telefónicas simultáneamente. Cada cabina se consulta en tiempo real para detectar estado del gancho, dígitos DTMF y señales de contestación.
- **Clasificación automática de llamadas.** Resuelve el prefijo marcado contra un plan de numeración configurable para determinar el tipo de llamada: local, larga distancia nacional (DDN), internacional (DDI), frontera, celular o servicio especial.
- **Motor de tarifas.** Aplica franjas horarias y tipos de día para calcular el costo de la llamada. Soporta hasta 16 bandas tarifarias DDN y hasta 20 bandas DDI, con hasta 6 franjas de horario y 3 tipos de día (normal, sábado, festivo).
- **Calendario de festivos.** Configurable por año; usado por el planificador de tarifas para aplicar tarifas de día festivo en fechas designadas.
- **Impresión de recibos.** Emite recibos numerados con IVA incluido a través de un sistema de controladores de impresora intercambiables. Soporta impresoras de 18, 28, 40 y 80 columnas, incluyendo modelos térmicos.
- **Base de datos de transacciones.** Almacén de archivos indexados (RX.DAT / RX.IDX) con detección de llamadas duplicadas y archivo multi-turno.
- **Soporte de tarjetas magnéticas prepago.** Hasta 4 denominaciones de tarjeta configurables.
- **Integración con módem.** Reportes remotos y sincronización de configuración a través del módulo SmartTar Communicator (STC).
- **Pantalla externa para cabinas.** Pantalla LED opcional orientada al cliente, conectada por puerto serial por cluster.
- **Módulo de estadísticas.** Resúmenes de llamadas e ingresos: diario, semanal, mensual y anual.
- **Protección de copia por hardware.** Llave EEPROM en puerto paralelo (desactivable solo en versiones de demostración).

## Ediciones del Producto

**SmartTar Estándar.** Soporta cabinas telefónicas públicas únicamente. La base de datos de recibos almacena llamadas por número de cabina.

**SmartTar Pro.** Extiende la edición estándar con facturación de líneas de extensión PBX. Cada cabina puede configurarse como cabina pública o como extensión interna. Las extensiones reciben un porcentaje de descuento configurable y pueden facturarse por instalación y arriendo mensual de línea. Una base de datos separada (RXX.DAT / RXX.IDX) registra las llamadas de extensiones.
