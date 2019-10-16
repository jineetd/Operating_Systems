#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<fcntl.h> 
#include<string.h>
#include<stdbool.h>
int shmid,shmid1;
int *flag;
char *final_path;
char *filename;
int search(char *path)
{
	
	char curr_path[1000];
	getcwd(curr_path,1000);
	//printf(" path (in search): %s\n",path);
	chdir(path);
	printf("\n \n current path=%s \n \n",curr_path);
	flag=shmat(shmid,NULL,0);
	final_path=shmat(shmid1,NULL,0);
	int pid,pid1;
	pid=fork();
	if(pid==0)
	{
		int fd=creat("file.txt",0777);
		dup2(fd,1);
		close(fd);
		char *arg[]={"ls","-F",NULL};
		execv("/bin/ls",arg);
		exit(10) ;
	}
	int status ; 
	int ret = waitpid(pid,&status,0) ;
	if((pid1=fork())==0)
	{
		int fd=open("file.txt",O_RDONLY);
		dup2(fd,0);
		int op=creat("result.txt",0777);		
		dup2(op,1) ;
		close(op) ;
		close(fd) ; 
		char *arg[]={"grep",filename,NULL};
		int temp = execvp("grep",arg);
	}
	wait(NULL) ;	
	char c[300];
	FILE *ptr;
	ptr=fopen("result.txt","r");
	while(1)
	{
		c[0] = '\0' ;
		int isEnd=fscanf(ptr,"%[^\n]\n",c);
		printf("Inside search function : %s , %d \n",c,isEnd);
		if(c[strlen(c)-1]!='/')
		{
			if((*flag)!=1)
			{
				printf("File found....\n");
				*flag=1;
				final_path=path;
				fclose(ptr) ;
				exit(1);
			}
		}
		else if(isEnd==EOF)
			break;
	}
	fclose(ptr) ;

	int g = fork() ; 
	if(g == 0)
	{
		char *arg[] = {"rm","file.txt","result.txt",NULL} ;
		int temp = execvp("rm",arg) ;
		printf("rm not working in search %d\n",temp);
	}
	return 0;
}
int main(int k,char *argv[])
{
	filename=argv[1];
	bool f = 0 ;
	int id=atoi(argv[2]);
	int marks=atoi(argv[3]);
	
	shmid=shmget(IPC_PRIVATE,sizeof(int),0666|IPC_CREAT);
	flag = (int *)shmat(shmid,NULL,0);
	shmid1=shmget(IPC_PRIVATE,256*sizeof(char),0666|IPC_CREAT);
	final_path=(char *)shmat(shmid1,NULL,0);
	int parent_pid = getpid() ; 
	char path[1000]="../Evaluator/";
	chdir(path) ;
	getcwd(path,1000);
	strcat(path,"/");
	char c[10][50];
	printf("%s\n",path );
	int pid2 ;
	//printf("%d , %d\n",shmid,shmid1);
	*flag = 0 ;
	FILE *ptr;
	int p_id , pid , pid1 , g , h ;
	while((*flag)==0)
	{	
		f = 0 ; 
		p_id = fork() ;
		if(p_id == 0)
		{
			flag=(int*)shmat(shmid,NULL,0);
			final_path=(char*)shmat(shmid1,NULL,0);
			printf("Inside main method : %s\n",path);
			search(path) ;
			shmdt(flag) ; 
			shmdt(final_path) ;
			exit(0) ;
		}
		else
		{
			waitpid(p_id,NULL,0);
		}
		printf("pid=%d ppid:%d now starting main job of that process\n .",getpid(),getppid());
		if((*flag)==0)
		{
			pid=fork();
			if(pid==0)
			{
				int fd=creat("file1.txt",0777);
				dup2(fd,1);
				close(fd);
				char *arg[]={"ls","-F",NULL};
				execv("/bin/ls",arg);
				exit(10) ;
			}
			int status ; 
			int ret = waitpid(pid,&status,0) ;
			if((pid1=fork())==0)
			{
				int fd=open("file1.txt",O_RDONLY);
				dup2(fd,0);
				int op=creat("result1.txt",0777);		
				dup2(op,1) ;
				close(op) ;
				close(fd) ; 
				char *arg[]={"grep","/",NULL};
				int temp = execvp("grep",arg);
			}
			wait(NULL) ;
			
			for(int i = 0 ; i < 10 ; i++ )
				c[i][0] = '\0' ;
			ptr=fopen("result1.txt","r");
			h = 0 ;
			for(h = 0 ; h<10 ; h++ )
			{	
				char temp[50] ;
				int isEnd= fscanf(ptr,"%[^\n]\n",temp);
				if(isEnd==EOF)
				{
					break;
				}
				else
				{
					int i = 0 ;
					while((c[h][i] = temp[i]) != '\0')
						i++;
					printf("content %d th in result1 %s\n",h,temp);
				}
			}
			fclose(ptr);

			g = fork() ; 
			if(g == 0)
			{
				//printf("%s\n",path);
				char *arg[] = {"rm","file1.txt","result1.txt",NULL} ;
				int temp = execvp("rm",arg) ;
				printf("rm not working in main %d\n",temp);
			}
			waitpid(g,NULL,0);
			if(h == 0)
				break ;
			while(h--)
			{
				if((*flag)==0)
				{
					pid2=fork();
					if(pid2==0){
						flag=(int*)shmat(shmid,NULL,0);
						final_path=(char*)shmat(shmid1,NULL,0);
						strcat(path,c[h]);
						chdir(path);
						printf("%s\n",path);
						f = 1 ; 
						break;
					}
					waitpid(pid2,NULL,0) ;
				}
				else
					break ;
			}
		}

		if(f)
			continue ;
		while(wait(NULL)!=-1) ;	
		break;		
	}	
	if(getpid() != parent_pid)
	{	
		shmdt(flag) ; 
		shmdt(final_path) ;
		exit(0);
	}
	printf("Final path:%s , flag %d \n",final_path,*flag);
	shmdt(flag) ; 
	shmdt(final_path) ;
	shmctl(shmid,IPC_RMID,0);
	shmctl(shmid1,IPC_RMID,0);
	return 0;
}