CFLAGS = -Wall -g
OBJSP = meuBcast.o
OBJSM = Bcast.o

all: meuBcast Bcast

meuBcast: $(OBJSP)
	mpic++ -o meuBcast $(CFLAGS) $(OBJSP)

Bcast: $(OBJSM)
	mpic++ -o Bcast $(CFLAGS) $(OBJSM)

meuBcast.o: meuBcast.c chrono.c
	mpic++ $(CFLAGS) -c meuBcast.c

Bcast.o: Bcast.c chrono.c
	mpic++ $(CFLAGS) -c Bcast.c

clean:
	-rm -f *~ *.o