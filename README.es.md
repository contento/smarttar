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

Todas las herramientas están incluidas en el repositorio bajo [`vendor/`](vendor/) — [`vendor/bc/`](vendor/bc/), [`vendor/pharlap/`](vendor/pharlap/) y [`vendor/zinc/`](vendor/zinc/).

---

## Estructura del repositorio

```text
vendor/          Herramientas de terceros incluidas:
  bc/            Borland C++ 3.1 — compilador, depurador (TD), TASM, TLIB, TLINK
  pharlap/       Pharlap 286 DOS Extender — BCC286, BIND286, CFIG286, RUN286, DLLs en runtime
  zinc/          Zinc Interface Library 3.5 — headers, librerías, GENHELP, DESIGN
  util/          Utilidades DOS para desarrollo (NC, QEdit, PKWARE, driver de mouse)
dosbox-x.conf    Configuración local del proyecto para DOSBox-X (carga automática desde la raíz)
build.sh Lanzador del host (bash); ejecuta la compilación dentro de DOSBox-X en modo no interactivo
build.ps1 Equivalente para Windows 11 en PowerShell Core
run.sh  Lanzador del host (bash) para st.exe dentro de DOSBox-X; cierra DOSBox-X al salir
run.ps1 Equivalente para Windows 11 en PowerShell Core
.gitattributes   Impone CRLF para archivos DOS, LF para archivos del host
st/              Aplicación
  src/           Archivos fuente C++ y C, organizados por subsistema
                 (ph/, rt/, db/, ui/, mb/, tb/, ct/, ctrl/, pr/)
  include/       Archivos de cabecera
  build/         Archivos objeto — salida intermedia (no se incluye en git)
  lib/           Librerías estáticas
  bin/           Binarios de salida y datos en tiempo de ejecución
  res/           Fuente binaria del recurso Zinc (RES.DAT) — editada en
                 Zinc Designer, copiada a bin/ durante la compilación
  docs/          Guías de usuario, manual de referencia, ayuda, capturas
  test/          Utilidades de prueba para desarrollo
  util/          Utilidades de construcción y mantenimiento
  MAKEFILE       Script de construcción Borland MAKE
  run.bat        Ejecuta st.exe desde bin/
  make*.bat      Atajos por variante:
                 makedemo (DEMO + NODONGLE + RUN), makedbg (DEBUG + RUN),
                 makeeda (EDA + RUN), makeprod (RUN)
```

---

## Entorno de desarrollo (DOSBox-X)

