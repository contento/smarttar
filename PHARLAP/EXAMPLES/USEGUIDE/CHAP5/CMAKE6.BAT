call c:\pat\msc6
rm *.exe *.dll *.lib
rem j:\masm6\binb\masm start;
cl -ALu -Gs -Lp -c ctestdll.c
rem link /noi/noe start ctestdll,ctestdll.dll,,/nod:llibce llibcdll,ctestdll.def
link /noi start ctestdll,ctestdll.dll,,llibcdll,ctestdll.def
implib ctestdll.lib ctestdll.dll
cl -Lp trydll.c -link ctestdll.lib
run286 trydll
