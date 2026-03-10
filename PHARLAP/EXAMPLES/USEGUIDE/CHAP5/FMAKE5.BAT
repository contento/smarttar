call c:\pat\msc5
rm *.exe *.lib *.dll
rem j:\masm6\binb\masm start;
cl -FPa -ALu -Gs -Lp -c ftestdll.c
link /noi start ftestdll,ftestdll.dll,,,ftestdll.def
implib ftestdll.lib ftestdll.def
rem cl -AL -Lp ftrydll.c -link ftestdll.lib
cl -Lp ftrydll.c -link ftestdll.lib
run286 ftrydll
