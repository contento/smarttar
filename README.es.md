# SmartTar

*Read in [English](README.md).*

Sistema de gestión de tarifas telefónicas en tiempo real para cabinas telefónicas públicas

Desarrollado por [MicroDiseño Ltda.](https://microdiseno.com) · Derechos reservados © 1993–2003 · Versión 2.34

![SmartTar](st/docs/SmartTar.jpg)

---

## Descripción general

SmartTar es un sistema punto de venta para DOS, diseñado para operadores de telecomunicaciones en Colombia que administran centros de cabinas telefónicas públicas. Monitorea las llamadas activas en tiempo real, las clasifica por destino, aplica la tarifa correspondiente, imprime recibos con impuestos (IVA) y mantiene una base de datos completa de transacciones — todo desde un único ejecutable DOS en modo protegido.

> **Sobre el nombre** — *SmartTar* = **Smart + Tar(ifa)**. *Tarifa* es la palabra usada en Colombia para el cobro de llamadas telefónicas. No tiene relación con el `tar` de Unix.

### Capacidades principales

- **Medición de llamadas en tiempo real** — monitorea hasta 32 cabinas telefónicas (4 clusters × 8 cabinas) simultáneamente vía línea serial
- **Clasificación automática de llamadas** — resuelve los números marcados contra un árbol de numeración configurable para determinar el tipo de llamada: local, larga distancia nacional (DDN), internacional (DDI), frontera, celular o especial
- **Motor de tarifas** — aplica horarios por hora del día y tipo de día (hasta 16 bandas tarifarias DDN / 20 DDI, 5–6 franjas horarias, 3 tipos de día incluyendo festivos)
- **Calendario de festivos** — configurable por año, usado por el programador tarifario
- **Impresión de recibos** — emite recibos numerados con impuesto incluido (IVA) a través de un sistema de controladores de impresora (plug-in) que soporta impresoras de 18, 40 y 80 columnas, además de modelos térmicos
- **Base de datos de transacciones** — almacenamiento indexado (`RX.DAT` / `RX.IDX`) con detección de duplicados y archivado
- **Soporte de tarjetas magnéticas prepagadas** — hasta 4 denominaciones
- **Integración con módem** — para reportes y sincronización remota
- **Display externo de cabina** — display serial opcional, orientado al cliente, por cabina
- **Módulo de estadísticas** — totales diarios de llamadas e ingresos
- **Llave de protección (dongle) por hardware** — protección anticopia vía dongle en puerto paralelo (omitible en builds demo)

---

## Requisitos del sistema

| Componente | Requisito |
| --- | --- |
| Sistema operativo | MS-DOS 5.0 |
| CPU | 286 o superior |
| Memoria | Memoria extendida vía Pharlap 286 DOS Extender |
| Impresora | Serial o paralela; 18, 40 u 80 columnas |
| Llave de hardware | Dongle en puerto paralelo (builds de producción) |

---

## Cadena de herramientas

| Herramienta | Versión |
| --- | --- |
| Compilador | Borland C++ 3.1 (`BCC` / `BCC286`) |
| Ensamblador | Turbo Assembler (`TASM`) |
| Enlazador | `TLINK` + Pharlap `BIND286` |
| Framework de UI | Zinc Interface Library 3.5 |
| Extensor DOS | Pharlap 286 v3.0 |
| Sistema de construcción | Borland `MAKE` |

Todas las herramientas están incluidas en el repositorio bajo [`BC/`](BC/), [`PHARLAP/`](PHARLAP/) y [`ZINC/`](ZINC/).

---

## Estructura del repositorio

```text
BC/              Borland C++ 3.1 — compilador, depurador (TD), TASM, TLIB, TLINK
PHARLAP/         Pharlap 286 DOS Extender — BCC286, BIND286, CFIG286, RUN286, DLLs en runtime
ZINC/            Zinc Interface Library 3.5 — headers, librerías pre-compiladas, herramientas de diseño
st/              Aplicación
  src/           Archivos fuente C++ y C, organizados por subsistema
                 (ph/, rt/, db/, ui/, mb/, tb/, ct/, ctrl/, pr/)
  include/       Archivos de cabecera
  build/         Archivos objeto — salida intermedia (no se incluye en git)
  lib/           Librerías estáticas
  bin/           Binarios de salida y datos en tiempo de ejecución
  docs/          Guías de usuario, manual de referencia, ayuda, capturas
  test/          Utilidades de prueba para desarrollo
  util/          Utilidades de construcción y mantenimiento
  web/           Recursos web e historial de versiones
  MAKEFILE       Script de construcción Borland MAKE
  run.bat        Ejecuta st.exe desde bin/
  makedemo.bat   Atajo: build demo + bind (DEMO + RUN + NODONGLE)
```

---

## Compilación

Todos los comandos de compilación se ejecutan desde el directorio `st/` dentro de un entorno DOS 5.0 (máquina física, DOSBox o emulador compatible).

### Build de producción estándar

```bat
cd st
make RUN=1
```

`RUN=1` invoca `BIND286` y `CFIG286` después del enlazado para producir un ejecutable Pharlap en modo protegido auto-contenido.

### Banderas de compilación

| Bandera | Efecto |
| --- | --- |
| `RUN=1` | Enlaza y configura el ejecutable con Pharlap (`BIND286` + `CFIG286`) |
| `DEBUG=1` | Habilita símbolos de depuración (`-v`) e información para el enlazador |
| `DEMO=1` | Define `__DEMO__` — activa modo demo |
| `NODONGLE=1` | Define `__NO_DONGLE__` — omite verificación del dongle (requiere `DEMO=1`) |
| `AUTO=1` | Define `__AUTO__` — modo simulación / piloto automático |
| `EDA=1` | Define `__EDA__` — variante para operador EDA |
| `HELP=1` | Regenera `bin/help.dat` desde `docs/help.txt` vía `genhelp` |

**Build demo (no requiere dongle):**

```bat
make RUN=1 DEMO=1 NODONGLE=1
```

O use el atajo:

```bat
makedemo
```

**Build con depuración:**

```bat
make DEBUG=1 RUN=1
```

### Salida

La compilación produce:

- `bin/st.exe` — aplicación principal en modo protegido
- `bin/pr_*.dll` — DLLs de controlador de impresora (una por cada modelo soportado)

### DLLs de controlador de impresora

Los controladores se compilan como DLLs de Pharlap a partir de `src/pr/pr_*.c`:

| DLL | Descripción |
| --- | --- |
| `pr_sr80.dll` | Serial 80 columnas |
| `pr_dr80.dll` | Paralela 80 columnas |
| `pr_lin80.dll` | Line printer 80 columnas |
| `pr_drpre.dll` | Formas pre-impresas |
| `pr_dr40.dll` | Paralela 40 columnas |
| `pr_sr40.dll` | Serial 40 columnas |
| `pr_dr18.dll` | Paralela 18 columnas |
| `pr_sr28.dll` | Serial 28 columnas |
| `pr_dreme.dll` | Térmica REME |
| `pr_drhal.dll` | Térmica HAL |

---

## Configuración

La aplicación se configura mediante dos archivos de texto plano en `bin/`:

### `st.ini`

Configuración del operador. Generada por la utilidad `SETUP`. Secciones principales:

| Sección | Ajustes |
| --- | --- |
| `[Sistema]` | País, moneda, nombre de la empresa, tasa de IVA, puerto de impresora, puerto serial |
| `[Aplicacion]` | Tiempos mínimos de llamada, redondeos, numeración de recibos, opciones de display |
| `[Telefonia]` | Parámetros de tiempo — detección de descolgado, DTMF, timeout de respuesta, timeout de marcado |
| `[Marcacion]` | Códigos de acceso, conteo de dígitos por tipo de llamada, niveles de acceso |
| `[Extensiones]` | Configuración de líneas de extensión interna |
| `[Valores Criticos]` | Banderas de detección de casos límite |
| `[Modem]` | Puerto COM del módem, baudaje, prefijo de marcado, timeouts |
| `[Display]` | Habilitar display de cabina, puerto COM, baudaje, mensaje por defecto |

### `local.inf`

Información de numeración local — mapea prefijos marcados con nombres de ciudad de destino y categorías de llamada para llamadas locales. El archivo de ejemplo cubre Antioquia, Colombia (434 entradas).

### `bin/ddn.inf`

Información de telefonía nacional de larga distancia — plan de numeración y datos tarifarios para llamadas DDN (domésticas).

### `bin/ddi.inf`

Información de telefonía internacional — plan de numeración y datos tarifarios para llamadas DDI (internacionales).

`local.inf`, `ddn.inf` y `ddi.inf` son compilados juntos por `SETUP` en `bin/ph_info.dat`.

---

## Archivos de datos en tiempo de ejecución

| Archivo | Propósito |
| --- | --- |
| `bin/st.exe` | Aplicación principal |
| `bin/st.ini` | Configuración del operador |
| `bin/ph_info.dat` | Base de datos de telefonía compilada (desde `ddi.inf` + `ddn.inf` + `local.inf`) |
| `bin/res.dat` | Recursos de UI de Zinc |
| `bin/help.dat` | Base de datos de ayuda interna |
| `bin/RX.DAT` | Registros de transacciones |
| `bin/RX.IDX` | Índice de transacciones |
| `bin/RX.STA` | Captura de estadísticas |
| `bin/LOG.DAT` | Registro de eventos del sistema |
| `bin/ddi.inf` | Información de telefonía internacional (fuente) |
| `bin/ddn.inf` | Información de telefonía nacional de larga distancia (fuente) |
| `bin/local.inf` | Información de telefonía local (fuente) |

---

## Arquitectura

SmartTar es una aplicación de un solo proceso en modo protegido. Los subsistemas principales son:

```text
┌─────────────────────────────────────────────────────┐
│  Capa de UI Zinc  (menús, diálogos, toolbar, vistas)│
├──────────────┬──────────────────────────────────────┤
│  Controlador │  Motor en tiempo real (rt_eng)       │
│  (control)   │  — máquina de estados por cabina     │
│              │  — temporización por ISR (rt_isr)    │
├──────────────┼──────────────────────────────────────┤
│  Motor de    │  Motor de base de datos              │
│  tarifas     │  (db_eng / dstorage)                 │
│  (ph_eng)    │  — RX.DAT / RX.IDX                   │
│              │  — ciclo de vida del recibo          │
├──────────────┴──────────────────────────────────────┤
│  Driver serial · Spooler de impresión · Dongle ·    │
│  Módem · Config (cfg) · Logger (log) · Salvapantalla│
└─────────────────────────────────────────────────────┘
          Pharlap 286 DOS Extender
          MS-DOS 5.0 / hardware PC
```

### Ciclo de vida de la llamada

1. **Detección de descolgado** — la línea serial señaliza actividad en la cabina; `rt_eng` arranca el temporizador
2. **Captura de dígitos** — se acumulan los dígitos DTMF; `parser` resuelve el prefijo contra el plan de numeración
3. **Detección de contestación** — comienza la facturación; la tarifa se busca en `ph_eng` según tipo de llamada, hora del día y tipo de día
4. **Detección de colgado** — se calcula el tiempo transcurrido, costo final (con redondeo/techo), y se escribe el recibo en `RX.DAT`
5. **Impresión del recibo** — el spooler despacha al DLL del controlador de impresora activo
6. **Actualización de estadísticas** — `dstatist` agrega los totales diarios

---

## Variantes de compilación

| Variante | Banderas | Uso |
| --- | --- | --- |
| Producción | `RUN=1` | Build completo con verificación de dongle |
| Demo | `DEMO=1 NODONGLE=1 RUN=1` | Ferias, evaluación |
| EDA | `EDA=1 RUN=1` | Operador EDA — clasificación de llamadas distinta |
| Depuración | `DEBUG=1 RUN=1` | Desarrollo; usar con el depurador Pharlap `TDP.EXE` |
| Simulación | `AUTO=1 RUN=1` | Pruebas automatizadas / reproducción de demo |

---

## Capturas de pantalla

| Versión | Captura |
| --- | --- |
| 2.33 | ![SmartTar 2.33](st/docs/SmartTar%202.33.gif) |
| 2.32.1 | ![SmartTar 2.32.1](st/docs/SmartTar%202.32.1.gif) |

---

## Historia

MicroDiseño Ltda. — la empresa que desarrolló SmartTar — ya no está en operación.

MicroDiseño fue una firma colombiana de tecnología e ingeniería especializada en el desarrollo de hardware, instalación y soporte técnico para sistemas de tarificación y medición telefónica. Sus operaciones tuvieron particular presencia en el suroccidente de Colombia, incluyendo los departamentos de Nariño, Cauca y Putumayo.

SmartTar era el sistema de información y plataforma de hardware propietarios de la empresa, diseñados para administrar, medir y controlar la facturación de llamadas telefónicas. Su despliegue principal incluyó:

- **Cabinas telefónicas comerciales** — software de punto de venta y pasarela de hardware central, utilizada para registrar y cobrar los minutos consumidos por clientes en centros comerciales de llamadas.
- **Puntos de venta institucionales y comerciales** — unidades administrativas regionales y redes comerciales (como las redes *Servicios & Transcripciones* y hubs académicos como los de la Universidad del Norte) lo usaron para gestionar la logística de comunicaciones y la facturación telefónica en punto de venta.

La era de las cabinas telefónicas comerciales dedicadas y del software de tarificación localizado basado en hardware tuvo su pico a finales de los noventa y durante los dos mil, antes del dominio de la telefonía celular y el VoIP. Por esta razón, la documentación técnica detallada, el código fuente y los registros corporativos activos de la plataforma están ausentes de los índices web públicos modernos. Las menciones que sobreviven se encuentran en auditorías administrativas institucionales heredadas, retrospectivas profesionales regionales de ingenieros que trabajaron allí, y reportes académicos de infraestructura.

---

## Licencia

Derechos reservados © 1993–2003 MicroDiseño Ltda. Todos los derechos reservados.
