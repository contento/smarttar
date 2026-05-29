---
title: "Niveles de Acceso por Contraseña"
lang: es
manual: manual-referencia-es
order: 11
---

# Niveles de Acceso por Contraseña
| Nivel | Nombre interno | Acceso otorgado |
|----|----|----|
| Puerta trasera | BACKDOOR | Acceso completo al sistema incluyendo cambios de configuración. Usado por técnicos de servicio. |
| Supervisor | SUPERVISOR | Acceso operacional completo: configuración, reportes, cierre de turno, base de datos telefónica. |
| Usuario 1 | USER1 | Acceso de operador estándar: monitoreo de cabinas, impresión de recibos, liquidación manual. |
| Usuario 2 | USER2 | Acceso de operador restringido. |
| Operador | OPERATOR | Acceso de solo lectura a servicios de extensión e información. No puede modificar configuración. |
| Utilidad | UTIL | Acceso para la utilidad de diagnóstico UTIL. |

Las contraseñas tienen un máximo de 8 caracteres y se almacenan cifradas en ST.CFG. La EEPROM STM2 valida el estado de sesión iniciada; si se detecta un cierre anormal, el sistema solicita confirmación de recuperación antes de continuar.
