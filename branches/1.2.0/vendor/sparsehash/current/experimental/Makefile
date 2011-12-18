example: example.o libchash.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

.SUFFIXES: .c .o .h
.c.o:
	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

example.o: example.c libchash.h
libchash.o: libchash.c libchash.h
