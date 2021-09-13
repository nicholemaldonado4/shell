all: tokenizer

tokenizer: tokenizer.h tokenizer.c ../common/bool.h ../common/shell_consts.h ../common/str_utils.h
	gcc -c -I../common tokenizer.c

clean:
	rm -f *.o