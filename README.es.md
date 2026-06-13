# SmartTar

*Read in [English](README.md).*

Sistema de gestión de tarifas telefónicas en tiempo real para cabinas telefónicas públicas
Desarrollado por [MicroDiseño Ltda.](https://microdiseno.com) · Derechos reservados © 1993–2003 · Versión 2.70.1

![SmartTar](st/docs/archive/SmartTar.jpg)

---

**SmartTar** es un sistema punto de venta para DOS, diseñado para operadores de
telecomunicaciones en Colombia que administran centros de cabinas telefónicas.
Monitorea hasta 32 cabinas en tiempo real, clasifica las llamadas por destino,
aplica tarifas según horario (hora del día, festivos), imprime recibos con IVA
a través de controladores de impresora enchufables, y mantiene una base de datos
completa de transacciones — todo desde un único ejecutable en modo protegido.

Capacidades principales: medición de llamadas en tiempo real, clasificación
automática contra un plan de numeración configurable, motor de tarifas con
calendario de festivos, impresión de recibos (18/40/80 col + térmica),
almacenamiento indexado de transacciones, soporte de tarjetas magnéticas
prepagadas, integración con módem, displays externos de cabina, módulo de
estadísticas.

El nombre proviene de **Smart + Tar(*ifa*)** — no tiene relación con Unix `tar`.

---

## Inicio rápido

**Requisitos:** [DOSBox-X](https://dosbox-x.com/).
```sh
brew install dosbox-x               # macOS
winget install joncampbell123.DOSBox-X   # Windows
```

**Compilar** (dentro de DOSBox-X: `cd ST` luego `makedemo`), o desde el shell del host:
```sh
./build.sh                 # por defecto, demo (sin dongle)
./build.sh --force prod    # variante de producción con verificación de dongle
```

La documentación completa está en [wiki/es/](wiki/es/).
La documentación en inglés está en [wiki/en/](wiki/en/).

---

## Contenido

### Español — [wiki/es/](wiki/es/)

- [Guía del Usuario](wiki/es/manual-usuario/) — inicio, monitoreo, informes, contraseñas
- [Manual de Referencia](wiki/es/manual-referencia/) — arquitectura, configuración, motor de tarifas, interfaz de hardware, recibos
- [Ayuda](wiki/es/ayuda/) — temas de ayuda de la aplicación (compilados en `help.dat`)

### English — [wiki/en/](wiki/en/)

- [User Guide](wiki/en/users-guide/) — starting, monitoring, reports, passwords
- [Reference Manual](wiki/en/reference-manual/) — architecture, config, tariff engine, hardware interface, receipts
- [In-app Help](wiki/en/help/) — English help topics (reference only; the application ships in Spanish)

### Documentación de desarrollo — [wiki/dev/](wiki/dev/)

- [Configuración de DOSBox-X](wiki/dev/dosbox-x-smarttar-setup.md)
- [Notas volátiles de ISR](wiki/dev/ISR_VOLATILE_NOTES.md)
- [Flujo de trabajo con Zinc Designer](wiki/dev/zinc-designer-workflow.md)

---

## Cadena de herramientas

SmartTar se compila con Borland C++ 3.1 + Turbo Assembler para el target de
modo protegido Pharlap 286, usando Zinc Interface Library 3.5 para la UI. No
se necesita un compilador en el host — la compilación se ejecuta dentro de
DOSBox-X.

Los binarios propietarios de la cadena de herramientas están en un repositorio
privado separado
(**[`smarttar-vendor`](https://github.com/contento/smarttar-vendor)**), y se
clonan en `vendor/` con `./setup-vendor.sh`. No están incluidos en este
repositorio por restricciones de copyright / redistribución. Ver
[VENDOR_SETUP.md](VENDOR_SETUP.md) para detalles.

---

## Variantes de compilación

| Variante | Atajo | Uso |
| -------- | ----- | --- |
| Producción | `makeprod` | Build completo con verificación de dongle |
| Demo | `makedemo` | Ferias, evaluación — sin dongle |
| EDA | `makeeda` | Operador EDA — clasificación de llamadas distinta |
| Depuración | `makedbg` | Desarrollo; usar con el depurador Pharlap `TDP.EXE` |

---

## Capturas de pantalla

| Versión | Captura |
| ------- | ------- |
| 2.33 | ![SmartTar 2.33](st/docs/archive/SmartTar%202.33.gif) |
| 2.32.1 | ![SmartTar 2.32.1](st/docs/archive/SmartTar%202.32.1.gif) |

---

## Historia

MicroDiseño Ltda. — la empresa que desarrolló SmartTar — fue una firma
colombiana de tecnología especializada en sistemas de tarificación y medición
telefónica en el suroccidente de Colombia (Nariño, Cauca, Putumayo). SmartTar
se desplegó en cabinas telefónicas comerciales y puntos de venta
institucionales (incluyendo redes *Servicios & Transcripciones* y la
Universidad del Norte). La empresa ya no está en operación; el código fue
preservado y resucitado en 2026.

---

## Licencia

Derechos reservados © 1993–2003 MicroDiseño Ltda. Todos los derechos reservados.
