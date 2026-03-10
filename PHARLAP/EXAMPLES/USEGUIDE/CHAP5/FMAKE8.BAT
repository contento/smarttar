rm *.exe *.lib *.dll
call c:\pat\msc8
rem masm start;
cl -FPa -ALu -Gs -Lp -c ftestdll.c
link start ftestdll,ftestdll.dll,,,ftestdll.def
implib ftestdll.lib ftestdll.dll
cl -AL -Lp -c ftrydll.c
link ftrydll,ftrydll,ftrydll,ftestdll /nod:llibce llibcep;
run286 ftrydll
