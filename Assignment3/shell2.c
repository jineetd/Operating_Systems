#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h> 
#include<time.h>
#include<sys/ipc.h>
#include<sys/shm.h>
int log_flag=0;
FILE *commandlog,*outputlog;
int *status_for_exec;
int shmid;
int *res;
/*function for the command line interpreter*/
void interpreter(char input[200],char *envp[])
{

	time_t T =time(NULL);
	struct tm *obj;
	obj=localtime(&T);
	int no_of_pipes=0;
	for(int i=0;i<strlen(input);i++)
	{
		if(input[i]=='|')
			no_of_pipes++;
	}
	
	/*if(no_of_pipes==0)
	{
		return;
	}*/
	int fd[no_of_pipes][2];
	



	char *words[no_of_pipes+1];
	char *tok;
	tok=strtok(input,"|");
	int k=0;
	while(tok!=NULL)
	{
		words[k++]=tok;
		//printf("%s\n",tok);
		tok=strtok(0,"|");
	}
	


	char *write,*read;
	int x=0,y=0;
	
	write=words[0];
	read=words[1];
	
	char *w_words[20],*r_words[20];
	//char *tok;
	tok=strtok(write," ");
	while(tok!=NULL)
	{
		w_words[x++]=tok;
		//printf("%s\n",tok);
		tok=strtok(0," ");
	}
	w_words[x]=NULL;

	tok=strtok(read," ");
	while(tok!=NULL)
	{
		r_words[y++]=tok;
		//printf("%s\n",tok);
		tok=strtok(0," ");
	}
	r_words[y]=NULL;

	
	if(no_of_pipes==0)
	{
		int pid;
		pid=fork();
		if(pid==0)
		{
			res=(int *)shmat(shmid,NULL,0);
			(*res)=execvp(w_words[0],w_words);
			if(*res==-1)
			{
				printf("Command %s not found...\n",w_words[0]);
				exit(5);
			}
		}
		else
		{
			waitpid(-1,NULL,0);
			char result[20];
			if((*res)!=-1)
				strcpy(result,"Success");
			else
				strcpy(result,"Fail");
			if(log_flag==1)
			{
				fprintf(commandlog, "%s %d:%d:%d %d:%d:%d %s\n",w_words[0],obj->tm_mday,obj->tm_mon,obj->tm_year+1900,obj->tm_hour,obj->tm_min,obj->tm_sec,result);
			}

		}
	}
	if((*res)==-1)
		return;
	int pid;
	int status;
	pid=fork();
	if(pid==0)
	{
		res=(int *)shmat(shmid,NULL,0);
		fd[0][0]=creat("results.txt",0777);
		fd[0][1]=creat("results1.txt",0777);
		dup2(fd[0][0],1);
		
		(*res)=execvp(w_words[0],w_words);
	}
	else{ 
		waitpid(-1,NULL,0);
		if(log_flag==1)
		{
			FILE *f=fopen("results.txt","r");
			char c;
			while((c=fgetc(f))!=EOF){
				fputc(c,outputlog);
			}
			fclose(f);
			char result[20];
			if(*res==-1)
				strcpy(result,"Fail");
			else
				strcpy(result,"Success");
			fprintf(commandlog, "%s %d:%d:%d %d:%d:%d %s\n",w_words[0],obj->tm_mday,obj->tm_mon,obj->tm_year+1900,obj->tm_hour,obj->tm_min,obj->tm_sec,result);
		}
		

	}

	pid=fork();
	if(pid==0)
	{
		res=(int *)shmat(shmid,NULL,0);
		fd[0][1]=open("results.txt",O_RDONLY);
		dup2(fd[0][1],0);
		
		if(no_of_pipes!=1){
			int fd=open("results1.txt",O_WRONLY);
			dup2(fd,1);
		}

	
		(*res)=execvp(r_words[0],r_words);
	}
	else{
		waitpid(-1,NULL,0);
		if(log_flag==1)
		{
			FILE *f=fopen("results1.txt","r");
			char c;
			while((c=fgetc(f))!=EOF){
				fputc(c,outputlog);
			}
			fclose(f);
			char result[20];
			if(*res==-1)
				strcpy(result,"Fail");
			else
				strcpy(result,"Success");
			fprintf(commandlog, "%s %d:%d:%d %d:%d:%d %s\n",r_words[0],obj->tm_mday,obj->tm_mon,obj->tm_year+1900,obj->tm_hour,obj->tm_min,obj->tm_sec,result);
		}
		
	}

	for(int i=2;i<=no_of_pipes;i++)
	{
		char *write;
		int x=0;
		
		write=words[i];
		
		
		char *w_words[20];
		char *tok;
		tok=strtok(write," ");
		while(tok!=NULL)
		{
			w_words[x++]=tok;
			//printf("%s\n",tok);
			tok=strtok(0," ");
		}
		w_words[x]=NULL;

		int temp_pid;
		temp_pid=fork();
		if(temp_pid==0)
		{
			res=(int *)shmat(shmid,NULL,0);
			if(i%2==0)
			{
				fd[i-1][0]=open("results1.txt",O_RDONLY);
				fd[i-1][1]=open("results.txt",O_WRONLY);
				dup2(fd[i-1][0],0);
				if(i!=no_of_pipes)
					dup2(fd[i-1][1],1);
			}
			else
			{
				fd[i-1][0]=open("results1.txt",O_WRONLY);
				fd[i-1][1]=open("results.txt",O_RDONLY);
				if(i!=no_of_pipes)
					dup2(fd[i-1][0],1);
				dup2(fd[i-1][1],0);
			}
			(*res)=execvp(w_words[0],w_words);
		}
		else
		{
			waitpid(-1,NULL,0);
			if(log_flag==1)
			{
				FILE *f;
				if(i%2==0)
					f=fopen("results.txt","r");
				else
					f=fopen("results1.txt","r");
				char c;
				while((c=fgetc(f))!=EOF){
					fputc(c,outputlog);
				}
				fclose(f);
				char result[20];
				if(*res==-1)
					strcpy(result,"Fail");
				else
					strcpy(result,"Success");
				fprintf(commandlog, "%s %d:%d:%d %d:%d:%d %s\n",w_words[0],obj->tm_mday,obj->tm_mon,obj->tm_year+1900,obj->tm_hour,obj->tm_min,obj->tm_sec,result);

			}
		}

		
	}
	/*code for removing intermediate files*/

	//printf("Exiting succesfully...\n");
	
}

