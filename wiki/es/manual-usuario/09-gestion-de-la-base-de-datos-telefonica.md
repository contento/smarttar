---
title: "Gestión de la Base de Datos Telefónica"
lang: es
manual: guia-usuario-es
order: 9
---

# Gestión de la Base de Datos Telefónica
La base de datos telefónica define cómo se clasifican y tarifican las llamadas. Está compuesta por el plan de numeración (LOCAL.INF, DDN.INF, DDI.INF) compilado en PH_INFO.DAT.

## Cuándo Actualizar la Base de Datos

Actualice la base de datos telefónica cuando:

- La empresa prestadora de servicio cambia tarifas o horarios.
- Se agregan nuevos códigos de área o códigos de país.
- Nuevos destinos necesitan ser bloqueados (agregados a la lista de números bloqueados).
- Cambian los prefijos del plan de numeración local.

## Procedimiento de Actualización

1.  Edite los archivos fuente (LOCAL.INF, DDN.INF, DDI.INF) con un editor de texto plano, o use el editor de tarifas en **Configuración \> Tarifa**.
2.  Ejecute la utilidad SETUP para recompilar los archivos fuente en PH_INFO.DAT.
3.  Reinicie SmartTar. La nueva base de datos se carga al arrancar.

**Nota:** No edite PH_INFO.DAT directamente. Es un archivo binario compilado y será sobreescrito la próxima vez que se ejecute SETUP.

## Números Bloqueados

Los números bloqueados son números de teléfono o prefijos que están inhabilitados para su uso. Cuando un cliente marca un número bloqueado, la llamada se intercepta antes de conectarse y no se genera ningún cargo. Para agregar o eliminar números bloqueados, use **Configuración \> Tarifa \> Números Bloqueados** y recompile la base de datos.
