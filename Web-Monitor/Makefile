tldmonitor: tldmonitor.o date.o tldlist.o
	clang -Wall -Werror -fsanitize=address -fno-omit-frame-pointer -01 -g -o tldmonitor tldmonitor.o date.o tldlist.o

date.o: date.h date.c
	clang -Wall -Werror -o date.o -c date.c

tldlist.o: tldlist.h tldlist.c
	clang -Wall -Werror -o tldlist.o -c tldlist.c

tldmonitor.o: tldmonitor.c date.h tldlist.h
	clang -Wall -Werror -o tldmonitor.o -c tldmonitor.c

# for testing:
datetests: test_date.c date.o tldlist.o
	clang -Wall -Werror -g -o datetest.o test_date.c

tldtests: tldlist.o date.o test_tldlist.c
	clang -Wall -Werror -g -o tldtest.o tldlist.c

clean:
	rm -f *.o tldmonitor
