all: redirection cmd

redirection: redirection.h redirection.c ../common/llist.h
	gcc -c -I./ -I../common redirection.c

cmd: cmd.h cmd.c ../common/bool.h ../common/llist.h redirection.h ../common/shell_consts.h
	gcc -c -I./ -I../common cmd.c

clean:
	rm -f *.o