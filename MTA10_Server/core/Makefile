#### Start of system configuration section ####

# Use gcc or gcc-3.3

CC = gcc
LD = gcc
PROG = core.so
CXX = gcc
CPP = gcc

USR_LIB = /usr/lib

# Compiler flags
# NOTE: add -g for debug, be sure to delete it for release
CPPFLAGS = -MD -Wall -DMTA_SERVER -Ixml
LDFLAGS = -fPIC -shared -Wl,-soname,$(OBJ).1
LIBS = 	$(OBJ_LIBS) \
        $(USR_LIB)/libcurses.a \
        $(USR_LIB)/libz.a \
        -ldl -lpthread -lstdc++
LIBS_STATIC = 	$(OBJ_LIBS) \
        $(USR_LIB)/libcurses.a \
        $(USR_LIB)/libz.a \
        -ldl -lpthread -lstdc++

#### End of system configuration section ####

#### Source and object files

SRC_LOCAL =		$(wildcard ./*.cpp)
SRC_XML =		$(wildcard xml/*.cpp)
SRC_SOCKETS =		$(wildcard sockets/*.cpp)
OBJ_LOCAL =		$(patsubst %.cpp,%.o,$(SRC_LOCAL))
OBJ_XML =		$(patsubst %.cpp,%.o,$(SRC_XML))
OBJ_SOCKETS =		$(patsubst %.cpp,%.o,$(SRC_SOCKETS))
OBJS	= $(OBJ_LOCAL) $(OBJ_XML) $(OBJ_SOCKETS)
SRCS	= $(SRC_LOCAL) $(SRC_XML) $(SRC_SOCKETS)
DEPS	= $(patsubst %.o,%.d,$(OBJS))

#### Make rules

static: core_static install
all : core install

core : $(OBJS)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $(PROG) $(OBJS) $(LIBS)

core_static : $(OBJS)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $(PROG) $(OBJS) $(LIBS_STATIC)

deps : $(SRCS)
	$(CC) $(CPPFLAGS) -MD -E $(SRCS) > /dev/null

install :
	cp $(PROG) ../
	chmod +x ../$(PROG)

clean :
	@find . -name "*.o" | xargs rm
	@find . -name "*.d" | xargs rm

-include $(DEPS)
