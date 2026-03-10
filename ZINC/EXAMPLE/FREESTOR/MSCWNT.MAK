# FREESTOR example makefile

# Be sure to set the LIB and INCLUDE environment variables for Zinc, e.g.:
#    set INCLUDE=.;C:\ZINC\INCLUDE;C:\C700\INCLUDE
#    set LIB=.;C:\ZINC\LIB\MSCPP700;C:\C700\LIB

# ----- Usage --------------------------------------------------------------
usage:
	@echo ...........
	@echo ...........
	@echo To make this Zinc example for Microsoft C++ type:
	@echo nmake -fmicrosft.mak dos
	@echo nmake -fmicrosft.mak dos16
	@echo (Microsoft does not support OS/2)
	@echo (Windows environments do not support this example)
	@echo (Windows NT environments do not support this example)
	@echo ...........
	@echo ...........

# ----- Clean ---------------------------------------------------------------
clean:
	z_clean

