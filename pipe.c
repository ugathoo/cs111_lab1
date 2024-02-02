#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>

int main(int argc, char * argv[]) {
 	if (argc == 1) {
 		exit(22);
	//only 1 arg
 	} else if (argc == 2) {
 		if (execlp(argv[1], argv[1], NULL) == -1) {
 			exit(errno);
 		}
	//only 2 args
 	} else if (argc == 3) {
 		int pipefd[2];
 		if (pipe(pipefd) == -1) {
			exit(errno);
 		}
 		int ret = fork();
 		if (ret < 0) {
 			exit(errno);
 		} else if (ret == 0) {
 			dup2(pipefd[1], STDOUT_FILENO); 
 			close(pipefd[0]);
 			close(pipefd[1]);
			if (execlp(argv[1], argv[1], NULL) == -1) {
				exit(errno);
			}
 		} else {
 			close(pipefd[1]);
 			int ret2 = fork();
 			if (ret2 == -1)
				exit(errno);
 			else if (ret2 == 0) {
 				dup2(pipefd[0], STDIN_FILENO);
 				close(pipefd[0]);
				if (execlp(argv[2], argv[2], NULL) == -1) {
					exit(errno);
				}
			} else {
				int status = 0;
				int pid = ret2;
				waitpid(pid, &status, 0);
				if (WIFEXITED(status))	
					exit(WEXITSTATUS(status));
				else if (WIFSIGNALED(status))
					exit(WTERMSIG(status));
				else if (WIFSTOPPED(status))	
					exit(WIFSTOPPED(status));
				
 				close(pipefd[0]);
 			}
 		}
	//more than 3 args
 	} else {
 		int pipefd[2];
 		int pipectr = argc - 2;
		
		for (int i = 0; i < pipectr; i++) {
			if (pipe(pipefd) == -1) {
				exit(errno);
			}
			int ret = fork();
			if (ret < 0) {
				perror("fork error");
				exit(errno);
			} else if (ret == 0) {
				dup2(pipefd[1], STDOUT_FILENO);
				if (execlp(argv[i + 1], argv[i + 1], NULL) == -1) {
					perror("execlp error");
					exit(errno);
				}
			} else {
				dup2(pipefd[0], STDIN_FILENO);
				close(pipefd[1]); // Close unused write end
				int pid = ret;
				int status = 0;
				waitpid(pid, &status, 0);
				if (WIFSIGNALED(status)) {
					//perror("waitpid error");
					exit(WEXITSTATUS(status));
				}

			}
		}
		

		// Last argument
		int ret = fork();
		if (ret < 0) {
			exit(errno);
		} else if (ret == 0) {
			dup2(pipefd[0], STDIN_FILENO);
			
			if (execlp(argv[argc - 1], argv[argc - 1], NULL) == -1) {
				exit(errno);
			}
		} else {
			close(pipefd[0]);
			int pid = ret;
			int status = 0;
			waitpid(pid, &status, 0);
			if (WIFEXITED(status))	
				exit(WEXITSTATUS(status));
			else if (WIFSIGNALED(status))
				exit(WTERMSIG(status));
			else if (WIFSTOPPED(status))	
				exit(WIFSTOPPED(status));
		}
	}
	return 0;
} 