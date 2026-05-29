---
title: "Distribución de la Pantalla Principal"
lang: es
manual: guia-usuario-es
order: 3
---

# Distribución de la Pantalla Principal
La pantalla principal de SmartTar se divide en cuatro áreas funcionales.

## Barra de Menús

Ubicada en la parte superior de la pantalla. Los menús disponibles dependen del nivel de acceso del operador actual:

| Menú | Contenido |
|----|----|
| **Archivo** | Gestión de turnos (iniciar/cerrar turno), archivo histórico, salir. |
| **Imprimir** | Impresión de reportes: acumulador de ventas, historial de cabina, registro de transacciones. |
| **Configuración** | Diálogos de configuración del sistema: fecha/hora, tipo de señal, tiempos, impresora, configuración de telefonía. |
| **Información** | Información del sistema, versión y número de serie. |
| **Simulación** | (Solo en versiones demo/desarrollo.) Pruebas de llamadas simuladas. |
| **Ayuda** | Ayuda en aplicación y diálogo Acerca de. |

## Barra de Herramientas

La barra de herramientas debajo de la barra de menús provee botones de acceso rápido a las operaciones más frecuentes: liquidación manual, impresión, accesos directos de configuración y opciones de vista. Pase el cursor sobre un botón para ver su función en la barra de estado.

## Vista de Cabinas (Área de Trabajo Principal)

El área central muestra una cuadrícula de celdas de cabinas, una por cada cabina telefónica. Cada celda muestra:

- Nombre o número de la cabina (configurable en **Configuración \> Nombres de Cabinas**).
- Estado actual de la llamada (libre, descolgado, marcando, llamando, hablando, bloqueado).
- Tiempo transcurrido de la llamada (en tiempo real durante una llamada activa).
- Costo acumulado de la llamada (actualizado en tiempo real; decimales según VIEW_DECIMALS).
- Indicador de tipo de llamada (local, DDN, DDI, celular, frontera).
- Indicadores de alarma: error de comunicación, error de marcación.

## Barra de Estado

La barra de estado en la parte inferior de la pantalla muestra: fecha y hora actuales, número de turno actual, número de llamadas activas, último número de recibo emitido, nombre del operador y nivel de acceso.
