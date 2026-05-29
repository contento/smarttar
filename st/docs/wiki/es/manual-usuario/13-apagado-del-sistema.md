---
title: "Apagado del Sistema"
lang: es
manual: guia-usuario-es
order: 13
---

# Apagado del Sistema
Siga este procedimiento para cerrar SmartTar de forma segura:

1.  **Verifique que todas las cabinas estén libres.** La vista debe mostrar todas las cabinas como *Libre*. Si hay llamadas activas, espere a que terminen.
2.  **Cierre el turno actual** si hay cambio de turno (ver Sección 10).
3.  **Imprima el recibo de turno** para verificación del supervisor si se requiere.
4.  Presione **Alt+F4** o cierre la ventana principal. Un diálogo de confirmación preguntará: *“Terminar la sesión de trabajo?”*.
5.  Haga clic en **Sí** para confirmar. La aplicación vacía todas las bases de datos, registra el cierre de sesión en la EEPROM y sale al DOS.

**Advertencia:** No apague el PC ni lo reinicie mientras SmartTar esté en ejecución, especialmente con llamadas activas. Hacerlo causará un evento de cierre anormal que deberá recuperarse en el siguiente arranque. Siempre salga de SmartTar correctamente antes de apagar.
