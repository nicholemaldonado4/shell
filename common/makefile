all: llist shell_consts str_utils

llist: llist.h llist.c bool.h
	gcc -c llist.c 

shell_consts: shell_consts.h shell_consts.c
	gcc -c shell_consts.c

str_utils: str_utils.h str_utils.c
	gcc -c str_utils.c

clean:
	rm -f *.o