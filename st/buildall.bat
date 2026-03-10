@echo off
deltree /y deploy
sweep del *.sym
sweep del *.obj
sweep del *.bak
sweep del *.lib
sweep del *.swp
sweep del *.exe
sweep del *.tdw
md deploy               > NUL
md deploy\common        > NUL
md deploy\eda           > NUL
md deploy\demo          > NUL
md deploy\demo\dongle   > NUL
md deploy\demo\nodongle > NUL
REM
REM Non common files
REM
:EDA
call build QUIET -DEDA %1 %2 %3 %4 %5 %6 %7 %8 %9
move bin\st.exe deploy\eda\st.exe
move bin\*.dll  deploy\eda
:NODONGLE
call build QUIET -DDEMO -DNODONGLE %1 %2 %3 %4 %5 %6 %7 %8 %9
move bin\st.exe deploy\demo\nodongle\st.exe
:DONGLE
call build QUIET -DDEMO %1 %2 %3 %4 %5 %6 %7 %8 %9
move bin\st.exe deploy\demo\dongle\st.exe
:DEFAULT
call build QUIET %1 %2 %3 %4 %5 %6 %7 %8 %9
copy bin\st.exe deploy\common\st.exe
REM
REM Common Files
REM
cd util
call buildall
cd ..
copy util\gen\gen.exe deploy
copy util\stl\stl.exe deploy
copy util\stl\stleda.exe deploy
cd bin
gc res.dat
copy res.dat       ..\deploy\common
copy help.dat      ..\deploy\common
copy inf2dat.exe   ..\deploy\common
copy ini2cfg.exe   ..\deploy\common
copy update.exe    ..\deploy\common
copy dump.exe      ..\deploy\common
copy repair.exe    ..\deploy\common
copy setup.exe     ..\deploy\common
copy viewlog.exe   ..\deploy\common
copy *.dll         ..\deploy\common
cd ..
REM Create batch to zip files
copy versions.txt deploy\versions.txt
cd deploy
echo @echo off                                                       >  zipit.bat
echo pkzip -rPu common.zip  common\*.* stl*.exe gen.exe versions.txt >> zipit.bat
echo pkzip -rPu edademo.zip eda\*.* demo\*.*                         >> zipit.bat
cd ..

