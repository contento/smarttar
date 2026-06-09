---
title: "Referencia de Archivos en Tiempo de Ejecución"
lang: es
manual: manual-referencia-es
order: 4
---

# Referencia de Archivos en Tiempo de Ejecución
| Archivo | Propósito |
|----|----|
| ST.EXE | Ejecutable principal de la aplicación (modo protegido Pharlap) |
| ST.CFG | Configuración del sistema (binario, cifrado). Escrito por la utilidad SETUP. |
| ST.INI | Configuración editable por el operador (texto plano). Subconjunto de ST.CFG. |
| RES.DAT | Archivo de recursos UI de Zinc (ventanas, diálogos, bitmaps, cadenas). |
| HELP.DAT | Base de datos de ayuda en aplicación (generada desde DOC.TXT). |
| PH_INFO.DAT | Base de datos telefónica compilada (binario). Generada de DDI.INF + DDN.INF + LOCAL.INF. |
| DDI.INF | Plan de numeración internacional y tarifas fuente (texto). |
| DDN.INF | Plan de numeración larga distancia nacional y tarifas fuente (texto). |
| LOCAL.INF | Plan de numeración local fuente (texto). |
| RX.DAT | Archivo de registros de transacciones. Registros binarios secuenciales. |
| RX.IDX | Archivo de índice de transacciones. |
| RX.STA | Archivo de instantánea de estadísticas. |
| RXX.DAT | Registros de llamadas de extensiones (solo SmartTar Pro). |
| RXX.IDX | Índice de llamadas de extensiones (solo SmartTar Pro). |
| ST.LOG | Registro de eventos del sistema (texto, modo agregar). |
| PR\_\*.DLL | DLLs de controladores de impresora (librerías dinámicas Pharlap). |

---

## Temas de ayuda relacionados

Los siguientes temas de ayuda interna (compilados en `help.dat`) cubren funcionalidad relacionada:

- [[es/ayuda/H_SYS_INFO|H_SYS_INFO]]
