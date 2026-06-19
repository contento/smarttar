---
title: "SmartTar Communicator (STC)"
lang: es
manual: stc-es
order: 0
version: "1.0"
---

# SmartTar Communicator (STC)

> Descripción técnica del módulo de comunicación cliente/servidor por módem.

## Definición

El STC es una aplicación de comunicación cliente/servidor para DOS en modo real con interfaz gráfica.

## Requerimientos

Un sistema PC con 640K de RAM, DOS 6.22 o posterior y un modem universal.

## Archivos de la Aplicación

Se puede tener el STC aparte del ST (se recomienda por procedimiento de respaldo). Los archivos que los constituyen son los siguientes:

1. STC.EXE: El ejecutable.
2. RES.DAT: Los recursos. Es el mismo que tienen las versiones ST.217C y posteriores.
3. ST.CFG: Configuración. Es el mismo que tienen las versiones ST.217C y posteriores.

## Procedimientos

## *Recibir y Enviar Archivos*

Se pueden enviar y recibir archivos del SmartTar o de otros directorios. Se debe establecer conexión con el servidor en primera instancia, es decir, la máquina remota debe haber sido puesta en modo servidor. El último número telefónico que haya logrado una conexión exitosa sera almacenado en el ST.CFG.

El modo de consola podra enviar/ recibir mensajes de acuerdo al protocolo del STC (ver adelante), pero solo debe ser usado por personal con conocimiento avanzado del STC.

## *Poner en modo servidor*

Pone la máquina a proveer servicios para una máquina cliente remota. Se puede salir con “Cerrar” o el cliente puede utilizar la secuencia de comandos “ST_COMMAND”\|“ST_CLOSE” para colgar la llamada a nivel remoto. De igual forma si el cliente cuelga el teléfono el servidor colgará su llamada.

## *Configuración*

Permite configurar los parametros básicos de comunicación. Si estos parametros no coinciden con los del modem que se posee o si los parametros de comunicación no son iguales a los de la máquina remota, se obtendrán mensajes de error y hasta bloqueos del sistema ya que el STC está en modo real y no en modo protegido como el SmartTar.

## *Salir*

Salir de STC a DOS.

## Protocolo

El STC tiene un protocolo basado en mensajes.

## *Mensajes*

Los mensajes que utiliza el STC pueden ser utilizados en el modo consola pero teniendo en cuenta que algunos activan comandos que requireren sincronización (ST_SEND por ejemplo). Los mensajes son los siguientes:

## *Protocolo de Envío de Archivos*

<table class="t1" data-cellspacing="0" data-cellpadding="0">
<colgroup>
<col style="width: 33%" />
<col style="width: 33%" />
<col style="width: 33%" />
</colgroup>
<tbody>
<tr>
<td class="td1" data-valign="top"><p>Cliente</p></td>
<td class="td2" data-valign="top"><p>Dirección</p></td>
<td class="td3" data-valign="top"><p>Servidor</p></td>
</tr>
<tr>
<td class="td4" data-valign="top"><p>ST_UNKNOWN</p></td>
<td class="td5" data-valign="top"><p><br />
</p></td>
<td class="td6" data-valign="top"><p><br />
</p></td>
</tr>
<tr>
<td class="td7" data-valign="middle"><p>ST_COMMAND</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
</tr>
<tr>
<td class="td4" data-valign="top"><p><br />
</p></td>
<td class="td5" data-valign="top"><p><br />
</p></td>
<td class="td6" data-valign="top"><p>ST_ACKNOWLEDGE</p></td>
</tr>
<tr>
<td class="td7" data-valign="middle"><p>ST_RECEIVE</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
</tr>
<tr>
<td class="td7" data-valign="middle"><p><br />
</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
<td class="td7" data-valign="middle"><p>ST_ACKNOWLEDGE</p></td>
</tr>
<tr>
<td class="td7" data-valign="middle"><p>“Path”</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
</tr>
<tr>
<td class="td7" data-valign="middle"><p><br />
</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
<td class="td7" data-valign="middle"><p>ST_ACKNOWLEDGE | ST_UNKNOWN</p></td>
</tr>
<tr>
<td class="td8" data-valign="top"><p>SendFile()</p></td>
<td class="td9" data-valign="top"><p><br />
</p></td>
<td class="td10" data-valign="top"><p>ReceiveFile()</p></td>
</tr>
</tbody>
</table>

## *Protocolo de Recepción de Archivos*

<table class="t1" data-cellspacing="0" data-cellpadding="0">
<colgroup>
<col style="width: 33%" />
<col style="width: 33%" />
<col style="width: 33%" />
</colgroup>
<tbody>
<tr>
<td class="td1" data-valign="top"><p>Cliente</p></td>
<td class="td2" data-valign="top"><p>Dirección</p></td>
<td class="td3" data-valign="top"><p>Servidor</p></td>
</tr>
<tr>
<td class="td4" data-valign="top"><p>ST_UNKNOWN</p></td>
<td class="td5" data-valign="top"><p><br />
</p></td>
<td class="td6" data-valign="top"><p><br />
</p></td>
</tr>
<tr>
<td class="td7" data-valign="middle"><p>ST_COMMAND</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
</tr>
<tr>
<td class="td4" data-valign="top"><p><br />
</p></td>
<td class="td5" data-valign="top"><p><br />
</p></td>
<td class="td6" data-valign="top"><p>ST_ACKNOWLEDGE</p></td>
</tr>
<tr>
<td class="td7" data-valign="middle"><p>ST_SEND</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
</tr>
<tr>
<td class="td7" data-valign="middle"><p><br />
</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
<td class="td7" data-valign="middle"><p>ST_ACKNOWLEDGE</p></td>
</tr>
<tr>
<td class="td7" data-valign="middle"><p>“Path” + “Filename”</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
</tr>
<tr>
<td class="td7" data-valign="middle"><p><br />
</p></td>
<td class="td7" data-valign="middle"><p><br />
</p></td>
<td class="td7" data-valign="middle"><p>ST_ACKNOWLEDGE | ST_UNKNOWN</p></td>
</tr>
<tr>
<td class="td8" data-valign="top"><p>ReceiveFile()</p></td>
<td class="td9" data-valign="top"><p><br />
</p></td>
<td class="td10" data-valign="top"><p>SendFile()</p></td>
</tr>
</tbody>
</table>

## *Referencia de Comandos del Protocolo*

Se deben usar con cuidado ya que algunos comandos requiren sincronización o pueden inclusive cerrar el sistema remoto.

- ST_ACKNOWLEDGE: reconocmiento por parte del interlocutor del mensaje enviado.
- ST_CLOSE: cerrar el sistema remoto, se debe estar en modo comando.
- ST_COMMAND: poner sistema remoto en modo comando. En modo comando se puede usar ST_CLOSE, ST_SEND y ST_RECEIVE.
- ST_CONSOLE: poner sistema remoto en modo consola. En modo consola el sistema transmite los mensajes escritos en la consola. Para volver a modo comando se usa ST_COMMAND.
- ST_RECEIVE: indicar al sistema remoto que se desea recibir un archivo de él.
- ST_SEND: indicar al sistema remoto que se le va a enviar un archivo.
- ST_UNKNOWN: desconocmiento por parte del interlocutor del mensaje enviado.

Gonzalo Contento [<span class="s1">mailto://gcontento@bigfoot.com</span>](mailto://gcontento@bigfoot.com) septiembre de 1998.
