call c:\pat\msc5
rm *.exe
rem masm start;
cl -ALu -Gs -Lp -c testdll.c
link /noi start testdll,testdll.dll,,,testdll.def
implib testdll.lib testdll.def
cl -Lp trydll.c -link testdll.lib
run286 trydll
