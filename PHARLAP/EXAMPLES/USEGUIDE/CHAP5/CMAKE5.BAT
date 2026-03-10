call c:\pat\msc5
rm *.exe *.lib *.dll
rem j:\masm6\binb\masm start;
cl -ALu -Gs -Lp -c ctestdll.c
link /noi/noe start ctestdll,ctestdll.dll,,/nod:llibce llibcdll,ctestdll.def
implib ctestdll.lib ctestdll.def
cl -Lp trydll.c -link ctestdll.lib
run286 trydll