El repositorio incluye un [`dosbox-x.conf`](dosbox-x.conf) local al proyecto, afinado para el target de SmartTar: hardware 386, DOS 5.0, modo protegido Pharlap 286, 32 MB de memoria extendida, manejadores de archivo de DOS ampliados para compilación, y un `PATH` ya poblado con `vendor\bc\BIN`, `vendor\pharlap\BIN` y los subdirectorios de `vendor\util\`.

### Instalar DOSBox-X

| Host | Comando |
| --- | --- |
| macOS | `brew install dosbox-x` |
| Windows | `winget install joncampbell123.DOSBox-X` *(o descargar desde [dosbox-x.com](https://dosbox-x.com/))* |

### Lanzar

Edita las líneas `mount c "..."` en [`dosbox-x.conf`](dosbox-x.conf) para que la activa apunte al proyecto en tu host (las rutas de Mac y Windows ya vienen pre-rellenadas — invierte cuál queda comentada al moverte entre hosts). Después:

```sh
cd /ruta/al/smarttar
dosbox-x
```

DOSBox-X carga automáticamente `dosbox-x.conf` desde el directorio actual. Aterrizarás en `C:\ST` con la cadena de herramientas en `PATH`, listo para invocar cualquiera de los atajos `make*.bat` siguientes o `run` para lanzar un ejecutable ya construido.

---

## Compilación

Todos los comandos de compilación se ejecutan desde el directorio `st/` dentro de un entorno DOS 5.0 (máquina física, DOSBox-X o emulador compatible).

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
| `EDA=1` | Define `__EDA__` — variante para operador EDA |
| ~~`HELP=1`~~ | Obsoleto / sin efecto. `bin/help.dat` es dependencia de `bin/st.exe` y se regenera automáticamente desde `docs/help.txt` vía `genhelp` cuando cambia — no se necesita ninguna bandera |

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

### Compilación sin interacción desde el host

[`build.sh`](build.sh) (macOS / Linux / bash) y [`build.ps1`](build.ps1) (Windows 11 / PowerShell Core 7+) ejecutan una compilación completa dentro de DOSBox-X desde tu shell del host sin tener que escribir nada dentro de DOS:

```sh
# bash (macOS / Linux)
./build.sh                # por defecto, demo
./build.sh debug
./build.sh --force prod   # -B (recompilar todo) + variante de producción
./build.sh --keep-log-in-st   # el log queda en st/build.log en lugar de ./build.log
```

```powershell
# PowerShell (Windows 11)
.\build.ps1                # por defecto, demo
.\build.ps1 debug
.\build.ps1 prod -Force    # -B (recompilar todo) + variante de producción
.\build.ps1 -KeepLogInSt   # el log queda en st\build.log en lugar de .\build.log
```

Ambos scripts lanzan DOSBox-X con `-c "command /c make<variante>.bat …"`, capturan toda la salida a `build.log` (transmitida en vivo con `tail -F` / `Get-Content -Wait`), esperan a que DOSBox-X termine y reportan éxito/fallo según si el batch imprimió `Build succeeded.`. Solo se puede ejecutar una instancia a la vez (DOSBox-X bloquea su pantalla). Para sobrescribir la ubicación del binario, usa `DOSBOX_X` (variable de entorno bash) o `$env:DOSBOX_X` (PowerShell).

### Ejecutar SmartTar desde el host

[`run.sh`](run.sh) y [`run.ps1`](run.ps1) lanzan el `st/bin/st.exe` ya compilado dentro de DOSBox-X y cierran DOSBox-X automáticamente cuando la aplicación termina:

```sh
# bash
./run.sh                 # cierra DOSBox-X al salir de SmartTar
./run.sh --keep-open     # deja el prompt de DOS abierto después de salir
./run.sh -- arg1 arg2    # lo que está después de `--` se pasa a st.exe
```

```powershell
# PowerShell
.\run.ps1
.\run.ps1 -KeepOpen
.\run.ps1 -- arg1 arg2
```

El cierre automático funciona porque cada script encola `-c "exit"` justo después de invocar `st`, así que DOSBox-X termina en cuanto `st.exe` devuelve el control a `COMMAND.COM`. Compila primero (`./build.sh ...`) — estos scripts solo lanzan, no compilan.

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

### `util/inf2dat/phones.csv`

Conjunto de datos de referencia con números telefónicos públicos reales y vigentes (gobierno, servicios públicos, universidades, hospitales, embajadas, bancos, aerolíneas) para Colombia, EE.UU., España y México, recopilados desde la página oficial de contacto de cada organización. Le proporciona al motor de demostración secuencias de marcado que resuelven a destinos con nombre en `ph_info.dat` en lugar de dígitos de abonado aleatorios sintetizados, de modo que las llamadas en modo demo produzcan filas de factura realistas.

Los números están normalizados al plan de marcado colombiano de la era 2003: el prefijo de área post-2021 `60X` (Resolución CRC 5826) se elimina de `dial_from_smarttar` para que los dígitos coincidan con lo que espera el árbol de numeración `local.inf` / `ddn.inf` / `ddi.inf`. La columna `published_number` conserva el formato moderno tal como se publica, para trazabilidad.

CSV RFC 4180 con todos los campos de texto entre comillas dobles y un bloque de comentarios inicial prefijado con `;`. Aún no es consumido por [`src/rt/demo_eng.cpp`](st/src/rt/demo_eng.cpp) — ese módulo sigue tomando los prefijos de pools cableados; integrar `phones.csv` en `GenCall` queda como tarea de seguimiento.

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

| Variante | Banderas | Atajo | Uso |
| --- | --- | --- | --- |
| Producción | `RUN=1` | `makeprod` | Build completo con verificación de dongle |
| Demo | `DEMO=1 NODONGLE=1 RUN=1` | `makedemo` | Ferias, evaluación |
| EDA | `EDA=1 RUN=1` | `makeeda` | Operador EDA — clasificación de llamadas distinta |
| Depuración | `DEBUG=1 RUN=1` | `makedbg` | Desarrollo; usar con el depurador Pharlap `TDP.EXE` |

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
