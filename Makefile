Common_cflags= -Wall -g -std=c99
Glib_cflags := $(shell pkg-config --cflags glib-2.0)
Event_cflags := $(shell pkg-config --cflags libevent)
CFLAGS=$(Common_cflags) $(Glib_cflags) $(Event_cflags) 

Objs = echo_client.o echo_server.o \
	time_client.o time_server.o uds.o
Targets = $(patsubst %.o,%,$(Objs))

install : $(Objs)
	cc -o echo_client echo_client.o
	cc -o echo_server echo_server.o 
	cc -o time_client time_client.o
	cc -o time_server time_server.o 
	cc -o uds uds.o -lglib-2.0


$(Objs) : myc.h

$(Objs) : %.o : %.c
	cc -c $< $(CFLAGS) -o $@

clean:
	rm $(Objs)
	rm $(Targets)
