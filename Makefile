CFLAGS	= -O2 -Wall

test_fp: test_fp.o bbcfp.o
	$(CC) $(CFLAGS) -o test_fp test_fp.o bbcfp.o -lm
