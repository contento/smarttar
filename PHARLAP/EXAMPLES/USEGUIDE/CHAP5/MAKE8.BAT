rm *.exe *.lib *.dll
call c:\pat\msc8
rem masm start;
cl -ALu -Gs -Lp -c testdll.c
link start testdll,testdll.dll,,,testdll.def
implib testdll.lib testdll.dll
cl -Lp -c trydll.c
link trydll,trydll,trydll,testdll /nod:slibce slibcep;
run286 trydll

