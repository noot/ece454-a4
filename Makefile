CC = g++
CFLAGS = -O3

all: randtrack randtrack_global_lock randtrack_tm

randtrack: list.h hash.h defs.h randtrack.cc
	$(CC) $(CFLAGS) randtrack.cc -o randtrack

randtrack_global_lock: list.h hash.h defs.h randtrack_global_lock.cc
	$(CC) $(CFLAGS) randtrack_global_lock.cc -o randtrack_global_lock -lpthread

randtrack_tm: list.h hash.h defs.h randtrack_tm.cc
	$(CC) $(CFLAGS) randtrack_tm.cc -o randtrack_tm -lpthread -fgnu-tm

clean:
	rm -f *.o randtrack randtrack_global_lock randtrack_tm randtrack_list_lock 
