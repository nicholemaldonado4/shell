all: redirection sub_cmd cmd

redirection: redirection.h redirection.c ../common/llist.h
	gcc -c -I./ -I../common redirection.c

sub_cmd: sub_cmd.h sub_cmd.c ../common/bool.h redirection.h ../common/llist.h
	gcc -c -I./ -I../common sub_cmd.c

cmd: cmd.h cmd.c ../common/bool.h ../common/llist.h sub_cmd.h redirection.h
	gcc -c -I./ -I../common cmd.c

clean:
	rm -f *.o