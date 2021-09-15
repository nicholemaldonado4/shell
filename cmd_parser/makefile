all: cmd_parser

cmd_parser: cmd_parser.h cmd_parser.c ../common/bool.h ../common/llist.h ../cmds/redirection.h ../cmds/cmd.h ../common/str_utils.h ../cmds/sub_cmd.h ../common/shell_consts.h
	gcc -c -I./ -I../common -I../cmds cmd_parser.c

clean:
	rm -f *.o