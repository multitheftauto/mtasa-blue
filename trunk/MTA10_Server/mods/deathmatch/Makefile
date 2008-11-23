#### Start of system configuration section ####

# use gcc or gcc-3.3
# ! -DLINUX for Linux compilations, change whenever compiled on another OS

CC = gcc
LD = gcc
PROG = deathmatch.so
CXX = g++
CPP = g++
NASM = nasm

# System specific paths
USR_LIB=/usr/lib

# Compiler flags
# NOTE: add -g for debug, remove for release!
#CPPFLAGS = -DLINUX -MD -masm=intel -Wall -I../../dependencies -I../../dependencies/pcre/include -I../../dependencies/pme -I../../dependencies/lua -I../../dependencies/sqlite -I../../dependencies/zlib -I../../dependencies/json -Ilogic/common -I../../core/net -I../../core/server
CPPFLAGS = -DLINUX -MD -Wall -I../../dependencies -I../../dependencies/pcre/include -I../../dependencies/pme -I../../dependencies/lua -I../../dependencies/sqlite -I../../dependencies/zlib -I../../dependencies/json -Ilogic/common -I../../core/net -I../../core/server
NASMFLAGS = -f elf -O1
LDFLAGS = -fPIC -shared -Wl,-soname,$(PROG).1,-R./
OBJ_LIBS = logic/lib/ASEQueryMTA.o
LIBS = 	$(OBJ_LIBS) \
	$(USR_LIB)/libjson.a \
	$(USR_LIB)/libsqlite3.a \
	$(USR_LIB)/libehs.a \
	$(USR_LIB)/libpme.a \
	$(USR_LIB)/libpcre.a \
	$(USR_LIB)/libz.a \
	../../dependencies/lua/src/liblua.a \
	../../dependencies/curl-7.17.1/lib/.libs/libcurl.a \
	-ldl -lpthread
LIBS_STATIC = $(LIBS)

#### End of system configuration section ####

#### Source and object files

SRC_ASM = 		logic/common/CMatrixASM.S
SRC_PLATFORM =		../../core/Platform.cpp
SRC_LOCAL =		$(wildcard ./*.cpp)
SRC_LOGIC =		$(wildcard logic/*.cpp)
SRC_PACKETS =		$(wildcard logic/packets/*.cpp)
SRC_LUA =		$(wildcard logic/lua/*.cpp)
SRC_LUADEFS =		$(wildcard logic/luadefs/*.cpp)
SRC_DMPACKETS =		$(wildcard logic/dmpackets/*.cpp)
SRC_COMMON =		$(wildcard logic/common/*.cpp)
SRC_UTILS =		$(wildcard utils/*.cpp)
SRC_UTILS_ZIP =		$(wildcard utils/zip/*.c)
SRC_UTILS_ZIP_CPP =	$(wildcard utils/zip/*.cpp)
OBJ_ASM = 		$(patsubst %.S,%.o,$(SRC_ASM))
OBJ_PLATFORM =		$(patsubst %.cpp,%.o,$(SRC_PLATFORM))
OBJ_LOCAL =		$(patsubst %.cpp,%.o,$(SRC_LOCAL))
OBJ_LOGIC =		$(patsubst %.cpp,%.o,$(SRC_LOGIC))
OBJ_PACKETS =		$(patsubst %.cpp,%.o,$(SRC_PACKETS))
OBJ_LUA = 		$(patsubst %.cpp,%.o,$(SRC_LUA))
OBJ_LUADEFS = 		$(patsubst %.cpp,%.o,$(SRC_LUADEFS))
OBJ_DMPACKETS =		$(patsubst %.cpp,%.o,$(SRC_DMPACKETS))
OBJ_COMMON =		$(patsubst %.cpp,%.o,$(SRC_COMMON))
OBJ_UTILS =		$(patsubst %.cpp,%.o,$(SRC_UTILS))
OBJ_UTILS_ZIP =		$(patsubst %.c,%.o,$(SRC_UTILS_ZIP))
OBJ_UTILS_ZIP_CPP =	$(patsubst %.cpp,%.o,$(SRC_UTILS_ZIP_CPP))
OBJS	= $(OBJ_LOCAL) $(OBJ_LOGIC) $(OBJ_PACKETS) $(OBJ_LUA) \
	  $(OBJ_DMPACKETS) $(OBJ_UTILS) $(OBJ_COMMON) \
	  $(OBJ_LUADEFS) $(OBJ_PLATFORM) $(OBJ_UTILS_ZIP) \
	  $(OBJ_UTILS_ZIP_CPP)
SRCS	= $(SRC_LOCAL) $(SRC_LOGIC) $(SRC_PACKETS) $(SRC_LUA) \
	  $(SRC_DMPACKETS) $(SRC_UTILS) $(SRC_COMMON) \
	  $(SRC_LUADEFS) $(SRC_PLATFORM) $(SRC_UTILS_ZIP) \
	  $(SRC_UTILS_ZIP_CPP)
DEPS	= $(patsubst %.o,%.d,$(OBJS))

#### Make rules

static: dm_static
all : dm
debug : dm_debug

dm_static : $(OBJS)
	$(NASM) $(NASMFLAGS) -o $(OBJ_ASM) $(SRC_ASM)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $(PROG) $(OBJS) $(OBJ_ASM) $(LIBS_STATIC)

dm : $(OBJS)
	$(NASM) $(NASMFLAGS) -o $(OBJ_ASM) $(SRC_ASM)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $(PROG) $(OBJS) $(OBJ_ASM) $(LIBS)

dm_debug : $(OBJS)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -g -o $(PROG) $(OBJS) $(LIBS)

clean :
	@find . -name "*.o" | grep -v $(OBJ_LIBS) | xargs rm
	@find . -name "*.d" | xargs rm

-include $(DEPS)
