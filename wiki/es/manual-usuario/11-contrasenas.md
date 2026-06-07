---
title: "Contraseñas"
lang: es
manual: guia-usuario-es
order: 11
---

# Contraseñas
SmartTar implementa control de acceso mediante contraseñas. El sistema solicita inicio de sesión antes de que se puedan realizar operaciones con control de acceso.

## Resumen de Niveles de Acceso

| Nivel | Qué pueden hacer los operadores de este nivel |
|----|----|
| Usuario 1 / Usuario 2 | Monitorear cabinas, liquidar llamadas, imprimir recibos, ver estadísticas. |
| Operador | Acceso de solo lectura a información de cuentas de extensiones. No puede cambiar configuración. |
| Supervisor | Todo lo anterior, más: cambiar configuración, administrar turnos, editar tarifas, gestionar contraseñas. |
| Puerta trasera | Acceso completo para técnicos de servicio. Omite verificaciones de versión. |

## Inicio de Sesión

Cuando se invoca una función con control de acceso, SmartTar solicita una contraseña. Ingrese la contraseña del nivel apropiado y presione **Enter**. El sistema otorga acceso si la contraseña coincide con algún nivel con privilegios suficientes.

## Cambio de Contraseñas

Vaya a **Configuración \> Claves**. Ingrese la contraseña anterior, la nueva contraseña y confírmela. La nueva contraseña tiene efecto inmediato.

**Reglas de contraseña:**

- Máximo 8 caracteres.
- Distingue mayúsculas de minúsculas.
- Se almacena cifrada en ST.CFG.

**Nota:** Si pierde el acceso, contacte a su técnico de servicio. La contraseña de puerta trasera es utilizada exclusivamente por el personal de servicio de MicroDiseño.
