all:
	(cd exec && make)
	(cd common && make)
	(cd cmd_parser && make)
	(cd cmds && make)

minershell: minershell_o exec/exec_cmd.o common/str_utils.o cmd_parser/cmd_parser.o cmds/redirection.o cmds/cmd.o common/llist.o common/err_msg.o cmd_parser/tok_utils.o
	gcc minershell.o exec/exec_cmd.o common/str_utils.o cmd_parser/cmd_parser.o cmds/redirection.o cmds/cmd.o common/llist.o common/err_msg.o cmd_parser/tok_utils.o -o minershell

minershell_o: minershell.c common/shell_consts.h common/bool.h exec/exec_cmd.h common/str_utils.h cmd_parser/cmd_parser.h cmds/cmd.h common/err_msg.h
	gcc -c -I./common -I./exec -I./cmd_parser -I./cmds minershell.c

clean:
	(cd exec && make clean)
	(cd common && make clean)
	(cd cmd_parser && make clean)
	(cd cmds && make clean)
	rm -f *.o