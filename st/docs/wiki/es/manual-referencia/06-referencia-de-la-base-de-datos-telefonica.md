---
title: "Referencia de la Base de Datos Telefónica"
lang: es
manual: manual-referencia-es
order: 6
---

# Referencia de la Base de Datos Telefónica
La base de datos telefónica se compila a partir de tres archivos fuente por la utilidad SETUP en PH_INFO.DAT. Contiene el plan de numeración, las bandas tarifarias y los horarios para llamadas locales, larga distancia nacional (DDN) e internacionales (DDI).

## Archivos Fuente

- **LOCAL.INF.** Define el plan de numeración local de la ciudad. Cada entrada mapea un prefijo marcado a un nombre de destino y una categoría de llamada. El archivo de muestra cubre Antioquia, Colombia (434 entradas).
- **DDN.INF.** Define el plan de numeración de larga distancia nacional y las bandas tarifarias base. Las entradas identifican códigos de área, nombres de destino y los índices tarifarios correspondientes.
- **DDI.INF.** Define la información de telefonía internacional. Las entradas identifican códigos de país, nombres de países e índices de tarifa DDI.

## Estructura Tarifaria

- **Tarifas DDN.** Hasta 16 bandas tarifarias, cada una con una tarifa por minuto para un grupo de destinos. Las tarifas varían según el horario del día (normal, reducida, super-reducida). Hasta 6 franjas horarias por tipo de día.
- **Tarifas DDI.** Hasta 20 bandas tarifarias para destinos internacionales. Cada banda define tarifas para períodos normal, reducido y super-reducido.
- **Tarifas locales.** Definidas por zona local usando la lista de tarifas predeterminadas (DEF_TARIFFS). Hasta 9 niveles tarifarios nombrados (A1–A9).
- **Tipos de día.** El planificador distingue tres tipos de día: día hábil normal, sábado y festivo. El calendario de festivos se verifica al inicio de la llamada para seleccionar el conjunto tarifario apropiado.
- **Horarios.** Los horarios DDN y DDI definen hasta 6 ventanas de tiempo por tipo de día. APPLY_DDN_SCHEDULE y APPLY_DDI_SCHEDULE activan o desactivan el cambio de horario.

## Números Bloqueados

PH_INFO.DAT también almacena una lista de números telefónicos bloqueados. Las llamadas a números bloqueados son interceptadas antes de que la marcación se complete. Se usa para bloquear destinos de tarifa premium o no autorizados.
