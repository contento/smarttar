---
title: "Mensajes de Error"
lang: es
manual: manual-referencia-es
order: 13
---

# Mensajes de Error
## Errores de Arranque

- **“Acceso Negado”.** Fallo de autenticación hardware STM2, versión de EEPROM no coincide, o llave no encontrada. La aplicación termina tras este mensaje.
- **“Configuración no disponible”.** ST.CFG falta o está corrupto. Ejecute la utilidad SETUP para reinstalar la configuración.
- **errno + “\RES.DAT”.** El archivo de recursos Zinc no puede abrirse. Verifique que RES.DAT esté en el directorio de la aplicación.
- **“Versión no válida”.** La versión de la EEPROM no coincide con esta versión. Contacte al distribuidor para actualizar el firmware.

## Errores en Tiempo de Ejecución

- **“El sistema está procesando información”.** Se muestra en el diálogo de salida cuando el motor en tiempo real tiene llamadas activas. Espere a que todas las cabinas queden libres antes de salir.
- **Sin memoria.** Se invoca el manejador new_handler cuando falla la asignación de memoria. El sistema intenta liberar la reserva de emergencia (2 KB) y mostrar un mensaje.
- **GPF (Fallo de Protección General).** Los GPF no manejados en modo protegido 286 son capturados por un manejador de excepciones personalizado (NewGPFHandler). El manejador registra el fallo e intenta una salida controlada.

---

## Temas de ayuda relacionados

Los siguientes temas de ayuda interna (compilados en `help.dat`) cubren funcionalidad relacionada:

- [[es/ayuda/H_SETUP|H_SETUP]]
