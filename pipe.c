#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int pipefd[2];

	//check for valid arg length
	if(argc == 1){
		printf("invalid number of arguments\n");
		return errno.EINVAL;
	}
	//one program only edge case
	else if(argc == 2){
		//check for valid second argument
		if(execlp(argv[1], argv[1], NULL) == -1){
			return errno.EINVAL;
		}
	}
	

	// TODO: it's all yours
	return 0;
}
