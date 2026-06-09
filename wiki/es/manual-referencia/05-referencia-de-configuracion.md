---
title: "Referencia de Configuración"
lang: es
manual: manual-referencia-es
order: 5
---

# Referencia de Configuración
La configuración se almacena en dos archivos: **ST.CFG** (binario, cifrado, escrito por SETUP) y **ST.INI** (texto plano, editable por la aplicación en tiempo de ejecución). Al guardar la configuración desde la interfaz gráfica, la aplicación escribe ambos archivos.

## \[Sistema\] — Parámetros del Sistema

| Parámetro | Tipo | Descripción |
|----|----|----|
| COUNTRY | Cadena (32) | Nombre del país donde está instalado el sistema. |
| CURRENCY | Cadena (16) | Símbolo monetario (p. ej., $`,US`$, S/.). |
| CITY | Cadena | Nombre de la ciudad donde se ubica el centro. |
| COMPANY | Cadena | Nombre de la empresa operadora del centro telefónico. |
| OPERATOR_NAME | Cadena | Nombre del proveedor del servicio telefónico. |
| ID | Cadena (32) | Número de identificación tributaria (NIT, CC, etc.). |
| TAX_NAME | Cadena (32) | Nombre del impuesto aplicable (p. ej., IVA). |
| TAX_PERCENT | Doble | Porcentaje de impuesto aplicado a llamadas estándar. |
| DDN_TAX | Doble | Porcentaje de impuesto aplicado a llamadas de larga distancia nacional. |
| DDI_TAX | Doble | Porcentaje de impuesto aplicado a llamadas internacionales. |
| CLUSTERS | Entero | Número de clusters de hardware configurados (1–4). |
| FORM | Enumeración | Tipo de formato de impresora. Ver Sección 9. |
| P_PORT | Cadena | Tipo de puerto de impresora: `lpt` o `com`. |
| LPT | Entero | Número de puerto paralelo (1 o 2) cuando P_PORT=lpt. |
| COM | Cadena | Configuración de puerto serial cuando P_PORT=com (puerto, baudios, bits, paridad, stop). |
| DOUBLE_PRN | Booleano | Habilitar operación con doble impresora. |
| P_FOOTER | Cadena (144) | Texto del pie de página impreso al final de cada recibo. |
| P_FOOTER1 | Cadena (64) | Línea 1 del pie de página del recibo. |
| P_FOOTER2 | Cadena (64) | Línea 2 del pie de página del recibo. |
| HEADER_LINE | Cadena (256) | Línea completa del encabezado impreso en la parte superior de los recibos. |
| HEADER_LINE1 | Cadena (64) | Línea 1 del encabezado del recibo. |
| HEADER_LINE2 | Cadena (64) | Línea 2 del encabezado del recibo. |
| HEADER_LINE3 | Cadena (64) | Línea 3 del encabezado del recibo. |
| HEADER_LINE4 | Cadena (64) | Línea 4 del encabezado (puede incluir formato %s o %d). |
| HEADER_PRINT_TAXNAME | Booleano | Imprimir nombre del impuesto en el encabezado del recibo. |
| HEADER_PRINT_RECNO | Booleano | Imprimir número de recibo en el encabezado. |
| SS_TIME | Entero | Tiempo de inactividad para el salvapantallas (minutos). |

## \[Aplicacion\] — Parámetros de Aplicación

| Parámetro | Tipo | Descripción |
|----|----|----|
| N_RECEIPT | Entero largo | Contador de recibos actual (0–999.999). Preservado en EEPROM. |
| M_ROUND | Doble | Granularidad de redondeo para totales de factura. |
| VIEW_DECIMALS | Entero | Decimales mostrados para el costo de llamada en la vista principal. |
| MIN_NAL | Entero | Minutos mínimos facturados para llamadas nacionales. |
| MIN_INTER | Entero | Minutos mínimos facturados para llamadas internacionales. |
| MIN_USA | Entero | Minutos mínimos facturados para llamadas a EE.UU. |
| MIN_BORDER | Entero | Minutos mínimos facturados para llamadas a zona de frontera. |
| MIN_CELLULAR | Entero | Minutos mínimos facturados para llamadas a celular. |
| CEIL_NAL | Doble | Fracción de redondeo de minuto para llamadas nacionales (1.0 = minuto entero, 0.5 = medio minuto). |
| CEIL_INTER | Doble | Fracción de redondeo de minuto para llamadas internacionales. |
| CEIL_USA | Doble | Fracción de redondeo de minuto para llamadas a EE.UU. |
| CEIL_BORDER | Doble | Fracción de redondeo de minuto para llamadas de frontera. |
| CEIL_CELLULAR | Doble | Fracción de redondeo de minuto para llamadas a celular. |
| APPLY_DDN_SCHEDULE | Booleano | Aplicar horario de tarifa reducida para llamadas DDN. |
| APPLY_DDI_SCHEDULE | Booleano | Aplicar horario de tarifa reducida para llamadas DDI. |
| GENERATE_PREPAID_RECEIPT | Booleano | Generar automáticamente un recibo de prepago. |
| DOUBLE_PREPAID_RECEIPT | Booleano | Imprimir dos copias de recibos de prepago. |
| MULTIPLE_PREPAID_CALLS | Booleano | Permitir múltiples llamadas en una sola sesión de prepago. |
| CALL_ACTUAL_COST | Booleano | Mostrar el costo real calculado en lugar del saldo prepago consumido. |
| MAX_COM_ERR | Entero | Número máximo de errores de comunicación antes de alarma. |
| MAX_DIAL_ERR | Entero | Número máximo de errores de marcación antes de alarma. |

## \[Telefonia\] — Parámetros de Temporización Telefónica

Todos los valores marcados (ms) están en milisegundos.

| Parámetro | Unidad | Descripción |
|----|----|----|
| T_ON_HOOK | ms | Tiempo de detección de cuelgue. |
| T_OFF_HOOK | ms | Tiempo de detección de descuelgue. |
| T_BREAK | ms | Tiempo de apertura de circuito para marcación decádica. |
| T_MAKE | ms | Tiempo de cierre de circuito para marcación decádica. |
| T_INTERDIG | ms | Pausa interdigital para marcación decádica. |
| T_DTMF_FLAG | ms | Tiempo de estabilización de indicador de dígito DTMF. |
| T_DTMF_INTERDIG | ms | Pausa interdigital en DTMF. |
| T_ANSWER | ms | Tiempo de espera de contestación. |
| T_TALK | ms | Período de gracia después de contestación antes de iniciar la facturación. |
| T_BIAS | ms | Duración de la señal de inversión de polaridad (cuando ASIGNAL = S_BIAS). |
| T_DIAL | ms | Tiempo máximo permitido para completar la secuencia de marcación. |
| T_COM | ms | Tiempo de espera de inicio de comunicación (cuando ASIGNAL = S_TIME). |
| T_LOCK | ticks | Tiempo de bloqueo después del cuelgue antes de liberar la cabina en la vista. |
| T_INTER_RING | ms | Pausa máxima entre timbrados; una pausa mayor se trata como cuelgue. |
| ASIGNAL | Enumeración | Método de detección de señal de contestación. |

**Métodos de señal de contestación (ASIGNAL):**

- **S_BIAS (inversión de polaridad).** La contestación se detecta por inversión de polaridad en la línea. T_BIAS especifica si la inversión es permanente o dura 150 ms.
- **S_TIME (por tiempo).** Se asume contestación después de T_COM segundos desde el descuelgue.
- **S_THREAD (hilo C).** Contestación detectada por señal de hilo C (tierra o −48 V).
- **S_TONE (por tono).** Contestación detectada por reconocimiento de tono.

## \[Marcacion\] — Parámetros de Marcación

