---
title: "Descripción de la Arquitectura"
lang: es
manual: manual-referencia-es
order: 3
---

# Descripción de la Arquitectura
SmartTar es una aplicación de proceso único orientada a eventos, estructurada en capas de subsistemas.

## Capas de Subsistemas

    +-----------------------------------------------------+
    |  Capa UI Zinc  (menús, diálogos, barra, vistas)     |
    +------------------+----------------------------------+
    |  Controlador     |  Motor en tiempo real (rt_eng)   |
    |  (control.cpp)   |  -- máquina de estados por cab.  |
    |                  |  -- temporización ISR (rt_isr)   |
    +------------------+----------------------------------+
    |  Motor tarifas   |  Motor base de datos (db_eng)    |
    |  (ph_eng)        |  -- RX.DAT / RX.IDX              |
    |                  |  -- ciclo de vida del recibo     |
    +------------------+----------------------------------+
    |  Driver serial  Spool impr.  Llave  Módem           |
    |  Configuración  Logger       Salvapantallas         |
    +-----------------------------------------------------+
              Extensor de DOS Pharlap 286
              MS-DOS 6.22 / Hardware PC

## Componentes Principales

- **CONTROLLER (control.cpp).** Coordinador central. Posee referencias a todos los subsistemas principales. Conduce el ciclo de eventos Zinc y enruta eventos a los subsistemas. Administra las colas de recibos (Receipts, PRNReceipts).
- **RT_ENGINE (rt_eng.cpp).** Motor en tiempo real. Escanea los puertos de hardware para detectar cambios de estado en cabinas. Mantiene un arreglo BoothCluster con la máquina de estados por cabina. Disparado por una ISR de temporizador por software (rt_isr.cpp).
- **PH_ENGINE (ph_eng.cpp).** Motor de telefonía y tarifas. Carga PH_INFO.DAT con tablas de lugares locales, DDN y DDI, bandas tarifarias y tablas de horarios. Provee clasificación de llamadas y cálculo de costos.
- **DB_ENGINE (db_eng.cpp).** Motor de base de datos de transacciones. Envuelve DB_STORAGE (acceso indexado a RX.DAT/RX.IDX) y DB_STATISTICS (totales acumulados). En versiones SmartTar Pro también administra RXX.DAT/RXX.IDX.
- **CFG (cfg.cpp).** Administrador de configuración. Lee ST.CFG (binario, cifrado) y ST.INI (texto plano). Escribe los cambios de vuelta a ambos archivos.
- **SPOOLER (spooler.cpp).** Cola de impresión. Encola trabajos de impresión y los despacha al controlador DLL activo. Soporta configuración de una o dos impresoras.
- **SERIAL (serial.cpp).** Controlador de puerto serial. Usado por la pantalla opcional para cabinas (BoothDisplay) y el subsistema de módem.
- **STM2 (stm2.cpp).** Administrador de estado no volátil. Almacena valores críticos (contadores de recibos, hora del último cierre) en la EEPROM de la llave. Detecta y recupera cierres anormales.
- **Log (log.cpp).** Registro de eventos. Agrega entradas estructuradas a ST.LOG para arranques, cierres, denegaciones de acceso y otros eventos del sistema.

## Interfaz con el Hardware

Cada cluster hardware STB-x ocupa un bloque de puertos E/S de 16 bytes empezando en APP_PORT_BASE. El mapa de puertos dentro de un cluster es:

- **PO_GENERAL (0x00):** Puerto de salida general (relés, cajón de billetes).
- **PO_DTMF_FLAGS:** Indicadores de disponibilidad de dígitos DTMF. Todos los bits en 1 (0xFF) indica que no hay placa STB presente.

ACTIVE_CLUSTERS se determina en el arranque leyendo los puertos de indicadores DTMF de todos los clusters configurados. Cualquier cluster que devuelva 0xFF se considera ausente.
