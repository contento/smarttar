---
title: "Inicio del Sistema"
lang: es
manual: guia-usuario-es
order: 2
---

# Inicio del Sistema
## Requisitos Previos

- La llave hardware (EEPROM) debe estar conectada al puerto paralelo (LPT1) antes de iniciar.
- Las placas cluster STB-x deben estar instaladas y conectadas a las líneas telefónicas.
- La impresora debe estar conectada, encendida y con papel cargado.
- Todos los archivos de datos en tiempo de ejecución (ST.CFG, RES.DAT, PH_INFO.DAT, etc.) deben estar presentes en el directorio de la aplicación.

## Inicio de la Aplicación

Desde el símbolo del sistema DOS, cambie al directorio de la aplicación y ejecute:

    C:\SMARTTAR\BIN> ST

La aplicación carga el extensor de DOS Pharlap, muestra la versión y el número de serie, y luego inicializa la interfaz gráfica. La ventana principal se abre maximizada.

## Secuencia de Arranque

Durante el arranque, SmartTar realiza los siguientes pasos:

1.  **Autenticación hardware.** Se lee y valida la EEPROM de la llave. Si la llave no está presente o la versión no coincide, se muestra el mensaje **“Acceso Negado”** y la aplicación termina.
2.  **Carga de configuración.** Se leen ST.CFG y ST.INI desde el directorio de la aplicación.
3.  **Carga de base de datos telefónica.** PH_INFO.DAT se carga en la memoria de modo protegido.
4.  **Detección de clusters.** El motor en tiempo real sondea cada puerto de cluster configurado para contar las placas STB-x físicamente instaladas. El contador ACTIVE_CLUSTERS se actualiza en consecuencia.
5.  **Recuperación por cierre anormal.** Si la sesión anterior no terminó correctamente, se muestra un aviso de recuperación.
6.  **Vista principal.** La vista de monitoreo de cabinas se muestra maximizada a pantalla completa.

## Recuperación por Cierre Anormal

Si la sesión anterior terminó de forma anormal (corte de energía, reinicio del sistema, fallo de la aplicación), la EEPROM registra esto como un evento BAD_SHUTDOWN. En el siguiente arranque, SmartTar muestra la fecha y hora del cierre anormal y pregunta si desea proceder con la recuperación.

Durante la ventana de 5 segundos, presione la tecla apropiada para omitir la recuperación o espere a que expire el tiempo para que la recuperación proceda automáticamente. La recuperación restaura el contador de recibos desde el último estado conocido en la EEPROM, evitando números de recibo duplicados o faltantes.

**Nota:** Siempre permita que la recuperación se complete si no está seguro. Omitir la recuperación puede resultar en saltos en la numeración de recibos.
