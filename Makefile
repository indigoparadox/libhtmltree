
all: parser-test

parser-test: parser.o bstrlib.o main.o
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -f *.[o] parser-test

