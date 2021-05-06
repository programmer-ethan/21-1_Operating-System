//구현 상황: 기본 동작 및 pipe 완성,오류 처리.
#include <stdio.h> // printf()
#include <string.h> // perror(), strlen(), strncmp()
#include <unistd.h> // fork()
#include <sys/wait.h> // waitpid()
#include <stdlib.h> // exit()
#define MAX 1000
 
void *getcmd(char cmd[]){//getcmd &change last'\n' to 'null'
	void *ptr;
	usleep(500);
	printf("myshell >");
	ptr = fgets(cmd, MAX-1, stdin);
	if(cmd[strlen(cmd)-1] == '\n')
		cmd[strlen(cmd)-1] = 0;
 
	return ptr;
}
 
int main()
{
	char cmd[MAX];
	char *arg[MAX];//argv to given fork+exec
	char *arg2[MAX];//for cmd before pipe
	char *arg3[MAX];//for cmd after pipe
	char *token;// get each token<- parsed string
	pid_t pid,pid2;
	int status,status2;
	int count;//<-argc count
	int fd[2];// for pipe
	int isPipe=0;
	int pipeIdx=0; //the index of '|'
 
	while(getcmd(cmd)) { // while successful input
		count=0;
		isPipe=0;
		if(!strncmp("exit", cmd, 4)||!strncmp("quit", cmd, 4)){
			break;
		}
		if((pid = fork()) < 0) {
			perror("fork error");
		}
		else if(pid == 0) {
			token=strtok(cmd, " ");
			while(token!=NULL){
				if(!strncmp("|",token,1)){
						isPipe=1;
						pipeIdx=count;
				}
				arg[count++] = token;
				token = strtok(NULL, " ");
			}
			arg[count]=NULL;
			if(isPipe){
				int j=0,k=pipeIdx+1;
				while(j<pipeIdx){
					arg2[j]=arg[j];
					j++;
				}
				arg2[j]=NULL;
				j=0;
				while(k<count){
					arg3[j]=arg[k];
					k++; j++;
				}
				arg3[k]=NULL;
				pipe(fd);
				
				if((pid2 = fork()) < 0) {// secnd child process
					perror("fork error");
				}
				else if(pid2 == 0){// child2 of child1 for pipe
					close(0);
					close(fd[1]);
					dup(fd[0]);
					close(fd[0]);
					execvp(arg3[0],arg3);
					exit(2);
				}
				close(1);
				close(fd[0]);
				dup(fd[1]);
				close(fd[1]);
				execvp(arg2[0],arg2);
				waitpid(pid2, &status2,0);
				exit(0);
			}
			else if(!strncmp("cd", arg[0], 2)) {
				printf("%s",arg[1]);
				if(chdir(arg[1])){
					fprintf(stderr, "change directory error.\n");
				}
				exit(1);
			}
			else{
				execvp(arg[0], arg);
				exit(0);
			}
		}
		memset(cmd, 0,MAX);
		waitpid(pid, &status,0);
	}
	return 0;
}
