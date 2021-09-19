all: exec_cmd

exec_cmd: exec_cmd.h exec_cmd.c ../common/bool.h ../common/shell_consts.h ../common/str_utils.h ../cmds/cmd.h ../cmds/redirection.h ../common/llist.h ../common/err_msg.h
	gcc -c -I../common -I../cmds exec_cmd.c

clean:
	rm -f *.o