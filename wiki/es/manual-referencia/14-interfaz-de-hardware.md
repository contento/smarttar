---
title: "Interfaz de Hardware"
lang: es
manual: manual-referencia-es
order: 14
---

# Interfaz de Hardware
## Placa Cluster STB-x

Cada placa cluster maneja 8 líneas telefónicas a través del bus de puertos E/S ISA. Las placas se asignan bloques de puertos E/S de 16 bytes consecutivos comenzando en APP_PORT_BASE. Con 4 clusters, el espacio E/S total usado es de 64 bytes.

El motor en tiempo real consulta los puertos del cluster a intervalos regulares del temporizador ISR. El puerto de salida PO_GENERAL activa el relé del cajón de billetes (bit GP_CASH) cuando ACTIVATE_RELAY está habilitado.

## Llave EEPROM

La llave de protección hardware se conecta al puerto paralelo (LPT1). Contiene una EEPROM que almacena el número de serie del sistema, el número máximo de clusters permitidos y la clave de versión. Al arrancar, la aplicación lee la EEPROM, valida la versión y registra el evento de inicio de sesión. Al cerrar normalmente, registra el cierre de sesión. Un cierre anormal deja el estado STM2 como BAD_SHUTDOWN, detectado en el siguiente arranque.

## Pantalla de Cabina

Una pantalla LED opcional visible al cliente puede conectarse a un puerto serial (DISPLAY_COM). La clase BoothDisplay envía el costo actual de la llamada o el mensaje DISPLAY_DEFAULT_MESSAGE a la pantalla en tiempo real. Se activa configurando DISPLAY_ENABLE = TRUE en ST.INI.


*Manual de Referencia SmartTar — Versión 2.34* *MicroDiseño Ltda. — Copyright © 1993–2003. Todos los derechos reservados.* *Gonzalo Contento Castañ*