| Parámetro | Tipo | Descripción |
|----|----|----|
| ACCESS_LEVELS | Entero | Número de niveles de acceso (2 o 3) en el plan de marcación. |
| ACCESS | Entero | Dígito de acceso al troncal (p. ej., 0 o 9). |
| INTER_ACCESS | Entero | Segundo dígito de acceso para llamadas internacionales. |
| CELLULAR_ACCESS | Entero | Segundo dígito de acceso para llamadas a celular. |
| BORDER_ACCESS | Entero | Segundo dígito de acceso para llamadas de frontera. |
| SPECIAL_ACCESS | Entero | Dígito de acceso para llamadas a servicios especiales. |
| OPERATOR_ACCESS | Entero | Código de acceso para llamadas asistidas por operador. |
| EDA_ACCESS | Entero | Dígito de acceso para llamadas a operador EDA. |
| LOCAL_DIGITS | Entero | Número de dígitos esperados para llamadas locales. |
| NAL_DIGITS | Entero | Número de dígitos esperados para llamadas nacionales. |
| INTER_DIGITS | Entero | Número de dígitos esperados para llamadas internacionales. |
| BORDER_DIGITS | Entero | Número de dígitos para llamadas de frontera. |
| CELLULAR_DIGITS | Entero | Número de dígitos para llamadas a celular. |
| SPECIAL_DIGITS | Entero | Número de dígitos para llamadas a servicios especiales. |
| IGNORE_EXTRA_DIGITS | Booleano | Ignorar dígitos más allá del conteo configurado. |

## \[Extensiones\] — Parámetros de Extensiones (SmartTar Pro)

| Parámetro | Tipo | Descripción |
|----|----|----|
| E_DISCOUNT | Doble | Porcentaje de descuento aplicado al costo de llamadas de extensión. |
| E_INSTALL_COST | Doble | Costo de instalación único para líneas de extensión. |
| E_LINE_COST | Doble | Costo mensual de arriendo de línea para extensiones. |
| E_FIRST_EXT | Entero | Primer número de extensión asignado. |
| E_SHOW_PHONE | Booleano | Mostrar número telefónico marcado en la vista para llamadas de extensión. |
| E_MIN_AVAILABLE | Doble | Saldo mínimo requerido para realizar una llamada de extensión. |
| E_APPLY_ROUND | Booleano | Aplicar redondeo a los totales de llamadas de extensión. |

## \[Modem\] — Parámetros del Módem

| Parámetro | Tipo | Descripción |
|----|----|----|
| MODEM_COM | Entero | Número de puerto COM usado por el módem. |
| MODEM_IRQ | Entero | Línea de interrupción (IRQ) para el puerto COM del módem. |
| MODEM_BAUDS | Entero largo | Velocidad en baudios para la comunicación por módem. |
| MODEM_DIAL | Entero | Tipo de marcación: tono o pulso. |
| MODEM_SPEAKER | Entero | Modo del altavoz del módem. |
| MODEM_PHONE | Cadena | Último número de teléfono remoto con conexión exitosa. |
| MODEM_ACKTIME | Entero | Tiempo de espera de acuse de recibo para mensajes del protocolo STC. |
| MODEM_MAXTIME | Entero largo | Tiempo máximo de conexión. |

## \[Display\] — Parámetros de la Pantalla de Cabina

| Parámetro | Tipo | Descripción |
|----|----|----|
| DISPLAY_ENABLE | Booleano | Habilitar la pantalla visible al cliente. |
| DISPLAY_COM | Entero | Puerto COM conectado a la unidad de pantalla. |
| DISPLAY_BAUDS | Entero | Velocidad en baudios para comunicación con la pantalla. |
| DISPLAY_DEFAULT_MESSAGE | Cadena (64) | Mensaje mostrado cuando no hay ninguna llamada en curso. |

---

## Temas de ayuda relacionados

Los siguientes temas de ayuda interna (compilados en `help.dat`) cubren funcionalidad relacionada:

- [[es/ayuda/H_CONFIG_MENU|H_CONFIG_MENU]]
- [[es/ayuda/H_GENERAL|H_GENERAL]]
- [[es/ayuda/H_TIME_DATE|H_TIME_DATE]]
- [[es/ayuda/H_ALARM|H_ALARM]]
- [[es/ayuda/H_LOCK|H_LOCK]]
- [[es/ayuda/H_LOCK_NUM|H_LOCK_NUM]]
- [[es/ayuda/H_ROUND|H_ROUND]]
- [[es/ayuda/H_SPY|H_SPY]]
- [[es/ayuda/H_FOOTER|H_FOOTER]]
- [[es/ayuda/H_ALIAS|H_ALIAS]]
- [[es/ayuda/H_OP_ID|H_OP_ID]]
- [[es/ayuda/H_SP_SERV|H_SP_SERV]]
- [[es/ayuda/H_FORMS|H_FORMS]]
