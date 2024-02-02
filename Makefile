PROG = slists
CFLAGS = -Wall -g -ansi -pedantic
OBJMODULES = file_data.o file_parser.o section_dbl.o item_dbl.o

.PHONY: clean 

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

$(PROG): main.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm $(PROG) *.o
