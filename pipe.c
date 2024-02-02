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
		if (pipe(pipefd) == -1) {
			perror("pipe error");
			return errno;
		}

		for (int i = 0; i < pipectr; i++) {
			printf("temp\n");
			
			int ret = fork();
			if (ret < 0) {
				perror("fork error");
				return errno;
			} else if (ret == 0) {
				printf("child process\n");
				close(pipefd[0]); // Close unused read end
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[1]); // Close duplicated write end

				if (execlp(argv[i + 1], argv[i + 1], NULL) == -1) {
					perror("execlp error");
					exit(errno);
				}
			} else {
				close(pipefd[1]); // Close unused write end
				wait(NULL); // Wait for child process to finish
			}
		}

		// Last argument
		int ret = fork();
		if (ret < 0) {
			perror("fork error");
			return errno;
		} else if (ret == 0) {
			printf("duped last arg\n");
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			close(pipefd[1]);
			printf("%s\n", argv[argc - 1]);
			
			if (execlp(argv[argc - 1], argv[argc - 1], NULL) == -1) {
				perror("execlp error");
				exit(errno);
			}
		} else {
			close(pipefd[0]);
			close(pipefd[1]);
			wait(NULL);
		}

	

	}
} 