SFX = 5.1
LIBNAME = lua$(SFX)

SRC = lapi.c lcode.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c \
	lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c \
	lundump.c lvm.c lzio.c \
	lauxlib.c lbaselib.c ldblib.c liolib.c lmathlib.c loslib.c ltablib.c \
	lstrlib.c loadlib.c linit.c

include lua_conf.inc

# These are only to build import libraries that use the vc6 DLL.

mingw3-dll:            
	@mkdir -p ../lib/mingw3.dll
	@cd ../lib/dll
	@dlltool -d lua$(SFX).def -D lua$(SFX).dll -l ../lib/mingw3.dll/liblua$(SFX).a
	@cd ../src

bc56-dll:                    
	@mkdir -p ../lib/bc56.dll
	@$(BC56)/bin/implib -a ../lib/bc56.dll/lua$(SFX).lib ../lib/dll/lua$(SFX).dll
