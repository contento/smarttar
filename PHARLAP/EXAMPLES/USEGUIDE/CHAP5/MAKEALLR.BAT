REM Chapter 12
cl -ALw -FPa -Gs2 -c realdll.c 
link /nod realdll, realdll.dll,,,realdll.def
markreal realdll.dll
implib realdll.lib realdll.def
cl -Lp testrdll.c -link realdll.lib
cl -Asnu -Gs -Lp p_api.c
cl -Gs2 rtestapi.c
masm -ml -DDOSX286 handler;
link handler,handler.dll,,,handler.def
markreal handler.dll
cl -Lp -DDOSX286 -c mouse.c
link mouse, mouse /noi,, /nod:slibce slibcep,mouse.def