int main(int c,char *argv[],char *envp[])
{
	shmid=shmget(IPC_PRIVATE,sizeof(int),0777|IPC_CREAT);
	res=(int *)shmat(shmid,NULL,0);
	char input[200];
	gets(input);
	while(strcmp(input,"entry")!=0)
	{
		printf("Command line interpreter not started\n");
		gets(input);
	}
	printf("************************WELCOME TO THE CUSTOM TERMINAL************************\n");
	printf("Version: 1.0.1\n");
	printf("Creator:Jineet Desai\n");
	printf("*******************************************************************************\n");

	while(1)
	{
		printf(">");
		gets(input);

		if(strcmp(input,"exit")==0)
		{
			printf("Exiting the terminal......\n");
			break;
		}
		else if(strcmp(input,"log")==0)
		{
			commandlog=fopen("command.log","a+");
			outputlog=fopen("output.log","a+");
			log_flag=1;
		}
		else if(strcmp(input,"unlog")==0)
		{
			fclose(commandlog);
			fclose(outputlog);
			log_flag=0;
		}
		else if(strcmp(input,"viewcmdlog")==0)
		{
			FILE *ptr=fopen("command.log","r");
			if(ptr==NULL)
			{
				printf("Cannot open the file.....\n");
			}
			else
			{
				char c;
				while((c=fgetc(ptr))!=EOF)
				{
					printf("%c",c);
				}
				printf("\n");
				fclose(ptr);
			}
		}
		else if(strcmp(input,"viewoutlog")==0)
		{
			FILE *ptr=fopen("output.log","r");
			if(ptr==NULL)
			{
				printf("Cannot open the file.....\n");
			}
			else
			{
				char c;
				while((c=fgetc(ptr))!=EOF)
				{
					printf("%c",c);
				}
				printf("\n");
				fclose(ptr);
			}
		}
		else 
		{
			//printf("Other commands still in progress\n");
			int no_of_spaces=0;
			for(int i=0;i<strlen(input);i++)
			{
				if(input[i]==' ')
					no_of_spaces++;
			}
			char words[no_of_spaces+1][100];
			int row=0;
			int col=0;
			for(int i=0;i<strlen(input);i++)
			{
				if(input[i]==' ' || input[i]=='\0')
				{
					words[row++][col]='\0';
					col=0;
				}
				else
					words[row][col++]=input[i];
			}
			if(strcmp(words[0],"changedir")==0)
			{
				int status=chdir(words[1]);
				if(status==0)
				{
					printf("Directory changed successfully....\n");
					char path[200];
					getcwd(path,200);
					printf("Current path is %s\n",path);
				}
				else
				{
					printf("Invalid path to changedir...\n");
				}
			}
			else
			{
				interpreter(input,envp);
			}
		}
	}
	return 0;

}