## Zinc Interface Library version 3.00 makefile
## DESIGN.EXE, WDESIGN.EXE
## COPYRIGHT (C) 1990-1992.  All Rights Reserved.
## Zinc Software Incorporated.  Pleasant Grove, Utah  USA

## Uses Microsoft C++ 7.00

## Compiler and linker: (Add -Zi to CPP_OPTS and /CO to LINK_OPTS for debug.)
CPP=cl
LINK=link
LIBRARIAN=lib

DOS_CPP_OPTS=-c -AL -BATCH -Gs -Os -nologo #-Fpzil.pch -Yu
DOS_LINK_OPTS=/NOD /NOI /BATCH /SEGMENTS:300 /DYNAMIC:475 /STACK:8192
#DOS_LINK_OPTS=/BATCH /SEGMENTS:256 /INFO /STACK:8192
DOS_LIB_OPTS=/NOI
DOS_OBJS=
DOS_LIBS=dos_zil dos_gfx ms_lgfx llibce graphics 

.cpp.obj:
	$(CPP) $(DOS_CPP_OPTS) $<

WIN_CPP_OPTS=-c -AL -BATCH -Gsw -Os -nologo -DWINVER=0x0300 #-Fpzilw.pch -Yu
WIN_LINK_OPTS=/NOD /NOI /BATCH /SEGMENTS:256 /STACK:8192
WIN_LIB_OPTS=/NOI
WIN_RC_OPTS=-k -30
WIN_OBJS=
WIN_LIBS=win_zil libw llibcew 
WIN_RC=rc

.cpp.obw:
	$(CPP) $(WIN_CPP_OPTS) -Fo$*.obw $<

WINDLL_CPP_OPTS=-c -AL -BATCH -Gsw -nologo -DWINVER=0x0300 -D__DLL__ # -Fpzilwd.pch -Yu
WINDLL_LINK_OPTS=/NOD /NOI /BATCH /SEGMENTS:256 /Ld /STACK:8192
WINDLL_LIB_OPTS=/NOI
WINDLL_OBJS=
WINDLL_LIBS=libw llibcew 

.cpp.obd:
	$(CPP) $(WINDLL_CPP_OPTS) -Fo$*.obd $<

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make the design program for Microsft C++ type:
	@echo nmake -fmicrosft.mak dos
	@echo nmake -fmicrosft.mak dos16
	@echo nmake -fmicrosft.mak windows
	@echo nmake -f mscwnt.mak winnt
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

# ----- Dos Extender ---------------------------------------------------------
D16_CPP=cl
D16_LINK=link
D16_LIBRARIAN=lib

D16_CPP_OPTS=-c -AL -BATCH -Gs2 -nologo -DDOSX286 #-Fpzil16.pch -Yu
D16_LINK_OPTS=/NOD /NOI /BATCH /SEGMENTS:256 /STACK:8192 /FAR
D16_LIB_OPTS=/NOI

# --- Use the next line for UI_GRAPHICS_DISPLAY ---
D16_OBJS=graphp.obj
D16_LIBS=phapi d16_zil d16_gfx ms_16gfx llibcep graphics
# --- Use the next lines for UI_MSC_DISPLAY ---
#D16_OBJS=graphp.obj
#D16_LIBS=phapi llibcep graphics d16_msc d16_zil

D16_BIND=bind286
PHARLAP_RTL=c:\phar286\rtk
D16_LOAD=run286a

.cpp.o16:
	$(D16_CPP) $(D16_CPP_OPTS) -Fo$*.o16 $<

.rc.res:
	rc -r $<

dos: design.exe
	copy design.exe ..\..\bin
	copy p_design.znc ..\..\bin

windows: wdesign.exe
	copy wdesign.exe ..\..\bin
	copy p_design.znc ..\..\bin

dos16: design16.exe
	copy design16.exe ..\..\bin
	copy p_design.znc ..\..\bin

design.exe: z_design.obj \
	z_bitobj.obj d_bitobj.obj z_scombo.obj z_config.obj z_ctrl.obj \
	z_dialog.obj z_cerror.obj z_file.obj z_frame.obj z_helped.obj \
	z_help1.obj z_hlpbar.obj z_hlpmsg.obj z_image.obj z_image1.obj \
	z_jump.obj z_objlst.obj z_resrc.obj z_util.obj z_object.obj \
	d_object.obj z_advanc.obj z_flag.obj z_import.obj z_image2.obj
	$(LINK) $(DOS_LINK_OPTS) @<<zil.rsp
$(DOS_OBJS)+z_design.obj+
z_bitobj.obj+d_bitobj.obj+z_scombo.obj+z_config.obj+z_ctrl.obj+
z_dialog.obj+z_cerror.obj+z_file.obj+z_frame.obj+z_helped.obj+
z_help1.obj+z_hlpbar.obj+z_hlpmsg.obj+z_image.obj+z_image1.obj+
z_jump.obj+z_objlst.obj+z_resrc.obj+z_util.obj+z_object.obj+d_object.obj+
z_advanc.obj+z_flag.obj+z_import.obj+z_image2.obj
$*, , $(DOS_LIBS)
msc_ovl.def
<<
#NUL
#<<

