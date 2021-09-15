all:
	(cd common && make)
	(cd cmd_parser && make)
	(cd cmds && make)
	(cd exec && make)
	(cd zombies && make)

main: main.o common/llist.o cmd_parser/cmd_parser.o cmds/cmd.o cmds/sub_cmd.o cmds/redirection.o common/shell_consts.o exec/exec_cmd.o common/str_utils.o exec/exec_from_file.o zombies/zombies.o
	gcc main.o common/llist.o cmd_parser/cmd_parser.o cmds/cmd.o cmds/sub_cmd.o cmds/redirection.o common/shell_consts.o exec/exec_cmd.o common/str_utils.o exec/exec_from_file.o zombies/zombies.o -o main

main.o: main.c cmd_parser/cmd_parser.h cmds/cmd.h common/llist.h cmds/cmd.h common/shell_consts.h common/bool.h exec/exec_cmd.h zombies/zombies.h
	gcc -c -I./cmd_parser -I./common -I./cmds -I./exec -I./zombies main.c

clean:
	(cd common && make clean)
	(cd cmd_parser && make clean)
	(cd cmds && make clean)
	(cd exec && make clean)
	(cd zombies && make clean)
	rm -f *.o