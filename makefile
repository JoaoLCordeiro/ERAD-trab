CFLAGS = -Wall -g
OBJS = meuBcast.o

all: meuBcast

meuBcast: $(OBJS)
	mpic++ -o meuBcast $(CFLAGS) $(OBJS)

meuBcast.o: meuBcast.c chrono.c
	mpic++ $(CFLAGS) -c meuBcast.c

clean:
	-rm -f *~ *.o