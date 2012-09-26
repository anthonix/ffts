all: test

test: test.c ../src/.libs/libffts.a
	$(CC) $(CFLAGS) test.c -o test -lm ../src/.libs/libffts.a -I../include

clean:
	-$(RM) -f test.o test

distclean:


