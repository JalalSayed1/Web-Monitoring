tldmonitor: tldmonitor.o date.o tldlist.o
	clang -Wall -Werror -o tldmonitor tldmonitor.o date.o tldlist.o

date.o: date.h date.c
	clang -Wall -Werror -o date.o -c date.c

tldlist.o: tldlist.h tldlist.c
	clang -Wall -Werror -o tldlist.o -c tldlist.c

tldmonitor.o: tldmonitor.c date.h tldlist.h
	clang -Wall -Werror -o tldmonitor.o -c tldmonitor.c

# for testing:
ck1tldmonitor: tldmonitor.c date.c tldlist.c
	clang -Wall -Werror -fsanitize=address -fno-omit-frame-pointer -o1 -g -o ck1tldmonitor.o tldmonitor.c date.c tldlist.c

ck2tldmonitor: tldmonitor.c date.c tldlist.c
	clang -Wall -Werror -fsanitize=memory -fno-omit-frame-pointer -o1 -g -o ck2tldmonitor.o tldmonitor.c date.c tldlist.c

ck3tldmonitor: tldmonitor.c date.c tldlist.c
	clang -Wall -Werror -fsanitize=undefined -fno-omit-frame-pointer -o1 -g -o ck3tldmonitor.o tldmonitor.c date.c tldlist.c

dtldmonitor: tldmonitor.c date.c tldlist.c
	clang -Wall -Werror -g -o dtldmonitor.o tldmonitor.c date.c tldlist.c

datetests: test_date.c date.c
	clang -Wall -Werror -g -o datetests.o test_date.c

tldtests: test_tldlist.c tldlist.c date.c
	clang -Wall -Werror -g -o tldtests.o test_tldlist.c

clean:
	rm -f *.o tldmonitor
