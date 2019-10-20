CFLAGS = -g -Wall -Werror
INCLUDES = -lm
LFLAGS = 
MAIN = main
PROGRAM = simplecal


all: $(MAIN)

$(MAIN): src/$(MAIN).c
	$(CC) $< $(CFLAGS) $(INCLUDES) $(LFLAGS) -o $(PROGRAM)

format:
	astyle \
		--align-pointer=type \
		--align-reference=type \
		--fill-empty-lines \
		--indent=force-tab=4 \
		--indent-switches \
		--min-conditional-indent=2 \
		--suffix=none \
		src/*.c

install:
	cp $(PROGRAM) /usr/bin/$(PROGRAM)
	chmod 755 /usr/bin/$(PROGRAM)

uninstall:
	$(RM) /usr/bin/$(PROGRAM)

clean:
	$(RM) $(PROGRAM)
