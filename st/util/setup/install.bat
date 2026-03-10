@echo off
cls
echo. Install (c) MicroDise¤o Ltda. 1995-2000.
echo.
choice /C:SN Est  seguro:
if errorlevel 2 goto end
cls
echo. Install (c) MicroDise¤o Ltda. 1995-2000.
echo.
echo. Instalando SmartTar 2.21.1 , favor espere ...
ctty nul
attrib -r -a -h -s c:\st
attrib -r -a -h -s c:\st\*.*
attrib -r -a -h -s c:\stold
attrib -r -a -h -s c:\stold\*.*
md c:\tmp
md c:\st
md c:\stold
move /y c:\st\*.* c:\stold
::
copy c:\stold\ph_info.dat  c:\st
copy c:\stold\ext_info.dat c:\st
copy c:\stold\rx.*         c:\st
copy c:\stold\rxx.*        c:\st
copy c:\stold\st.ini       c:\st
copy c:\stold\st.cfg       c:\st
copy c:\stold\*.inf        c:\st
copy c:\stold\st.log       c:\st
copy c:\stold\*.dll        c:\st
::
copy *.inf c:\st
::
if exist rain goto InstallIt
attrib -r -a -h -s c:\stx
attrib -r -a -h -s c:\stx\*.*
deltree /y c:\stx
md c:\stx
copy leame.txt   c:\stx
copy install.bat c:\stx
copy stx.ex_     c:\stx
echo Lluvia al Alba > c:\stx\rain
::
:InstallIt
::
c:
cd \stx
ren stx.ex_ stx.exe
stx -o -d -sGCC c:\st
copy leame.txt c:\st
ren stx.exe stx.ex_
cd \st
ctty con
setup
cls
echo.
echo. No olvide leer el archivo LEAME.TXT.
echo. Si desea verlo utilice el comando
echo.     LEAME [Enter]
echo.                             Gracias.
echo.
:end

