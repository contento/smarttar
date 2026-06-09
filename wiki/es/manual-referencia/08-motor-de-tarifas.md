---
title: "Motor de Tarifas"
lang: es
manual: manual-referencia-es
order: 8
---

# Motor de Tarifas
## Clasificación de Llamadas

PH_ENGINE clasifica cada llamada en uno de los siguientes tipos:

| Tipo | Regla de clasificación |
|----|----|
| Local | Coincide con un prefijo en LOCAL.INF. |
| Nacional DDN | Coincide con un código de área en DDN.INF. |
| Internacional DDI | Coincide con un código de país en DDI.INF. |
| EE.UU. | Enrutada a destino EE.UU.; tarifa DDI con mínimos específicos. |
| Frontera | Marcada con código BORDER_ACCESS. |
| Celular | Marcada con código CELLULAR_ACCESS. |
| Servicios especiales | Marcada con código SPECIAL_ACCESS. Gratuita o facturación especial. |
| Télex | Tarifa de télex nacional mediante NA_TLX_BASE. |

## Cálculo del Costo

Al terminar la llamada (cuelgue), el costo se calcula en los siguientes pasos:

1.  **Duración.** Segundos transcurridos desde la señal de contestación hasta el cuelgue (menos el período de gracia T_TALK).
2.  **Redondeo al techo.** La duración se redondea al alza según la fracción CEIL\_\* (1.0 = minuto entero, 0.5 = medio minuto).
3.  **Minutos mínimos.** Si la duración redondeada es menor que MIN\_*, la duración facturada se eleva a MIN\_*.
4.  **Consulta de tarifa.** La tarifa por minuto se selecciona de la banda tarifaria según el tipo de llamada, la hora actual, el tipo de día y el estado del horario.
5.  **Subtotal.** Minutos facturados × tarifa por minuto.
6.  **Impuesto.** Se aplica TAX_PERCENT (o DDN_TAX / DDI_TAX) al subtotal.
7.  **Redondeo.** El total final se redondea a la unidad M_ROUND más cercana.

---

## Temas de ayuda relacionados

Los siguientes temas de ayuda interna (compilados en `help.dat`) cubren funcionalidad relacionada:

- [[es/ayuda/H_NAL_TAR|H_NAL_TAR]]
- [[es/ayuda/H_INTER_TAR|H_INTER_TAR]]
- [[es/ayuda/H_TIMES|H_TIMES]]
