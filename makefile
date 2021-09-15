all:
	(cd exec && make)
	(cd common && make)
	(cd cmd_parser && make)

minershell: minershell.o exec/exec_cmd.o common/str_utils.o cmd_parser/tokenizer.o
	gcc minershell.o exec/exec_cmd.o common/str_utils.o cmd_parser/tokenizer.o -o minershell

minershell.o: minershell.c common/shell_consts.h common/bool.h exec/exec_cmd.h common/str_utils.h cmd_parser/tokenizer.h
	gcc -c -I./common -I./exec -I./cmd_parser minershell.c

clean:
	(cd exec && make clean)
	(cd common && make clean)
	(cd cmd_parser && make clean)
	rm -f *.o