# Common Tecmake definitions for Lua 5.1

SFX = 5.1
PROJNAME = lua$(SFX)
OPT = Yes           # Turn optimization on
USE_NODEPEND = Yes  # Disable dependencies
NO_SCRIPTS = Yes    # Disable scripts and batch files creation
NO_DYNAMIC = no     # Build also dynamic libraries with gcc in Windows

DEFINES = 
                          
# All non-Windows (posix)                         
ifeq ($(findstring Win, $(TEC_SYSNAME)), )
  DEFINES += LUA_USE_POSIX
endif

# Linux Only
ifneq ($(findstring Linux, $(TEC_UNAME)), )
  DEFINES += LUA_USE_DLOPEN LUA_USE_READLINE
endif

# MacOS X
ifneq ($(findstring Darwin, $(TEC_UNAME)), )
  DEFINES += LUA_DL_DYLD
endif

# BSD Only
ifneq ($(findstring BSD, $(TEC_UNAME)), )
  DEFINES += LUA_USE_DLOPEN #LUA_USE_READLINE
endif
           
# Cygwin Only (POSIX build)
ifneq ($(findstring cygw, $(TEC_UNAME)), )
  DEFINES += LUA_USE_DLOPEN LUA_USE_READLINE
endif

# For Executables only
ifdef APPNAME
  # Disable strip
  STRIP = 
  
  APPTYPE = console

  ifneq ($(findstring Win, $(TEC_SYSNAME)), )
    SRC += lua.rc 
    SLIB += setargv.obj
    
    # In Windows, use the Dynamic Libraries
    USE_DLL = Yes
    LIBS += lua$(SFX)
    ifeq "$(TEC_SYSNAME)" "Win32"
      LDIR += ../lib/dll
    else
      LDIR += ../lib/dll8_64
    endif
  else
    ifneq ($(findstring cygw, $(TEC_UNAME)), )
#      SRC += lua.rc
      LIBS += lua$(SFX)
      LDIR += ../lib/$(TEC_UNAME)   
      LDFLAGS = -s
    else
      #UNIX Use Static Libraries
      SLIB += ../lib/$(TEC_UNAME)/liblua$(SFX).a
    endif
  endif
  
  ifneq ($(findstring Linux, $(TEC_UNAME)), )
    LIBS += dl 
    LFLAGS = -Wl,-E
    
    LIBS += readline history curses ncurses
  endif
  
  ifneq ($(findstring BSD, $(TEC_UNAME)), )
#    LIBS += dl 
    LFLAGS = -Wl,-E
  endif
  
  ifneq ($(findstring SunOS, $(TEC_UNAME)), )
    LIBS += dl 
  endif
  
  ifneq ($(findstring cygw, $(TEC_UNAME)), )
    LIBS += readline history
  endif

endif
