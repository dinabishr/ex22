#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#define BUFFER_LEN 1024

bool search_for_pipe(char **argv,char **input,char **out){
	bool found=false;
	printf("searching for pipes\n");
	int i;
	for(i=0;argv[i]!=NULL;i++){
		if(strcmp(argv[i],"|")==0) /*if pipe is found*/
		{
			found = true;
			argv[i]=NULL; 		/*set its location to null*/
			input[0] =argv[i-2];/*store first word*/	
			input[1]=argv[i-1];/*store word just before pipe*/
			out[0] = argv[i+1];/*store output parts after pipe*/
			out[1]=argv[i+2];
		}
		
	}

	return found;
}

bool echo_(char **argv,char *v[][1]){
	bool found = false;
	printf("i am here\n");
	int i;
	for(i=0;argv[i]!=NULL;i++){
		if(strcmp(argv[i],"echo")==0){/*search for echo command*/
			printf("echo found");
			int j;
			for(j=0;j<15;j++){

				if(strcmp(v[j][0],argv[i+2])==0){/*i plus 2 after dollar sign*/
					printf("v found!!");
					printf(v[j][1]);/*print corresponding va;ue stored*/
					found=true;
				}
			}
		}

	}
return found;

}


bool search_for_var(char **argv,char *variables[][1]){
	bool found = false;
	int i;
	for(i=0;argv[i]!=NULL;i++){
		if(strcmp(argv[i],"=")==0){ /*search for equal sign that indicates storing a new variable*/
			found =true;
			int j;
			for(j=0;j<15;j++){
				if(variables[j][0]==NULL){/*find an empty place in variables array*/
					variables[j][0]=argv[i-1];/*store the varibale name*/
					variables[j][1]=argv[i+1];/*store the variable value*/
				}
			}
		}
	}
	return found;
}
bool output_redirection(char ** argv,char *output,char *cmd){
	bool found=false;
	int i;
	for(i=0;argv[i]!=NULL ;i++){
		if(strcmp(argv[i],">")==0){/*search for output redirection sign*/
			argv[i]=NULL;
			cmd = argv[i-1];/*place value before > as input command*/
			strcpy(output,argv[i+1]);/*place value after > in output var to be used */
			found=true;
			break;
		}
	}
return found;
}


bool input_redirection(char **argv,char *input,char *cmd){
	bool found;	
	int i;
	for(i=0;argv[i]!=NULL;i++){
		if(strcmp(argv[i],"<")==0){/*search for input redirection sign*/
			argv[i]=NULL;
			cmd = argv[i-1];/*place value before < as input*/
			strcpy(input,argv[i+1]);/*place value after < as output*/
			found=true;
			break;
		}

	}
return found;

}

size_t read_command(char *cmd) {

	if(!fgets(cmd, BUFFER_LEN, stdin)) /*get command and put it in line*/
 	return 0; /*if user hits CTRL+D break*/
 	size_t length = strlen(cmd); /* get command length*/
	 if (cmd[length - 1] == '\n') cmd[length - 1] = '\0'; /* clear new line*/
	return strlen(cmd); /* return length of the command read*/
}

int build_args(char * cmd, char ** argv) {
	 char *token; /*split command into separate strings*/
	 token = strtok(cmd," ");
	 int i=0;
 while(token!=NULL){/* loop for all tokens*/
 	argv[i]=token; /* store token*/
	 token = strtok(NULL," "); /*get next token*/
	 i++; /*increment number of tokens*/
 }
 	argv[i]=NULL; /*set last value to NULL for execvp*/
  return i; /* return number of tokens*/
}

void set_program_path (char * path, char * bin, char * prog) {
	memset (path,0,1024); /* intialize buffer*/
	strcpy(path, bin); /*copy /bin/ to file path*/
	 strcat(path, prog); /*add program to path*/
	/* printf(path);*/
	 int i;
 for(i=0; i<strlen(path); i++) /*delete newline*/
	if(path[i]=='\n') path[i]='\0';
}

int main(){

 char line[BUFFER_LEN]; /*get command line*/
 char* argv[100]; /*user command*/
 char* bin= "/bin/"; /*set path at bin*/
 char path[1024]; /*full file path*/
 int argc; /*arg count*/
char in[100],out[100],command[100];/*used for in/out redirection*/
char* vars[15][1];/*fixed size max 15 variables can be stored*/
char *input[]={NULL,NULL,NULL};/*used for pipe*/
char *output[]={NULL,NULL,NULL};
bool p=false;/*check for pipe boolean*/
int fd[2];

while(1){

	printf("My shell>> "); /*print shell prompt*/
	if (read_command(line) == 0 )
	{
		printf("\n");
	       	break;
		exit(0); /*sys call*/
	} /* CRTL+D pressed*/
	if (strcmp(line, "exit") == 0){
	       	break; /*exit*/
	       exit(0);/*sys call*/
	}
		argc = build_args (line,argv); /* build program argument*/


	set_program_path (path,bin,argv[0]);/*  set program full path*/
	if(search_for_pipe(argv,input,output)){/*parse pipe if found*/
		printf("pipe found\n");
		p=true;
	}
	else{
		printf("pipe not found\n");}
	pipe(fd);/*call pipe sys call*/		 
	int pid= fork(); /*fork child*/
	 if(pid==0){ /*Child*/
	
		 if(echo_(argv,vars)){/*search for echo if found print right value*/
			printf("success\n");
		 
		 }
		 else { printf("echo not found\n");}
		if(strcmp(argv[0],"cd")==0){/*search for cd*/
		printf("Current working dir : %s\n",getcwd(path,sizeof(path)));/*print current dir*/
			if(chdir(argv[1])==0){/*if change dir was done successfuly*/
			printf("cd succsess\n");
			/*print directory after cd*/
			printf("Directory after cd:%s\n",getcwd(path,sizeof(path)));
			/*	exit(0);*/
			/*	printf("PATH : %s\n",getenv("PATH"));*/
			}
			else{/*if chidir returned -1*/
				printf("cd failed\n");
			/*	exit(0);*/
			}
		execvp(path,argv);
		}
		if(p){/*if pipe is found*/
		dup2(fd[0],0);/*input of pipe*/
		close(fd[1]);/*close other side of the pipe*/
		execvp(output[0],output);/*execute right side in child*/
		}


		if(search_for_var(argv,vars)){/*search for equal sign and parse to store variable*/
			printf("variable saved succesfully\n");
			printf(vars[0][0]);/*for test purpose*/
			printf(vars[0][1]);
		/*execvp(path,argv);*/		
		}
		
		/*	else { printf("not cd");}*/
		if( output_redirection(argv,out,command) ){/*search for > and parse*/
			printf("out found\n");
		FILE *file =freopen(out,"w",stdout);/*open file for writing the result*/
		execvp(path,argv);/*execute*/
		
		}
		
		if( input_redirection(argv,in,command) ){/*IF < is found ,parse*/
			printf("in found\n");
			FILE *file= freopen(command,"r",stdin);
			execvp(path,argv);
		}
	 execve(path,argv,0);/*  if failed process is not replaced*/
	execvp(path,argv); 
	 	 /* then print error message*/
	/*		 fprintf(stderr, "Child process could not do execve\n");*/
		/* exit(0);*/
 	}

	else {
		
		if(p){/*if pipe is found*/
		
			dup2(fd[1],1);/*output of pipe*/
			close(fd[0]);/*close other side of the pipe*/
			execvp(input[0],input);/*execute first part of pipe in parent*/
		
		}
		wait(NULL); /*parent*/
	}


    }
	return 0;

}


