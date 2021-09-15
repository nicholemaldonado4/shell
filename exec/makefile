all: exec_cmd exec_from_file

exec_cmd: exec_cmd.h exec_cmd.c ../common/llist.h ../common/bool.h ../cmds/cmd.h ../cmds/sub_cmd.h ../common/str_utils.h ../zombies/zombies.h exec_from_file.h ../common/shell_consts.h
	gcc -c -I./ -I../common -I../cmds -I../zombies exec_cmd.c

exec_from_file: exec_from_file.h exec_from_file.c ../cmds/sub_cmd.h ../common/llist.h ../common/bool.h ../common/str_utils.h ../cmds/cmd.h ../cmd_parser/cmd_parser.h
	gcc -c -I./ -I../common -I../cmds -I../cmd_parser -I../zombies exec_from_file.c

clean:
	rm -f *.o