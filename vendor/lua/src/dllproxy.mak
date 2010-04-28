SFX = 51
LIBNAME = lua$(SFX)

USE_DLL = Yes
LIBS = lua5.1
LDIR = ../lib/$(TEC_UNAME)
LFLAGS = /NOENTRY
DEF_FILE = lua5.1.def

# These are only to build import libraries that use the vc6 DLL.

mingw3-dll:            
	@mkdir -p ../lib/mingw3.dll
	@cd ../lib/dll
	@dlltool -d lua5.1.def -D lua$(SFX).dll -l ../lib/mingw3.dll/liblua$(SFX).a
	@cd ../src

bc56-dll:                    
	@mkdir -p ../lib/bc56.dll
	@$(BC56)/bin/implib -a ../lib/bc56.dll/lua$(SFX).lib ../lib/dll/lua$(SFX).dll