wdesign.exe: z_design.obw \
	z_bitobj.obw w_bitobj.obw z_scombo.obw z_config.obw z_ctrl.obw z_dialog.obw \
	z_file.obw z_cerror.obw z_frame.obw z_helped.obw z_help1.obw \
	z_hlpbar.obw z_hlpmsg.obw z_image.obw z_image1.obw z_jump.obw \
	z_objlst.obw z_resrc.obw z_util.obw z_object.obw w_object.obw z_advanc.obw \
	z_flag.obw z_import.obw z_image2.obw
	$(LINK) $(WIN_LINK_OPTS) @<<zil.rsp
$(WIN_OBJS)+z_design.obw+
z_bitobj.obw+w_bitobj.obw+z_scombo.obw+z_config.obw+z_ctrl.obw+z_dialog.obw+
z_file.obw+z_cerror.obw+z_frame.obw+z_helped.obw+z_help1.obw+z_hlpbar.obw+
z_hlpmsg.obw+z_image.obw+z_image1.obw+z_jump.obw+z_objlst.obw+z_resrc.obw+
z_util.obw+z_object.obw+w_object.obw+z_advanc.obw+z_flag.obw+z_import.obw+z_image2.obw
$*, , $(WIN_LIBS)
wdesign.def
<<
	$(WIN_RC) $(WIN_RC_OPTS) $*.rc $*.exe

design16.exe: z_design.o16 \
	z_bitobj.o16 d_bitobj.o16 z_scombo.o16 z_config.o16 z_ctrl.o16 \
	z_dialog.o16 z_cerror.o16 z_file.o16 z_frame.o16 z_helped.o16 \
	z_help1.o16 z_hlpbar.o16 z_hlpmsg.o16 z_image.o16 z_image1.o16 \
	z_jump.o16 z_objlst.o16 z_resrc.o16 z_util.o16 z_object.o16 d_object.o16\
	z_advanc.o16 z_flag.o16 z_import.o16 z_image2.o16
	$(LINK) $(D16_LINK_OPTS) @<<zil.rsp
$(D16_OBJS)+z_design.o16+
z_bitobj.o16+d_bitobj.o16+z_scombo.o16+z_config.o16+z_ctrl.o16+
z_dialog.o16+z_cerror.o16+z_file.o16+z_frame.o16+z_helped.o16+
z_help1.o16+z_hlpbar.o16+z_hlpmsg.o16+z_image.o16+z_image1.o16+
z_jump.o16+z_objlst.o16+z_resrc.o16+z_util.o16+z_object.o16+d_object.o16+
z_advanc.o16+z_flag.o16+z_import.o16+z_image2.o16
$*, , $(D16_LIBS)
NUL
<<
	$(D16_BIND) @<<zil.rsp
$(PHARLAP_RTL)\$(D16_LOAD)
$*
-dll $(PHARLAP_RTL)\moucalls
     $(PHARLAP_RTL)\int33
     $(PHARLAP_RTL)\doscalls
<<
	@del zil.rsp

## Build the pre-compiled header files:
zil.pch: ui_dsn.hpp
	@echo @<<hdrfile.cpp
#define OEMRESOURCE
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <direct.h>
#include <errno.h>
#include <fcntl.h>
#include <graph.h>
#include <io.h>
#include <limits.h>
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <time.h>
#include <new.h>
#define USE_RAW_KEYS
#include "ui_dsn.hpp"
#define USE_HELP_CONTEXTS
#include "design.hpp"
<<
	$(CPP) $(DOS_CPP_OPTS) -Yc hdrfile.cpp
	@del hdrfile.obj


## Build the pre-compiled header files:
zilw.pch: ui_dsn.hpp
	@echo @<<hdrfile.cpp
#define OEMRESOURCE
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <direct.h>
#include <errno.h>
#include <fcntl.h>
#include <graph.h>
#include <io.h>
#include <limits.h>
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <time.h>
#define USE_RAW_KEYS
#include "ui_dsn.hpp"
#define USE_HELP_CONTEXTS
#include "design.hpp"
<<
	$(CPP) $(WIN_CPP_OPTS) -Yc hdrfile.cpp
	@del hdrfile.obj


## Build the pre-compiled header files:
zilwd.pch: ui_dsn.hpp
	@echo @<<hdrfile.cpp
#define OEMRESOURCE
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <direct.h>
#include <errno.h>
#include <fcntl.h>
#include <graph.h>
#include <io.h>
#include <limits.h>
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <time.h>
#define USE_RAW_KEYS
#include "ui_dsn.hpp"
<<
	$(CPP) $(WINDLL_CPP_OPTS) -Yc hdrfile.cpp
	@del hdrfile.obj

## Build the pre-compiled header files:
zil16.pch: ui_dsn.hpp
	@echo @<<hdrfile.cpp
#define OEMRESOURCE
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <direct.h>
#include <errno.h>
#include <fcntl.h>
#include <graph.h>
#include <io.h>
#include <limits.h>
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <time.h>
#define USE_RAW_KEYS
#include "ui_dsn.hpp"
#define USE_HELP_CONTEXTS
#include "design.hpp"
<<
	$(D16_CPP) $(D16_CPP_OPTS) -Yc hdrfile.cpp
	@del hdrfile.obj

