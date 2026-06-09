---
title: "Niveles de Acceso por Contraseña"
lang: es
manual: manual-referencia-es
order: 11
---

# Niveles de Acceso por Contraseña
| Nivel | Nombre interno | Código por omisión | Acceso otorgado |
|----|----|----|----|
| Puerta trasera | BACKDOOR | (reservado al proveedor) | Acceso completo al sistema incluyendo cambios de configuración. Usado por técnicos de servicio. |
| Supervisor | SUPERVISOR | `Super` | Acceso operacional completo: configuración, reportes, cierre de turno, base de datos telefónica. |
| Usuario 1 | USER1 | `User1` | Acceso de operador estándar: monitoreo de cabinas, impresión de recibos, liquidación manual. |
| Usuario 2 | USER2 | `User2` | Acceso de operador restringido. |
| Operador | OPERATOR | `Oper` | Acceso de solo lectura a servicios de extensión e información. No puede modificar configuración. |
| Utilidad | UTIL | `Util` | Acceso para la utilidad de diagnóstico UTIL. |

**Códigos por omisión.** Una instalación nueva -- o una restaurada con la utilidad DEFPWD -- usa los códigos de la tabla. **Distinguen mayúsculas de minúsculas** y admiten hasta 8 caracteres. Para **activar el menú de Configuración** sirve cualquier código válido *excepto* `Util`; `Super` otorga acceso completo, los demás un acceso restringido. Existe además un código maestro de recuperación reservado al proveedor (no publicado). Cambie estos códigos por omisión en cualquier instalación de producción mediante *Configuración > Cambiar código de acceso*.

Las contraseñas tienen un máximo de 8 caracteres y se almacenan cifradas en ST.CFG. La EEPROM STM2 valida el estado de sesión iniciada; si se detecta un cierre anormal, el sistema solicita confirmación de recuperación antes de continuar.

---

## Temas de ayuda relacionados

Los siguientes temas de ayuda interna (compilados en `help.dat`) cubren funcionalidad relacionada:

- [[es/ayuda/H_PASSWORD|H_PASSWORD]]
- [[es/ayuda/H_CHANGE_PASSWD|H_CHANGE_PASSWD]]
