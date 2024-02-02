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
		
		//printf("before loop\n");
 		for (int i = 0; i < pipectr; i+=1) {
			//printf("temp\n");
			if (pipe(pipefd) == -1) {
				printf("error pipe\n");
				return errno;
			}
			int ret = fork();
			//printf("forked\n");
			//with the wait, forked prints twice, then hangs on the second iteration after printing child process
			if (ret < 0) {
				printf("error forking\n");
				return errno;
			} else if (ret == 0) {
				//printf("child process\n");
				dup2(pipefd[0], STDIN_FILENO);
				close(pipefd[0]);
				dup2(pipefd[1], STDOUT_FILENO); //write to pipefd[1], read from STDOUT_FILENO
				//printf("hit dup2\n"); //with wait this isn't ever printed, hangs @ dup2 call
				close(pipefd[1]);
				//printf("duped\n");
				int id = i+1;
				if (execlp(argv[id], argv[id], NULL) == -1) {
					printf("2 args error\n");
					return errno;
				}
				//printf("exited\n");
			} else {
				/*int status = 0;
				int pid = ret;
				waitpid(pid, &status, 0);
				printf("cleared wait\n");
				printf("%d\n", WEXITSTATUS(status));
				printf("cleared status print\n");*/
				dup2(pipefd[0], STDIN_FILENO);
				//printf("hit after dup2\n");
				//if I comment out wait in both places, it passes through all the way to duped last arg, then prints argv[argc-1] and then exits? it doesn't print anything after which i don't understand
			}
 		}

		//last arg
		int ret = fork();
		//printf("hit last arg\n");
		if (ret < 0) {
			printf("error forking\n");
			return errno;
		} else if (ret == 0) {
			dup2(pipefd[0], STDIN_FILENO);
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[0]);
			close(pipefd[1]);
			//printf("duped last arg\n");
			//close(pipefd[1]);
			//printf("%s\n", argv[argc-1]); //with both waits commented out, this prints, then it exits
			if (execlp(argv[argc-1], argv[argc-1], NULL) == -1) {
				printf("2 args error\n");
				exit(errno);
			}
			//printf("exited last arg else if\n");
		} 
		else {
			close(pipefd[0]);
			int status = 0;
			int pid = ret;
			waitpid(pid, &status, 0);
			printf("cleared wait last arg\n");
			//printf("%d\n", WEXITSTATUS(status));
			
		}
 	}
} 