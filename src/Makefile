PROG = slists
CFLAGS = -Wall -g -ansi -pedantic
OBJMODULES = file_data.o file_parser.o section_dbl.o item_dbl.o config.o\
			 util.o command.o

.PHONY: clean install

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

$(PROG): main.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@

install:
	install $(PROG) /usr/local/bin

clean:
	rm $(PROG) *.o
