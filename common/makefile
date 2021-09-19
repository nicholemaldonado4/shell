all: str_utils llist err_msg

str_utils: str_utils.h str_utils.c
	gcc -c str_utils.c

llist: llist.h llist.c bool.h
	gcc -c llist.c 

err_msg: err_msg.h err_msg.c
	gcc -c err_msg.c 

clean:
	rm -f *.o