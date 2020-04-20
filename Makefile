all: note

note: note.c
	gcc -o note -ldb $^

install:
	cp -f note /usr/bin
	chmod +x /usr/bin/note

.PHONY: clean

clean:
	rm -f note