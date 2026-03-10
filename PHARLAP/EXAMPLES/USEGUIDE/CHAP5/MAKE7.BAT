rm *.exe *.lib *.dll
call c:\pat\msc7
rem masm start;
cl -ALu -Gs -Lp -c testdll.c
link /noi start testdll,testdll.dll,,,testdll.def
implib testdll.lib testdll.dll
cl -Lp trydll.c -link testdll.lib
run286 trydll

