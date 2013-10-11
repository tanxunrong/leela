Common_cflags= -Wall -g -std=c99
Glib_cflags := $(shell pkg-config --cflags glib-2.0)
Event_cflags := $(shell pkg-config --libs libevent)
CFLAGS= $(Common_cflags) $(Glib_cflags) $(Event_cflags)

install : echo_client.o echo_server.o \
	time_client.o time_server.o uds.o
	cc -o echo_client echo_client.o
	cc -o echo_server echo_server.o 
	cc -o time_client time_client.o
	cc -o time_server time_server.o 
	cc -o uds uds.o -lglib-2.0

echo_client.o : echo_client.c myc.h
	cc -c echo_client.c $(CFLAGS)
echo_server.o : echo_server.c myc.h
	cc -c echo_server.c $(CFLAGS)
time_client.o : time_client.c myc.h
	cc -c time_client.c $(CFLAGS)
time_server.o : time_server.c myc.h
	cc -c time_server.c $(CFLAGS)
uds.o : uds.c myc.h
	cc -c uds.c $(CFLAGS)
clean:
	rm echo_client echo_client.o \
		echo_server echo_server.o
	rm time_client time_client.o \
		time_server time_server.o
	rm uds.o uds
