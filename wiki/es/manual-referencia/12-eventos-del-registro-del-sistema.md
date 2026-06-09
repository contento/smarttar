---
title: "Eventos del Registro del Sistema"
lang: es
manual: manual-referencia-es
order: 12
---

# Eventos del Registro del Sistema
| Constante de evento | Descripción |
|----|----|
| NORMALSTART | Aplicación iniciada correctamente. |
| NORMALSHUTDOWN | Aplicación cerrada correctamente. |
| APPBADTRY | La aplicación no pudo iniciarse (RES.DAT falta o está corrupto). |
| CFGBADTRY | La aplicación no pudo iniciarse (ST.CFG falta o está corrupto). |
| STM2BADTRY | Acceso denegado: falló la autenticación hardware STM2. |
| DONGLEBADTRY | Acceso denegado: llave hardware no encontrada (versión demo). |
| EEPROMBADTRY | Acceso denegado: versión de EEPROM no coincide. |
| STM2GARBAGE | El estado STM2 estaba corrupto al arrancar; se reinició el estado. |
| STM2BADSHUTDOWN | La sesión anterior no terminó correctamente; se inició recuperación. |
| STM2RECOVER | El sistema entró en modo de recuperación tras cierre anormal. |
| STM2IGNORERECOVER | El operador omitió la recuperación tras cierre anormal. |

Las entradas del registro incluyen fecha, hora y el código del evento correspondiente. El archivo de registro es de solo lectura cuando no está siendo escrito.

---

## Temas de ayuda relacionados

Los siguientes temas de ayuda interna (compilados en `help.dat`) cubren funcionalidad relacionada:

- [[es/ayuda/H_SYS_INFO|H_SYS_INFO]]
- [[es/ayuda/H_ALARM|H_ALARM]]
