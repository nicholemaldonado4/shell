all: cmd_parser

cmd_parser: cmd_parser.h cmd_parser.c ../common/bool.h ../common/shell_consts.h ../common/str_utils.h ../cmds/cmd.h ../common/err_msg.h
	gcc -c -I../common -I../cmds cmd_parser.c

clean:
	rm -f *.o