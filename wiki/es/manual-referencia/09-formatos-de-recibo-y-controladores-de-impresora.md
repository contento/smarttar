---
title: "Formatos de Recibo y Controladores de Impresora"
lang: es
manual: manual-referencia-es
order: 9
---

# Formatos de Recibo y Controladores de Impresora
## DLLs de Controladores de Impresora

| DLL          | Valor FORM | Descripción                     |
|--------------|------------|---------------------------------|
| pr_dr80.dll  | DR_80      | 80 col. doble recibo (paralela) |
| pr_drpre.dll | DR_PRE     | 80 col. doble recibo preimpreso |
| pr_lin80.dll | LINEAL_80  | 80 col. lineal                  |
| pr_sr80.dll  | SR_80      | 80 col. un recibo (serial)      |
| pr_dr40.dll  | DR_40      | 40 col. doble rollo (paralela)  |
| pr_sr40.dll  | SR_40      | 40 col. rollo simple (serial)   |
| pr_dr18.dll  | DR_18      | 18 col. doble rollo (paralela)  |
| pr_sr28.dll  | SR_28      | 28 col. rollo simple (serial)   |
| pr_dreme.dll | DR_EME     | 80 col. EMETEL térmico          |
| pr_drhal.dll | DR_HALF    | 80 col. media página térmico    |

## Contenido del Recibo

Cada recibo incluye:

- **Encabezado:** nombre de la empresa, ciudad, nombre del operador, NIT, número de recibo, fecha y hora.
- **Detalle de la llamada:** identificador/nombre de la cabina, tipo de llamada, destino, duración, tarifa por minuto.
- **Bloque financiero:** subtotal, nombre e importe del impuesto, total, ajuste de redondeo.
- **Pie de página:** líneas P_FOOTER configurables.

---

## Temas de ayuda relacionados

Los siguientes temas de ayuda interna (compilados en `help.dat`) cubren funcionalidad relacionada:

- [[es/ayuda/H_RECEIPT|H_RECEIPT]]
- [[es/ayuda/H_ZPRINT|H_ZPRINT]]
- [[es/ayuda/H_IPRINT|H_IPRINT]]
- [[es/ayuda/H_P_PORT|H_P_PORT]]
- [[es/ayuda/H_S_PORT|H_S_PORT]]
- [[es/ayuda/H_PPORT|H_PPORT]]
- [[es/ayuda/H_CASH|H_CASH]]
- [[es/ayuda/H_ADM_REC|H_ADM_REC]]
- [[es/ayuda/H_OPERATION|H_OPERATION]]
- [[es/ayuda/H_PRINT_MENU|H_PRINT_MENU]]
