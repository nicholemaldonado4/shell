all: exec_cmd

exec_cmd: exec_cmd.h exec_cmd.c ../common/bool.h ../common/shell_consts.h ../common/str_utils.h
	gcc -c -I../common exec_cmd.c

clean:
	rm -f *.o