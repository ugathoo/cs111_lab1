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
		return errno;
	}
	//one program only edge case
	else if(argc == 2){
		//check for valid second argument
		if(execlp(argv[1], argv[1], NULL) == -1){
			return errno;
		}
	}
	//exactly two programs edge case
	else if(argc == 3){
		int pipefd[2];
		if(pipe(pipefd) == -1){
			return errno;
		}
		int ret = fork();
		if(ret < 0){
			return errno;
		}
		else if (ret == 0){
			printf("Child process\n");
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[0]);
			close(pipefd[1]);
			//check valid arg
			if(execlp(argv[1], argv[1], NULL) == -1){
				return errno;
			}
			else{
				printf("parent process\n");
				close(pipefd[1]);
				int ret2 = fork();
				if (ret2 < 0){
					return errno;
				}
				else if (ret2 == 0){
					printf("parent child process\n");
					dup2(pipefd[0], STDIN_FILENO);
					close(pipefd[0]);
					if(execlp(argv[2], argv[2], NULL) == -1){
						return errno;
					}
				}
				else{
					close(pipefd[0]);
				}
			}
		}
	}
	

	// TODO: it's all yours
	return 0;
}
