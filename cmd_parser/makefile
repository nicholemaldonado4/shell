all: cmd_parser tok_utils

cmd_parser: cmd_parser.h cmd_parser.c ../common/bool.h ../common/shell_consts.h ../common/str_utils.h ../cmds/cmd.h ../common/err_msg.h
	gcc -c -I../common -I../cmds cmd_parser.c

tok_utils: tok_utils.h tok_utils.c ../common/shell_consts.h
	gcc -c -I../common tok_utils.c

clean:
	rm -f *.o