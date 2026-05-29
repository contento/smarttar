---
title: "Monitoreo de Cabinas Telefónicas"
lang: es
manual: guia-usuario-es
order: 4
---

# Monitoreo de Cabinas Telefónicas
SmartTar monitorea todas las cabinas de forma continua sin ninguna acción del operador. La vista de cabinas se actualiza en tiempo real a medida que las llamadas avanzan en su ciclo de vida.

## Estados de las Cabinas

| Estado | Significado |
|----|----|
| Libre | Auricular en cuelgue. La cabina está lista para su uso. |
| Descolgado | Auricular levantado; esperando dígitos. |
| Marcando | Se están marcando dígitos. |
| Llamando | Marcación completa; esperando contestación. |
| Hablando | Llamada conectada y facturación en curso. El costo se acumula en tiempo real. |
| Colgado | Auricular puesto; se está generando e imprimiendo el recibo. |
| Bloqueado | Período de bloqueo post-cuelgue; la cabina volverá a libre en breve. |
| Error COM | El conteo de errores de comunicación superó el umbral configurado. |
| Error Marcación | El conteo de errores de marcación superó el umbral configurado. |

## Modo Espía (Intervención)

Los supervisores pueden monitorear una llamada en curso seleccionando una cabina y activando la función de espía. En modo espía, el operador puede escuchar la llamada sin molestar al cliente. El modo espía está sujeto al indicador de configuración EXCLUSIVE_SPY, que puede limitar las intervenciones simultáneas a una cabina a la vez.

Para activar el modo espía: seleccione la celda de la cabina activa y presione el botón de espía en la barra de herramientas, o use el atajo de teclado definido en el archivo de recursos.

**Nota:** El modo espía está disponible únicamente para operadores con acceso de Supervisor. Si NO_SOUND_WHILE_SPY está habilitado, la alarma sonora se suprime durante la intervención.

## Condiciones de Alarma

Las siguientes condiciones de alarma activan un indicador visual en la celda afectada y, según la configuración, una alerta sonora:

- **Error de comunicación.** La cabina no pudo establecer señal de comunicación luego de marcar. Se activa cuando N_COM_ERR supera MAX_COM_ERR.
- **Error de marcación.** Se recibió una secuencia de dígitos inválida o incompleta. Se activa cuando N_DIAL_ERR supera MAX_DIAL_ERR.
- **Llamada entrante detectada.** Cuando DETECT_INCOME está habilitado, se señaliza una llamada entrante en la línea de una cabina.
- **Error entre clusters.** Señal inesperada de una cabina en otro cluster.

Para limpiar una alarma: corrija la causa subyacente (p. ej., verifique la línea telefónica) y luego use la función de restablecimiento de alarma en la barra de herramientas o el menú Configuración.
