CC = gcc
RM = rm -f
CFLAGS = -Wall

# Uncomment the following line for more debugging info.
#DEFS = -DDEBUG=1

# Uncomment the following line for MS-DOS builds.
# LIBS = com.a

all: gclink

clean:
	$(RM) gclink gclink.exe *.o

gclink: gclink.o serial.o protocol.o message.o slot.o
	$(CC) $(CFLAGS) $(DEFS) -o gclink gclink.o serial.o \
	protocol.o message.o slot.o $(LIBS)

gclink.o: gclink.c protocol.h slot.h
	$(CC) $(CFLAGS) $(DEFS) -c gclink.c
	
serial.o: serial.c
	$(CC) $(CFLAGS) $(DEFS) -c serial.c
	
protocol.o: protocol.c serial.h message.h commands.h
	$(CC) $(CFLAGS) $(DEFS) -c protocol.c
	
message.o: message.c
	$(CC) $(CFLAGS) $(DEFS) -c message.c
	
slot.o:	slot.c
	$(CC) $(CFLAGS) $(DEFS) -c slot.c
	
	
	
		
