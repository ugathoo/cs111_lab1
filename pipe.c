#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>

int main(int argc, char * argv[]) {
 	if (argc == 1) {
 		//printf("invalid number of args\n");
 		exit(EINVAL);
	//only 1 arg
 	} else if (argc == 2) {
 		if (execlp(argv[1], argv[1], NULL) == -1) {
 			//printf("invalid command for one arg\n");
 			return errno;
 		}
	//only 2 args
 	} else if (argc == 3) {
 		int pipefd[2];
 		if (pipe(pipefd) == -1) {
 			//printf("error pipe\n");
			return errno;
 		}
 		int ret = fork();
 		if (ret < 0) {
 			//printf("error forking\n");
 			return errno;
 		} else if (ret == 0) {
 			dup2(pipefd[1], STDOUT_FILENO); 
 			close(pipefd[0]);
 			close(pipefd[1]);
			if (execlp(argv[1], argv[1], NULL) == -1) {
				//printf("2 args error\n");
				return errno;
			}
 		} else {
 			//printf("parent process only 2 args\n");
 			close(pipefd[1]);
 			int ret2 = fork();
 			if (ret2 == -1)
 				//printf("error forking");
				return errno;
 			else if (ret2 == 0) {
 				//printf("parent child process only 2 args\n");
 				dup2(pipefd[0], STDIN_FILENO);
 				close(pipefd[0]);
				if (execlp(argv[2], argv[2], NULL) == -1) {
					//printf("2 args error\n");
					return errno;
				}
			} else {
 				//printf("parent process only 2 args\n");
				int status = 0;
				int pid = ret2;
				waitpid(pid, &status, 0);
				//printf("%d\n", WEXITSTATUS(status));
 				close(pipefd[0]);
 			}
 		}
	//more than 3 args
 	} else {
 		int pipefd[2];
 		int pipectr = argc - 2;
 		int ret;
 		for (int i = 0; i < pipectr; i+=1) {
			printf("temp");
 			//int pipefd[2];
			if (pipe(pipefd) == -1) {
				printf("error pipe\n");
				return errno;
			}
			ret = fork();
			if (ret < 0) {
				printf("error forking\n");
				return errno;
			} else if (ret == 0) {
				dup2(pipefd[1], STDOUT_FILENO); //write to pipefd[1], read from STDOUT_FILENO
				int id = i+1;
				if (execlp(argv[id], argv[id], NULL) == -1) {
					printf("2 args error\n");
					return errno;
				}
			} else {
				
				int status = 0;
				int pid = ret;
				waitpid(pid, &status, 0);
				printf("%d\n", WEXITSTATUS(status));
				dup2(pipefd[0], STDIN_FILENO);
				//close(pipefd[0]);
			}
 		}

		//last arg
		ret = fork();
		if (ret < 0) {
			printf("error forking\n");
			return errno;
		} else if (ret == 0) {
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[1]);
			if (execlp(argv[argc-1], argv[argc-1], NULL) == -1) {
				printf("2 args error\n");
				return errno;
			}
		} 
		else {
			close(pipefd[0]);
			int status = 0;
			int pid = ret;
			waitpid(pid, &status, 0);
			printf("%d\n", WEXITSTATUS(status));
			
		}
 	}
} 