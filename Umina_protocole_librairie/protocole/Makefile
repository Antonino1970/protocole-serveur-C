CC ?= gcc
CFLAGS ?= -g #debug
#CFLAGS ?= -O3
VPATH = lib:.

# add warnings & add inc directory to the include path
override CFLAGS += -Wall -Wpedantic -Wextra -Iinc

.PHONY: all clean

all: proto-client proto-server

clean:
	find . -name '*.so*' -exec rm -v {} \+
	rm -fv proto-client proto-server

# compile c files with the lib dependency
proto-client proto-server: proto-%: src/proto-%.c -ltcp -llist -lserial
	$(CC) $(CFLAGS) -Llib -Wl,-rpath='$$ORIGIN/lib' -o $@ $^

# make the lib available unversioned
lib/libtcp.so: lib/libtcp.so.1
	ln -sf $(notdir $<) $@
lib/liblist.so: lib/liblist.so.1
	ln -sf $(notdir $<) $@
lib/libserial.so: lib/libserial.so.1
	ln -sf $(notdir $<) $@

# resolve links (here .so.1 -> .so.1.0)
lib/libtcp.so.1: lib/libtcp.so.1.0
	ldconfig -r lib -n .
lib/liblist.so.1: lib/liblist.so.1.0
	ldconfig -r lib -n .
lib/libserial.so.1: lib/libserial.so.1.0
	ldconfig -r lib -n .

# compile the library v1.0
lib/libtcp.so.1.0: lib/tcp-util.c inc/tcp-util.h
	$(CC) $(CFLAGS) -Wl,-soname,libtcp.so.1 -shared -fPIC -o $@ $<
lib/liblist.so.1.0: lib/list-util.c inc/list-util.h
	$(CC) $(CFLAGS) -Wl,-soname,liblist.so.1 -shared -fPIC -o $@ $<
lib/libserial.so.1.0: lib/serial-util.c inc/serial-util.h
	$(CC) $(CFLAGS) -Wl,-soname,libserial.so.1 -shared -fPIC -o $@ $<