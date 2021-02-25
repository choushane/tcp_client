
all: client

client: tcpclient.o
	$(CC) -o $@ $^ $(LDFLAGS) -lpthread

clean:
	rm -f *.o $(all)
