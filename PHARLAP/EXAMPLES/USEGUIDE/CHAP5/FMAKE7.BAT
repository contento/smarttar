rm *.exe *.lib *.dll
call c:\pat\msc7
rem masm start;
cl -O -FPa -ALu -Gs -Lp -c ftestdll.c
link /noi start ftestdll,ftestdll.dll,,,ftestdll.def
implib ftestdll.lib ftestdll.dll
cl -AL -Lp ftrydll.c -link ftestdll.lib
run286 ftrydll
