#### Start of system configuration section ####

# Use gcc or gcc-3.3

CC = gcc
LD = gcc
PROG = mta_server
CXX = gcc
CPP = gcc

# Compiler flags
# NOTE: add -g for debug, be sure to delete it for release
CPPFLAGS = -MD -Wall -DMTA_SERVER
LDFLAGS = 
LIBS = -ldl -lstdc++

#### End of system configuration section ####

#### Source and object files

SRC_LOCAL =		$(wildcard ./*.cpp)
OBJ_LOCAL =		$(patsubst %.cpp,%.o,$(SRC_LOCAL))
OBJS	= $(OBJ_LOCAL) $(OBJ_XML) $(OBJ_SOCKETS)
SRCS	= $(SRC_LOCAL) $(SRC_XML) $(SRC_SOCKETS)
DEPS	= $(patsubst %.o,%.d,$(OBJS))

#### Make rules

all : launcher install

launcher : $(OBJS)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $(PROG) $(OBJS) $(LIBS)

deps : $(SRCS)
	$(CC) $(CPPFLAGS) -MD -E $(SRCS) > /dev/null

install :
	cp $(PROG) ../
	chmod +x ../$(PROG)

clean :
	@find . -name "*.o" | xargs rm
	@find . -name "*.d" | xargs rm

-include $(DEPS)
