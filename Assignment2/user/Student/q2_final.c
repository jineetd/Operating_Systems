#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<fcntl.h> 
#include<string.h>

int shmid,shmid1;
int *flag;
char *final_path;
char filename[300];

int search(char *path)
{
	//printf("Search called in %s\n",path);
	chdir(path);
	//printf("Path passed:%s",path);
	char curr_path[256];
	getcwd(curr_path,256);
	//printf("current path=%s\n",curr_path);
	//printf("File to be found=%s\n",filename);
	flag=(int *)shmat(shmid,NULL,0);
	final_path=(char *)shmat(shmid1,NULL,0);
	int k=0;
	int pid,pid1;
	pid=fork();
	if(pid==0)
	{
		flag=(int *)shmat(shmid,NULL,0);
		final_path=(char *)shmat(shmid1,NULL,0);
		int fd=creat("file.txt",0777);
		dup2(fd,1);
		char *arg[]={"ls","-F",0};
		k=1;
		execvp("ls",arg);
	}
	waitpid(pid,NULL,0);
	if((pid1=fork())==0)
	{
		flag=(int *)shmat(shmid,NULL,0);
		final_path=(char *)shmat(shmid1,NULL,0);
		int status;
		//waitpid(pid,&status,0);
		int fd=open("file.txt",O_RDONLY);
		dup2(fd,0);
		//printf("file descriptor:%d\n",fd);
		char *arg[]={"grep",filename,0};

		int op=creat("result.txt",0777 | O_WRONLY);
		dup2(op,1);
		execvp("grep",arg);
	}
	else
	{
		while(wait(NULL)!=-1){}	
		char c[300];
		FILE *ptr;
		ptr=fopen("result.txt","r");
		while(fgets(c,sizeof(c),ptr)!=NULL)
		{
			int len=strlen(c);
			if(c[len-1]!='/')
			{
				if((*flag)==0)
				{
					//printf("File found.....\n");
					strcpy(final_path,path);
					(*flag)=1;
					break;
				}
			}
		}
		//if(*flag==0)
			//printf("File not found....");
		fclose(ptr);
	}
	
	/*delete intermdediate files*/
	int g;
	g=fork();
	if(g==0)
	{
		char *arg[]={"rm","file.txt","result.txt",0};
		int temp=execv("/bin/rm",arg);
		//printf("rm working in search:%d\n",temp);
	}
	wait(NULL);
	return 0;
}

int main(int c,char *argv[])
{
	int parent_pid=getpid();
	strcpy(filename,argv[1]);

	int id=atoi(argv[2]);
	int marks=atoi(argv[3]);
	
	/*shared memory for flag*/
	shmid=shmget(IPC_PRIVATE,sizeof(int),0777|IPC_CREAT);
	flag=(int *)shmat(shmid,NULL,0);
	(*flag)=0;

	shmid1=shmget(IPC_PRIVATE,256*sizeof(int),0777|IPC_CREAT);
	final_path=(char *)shmat(shmid1,NULL,0);

	char path[2000]="/home/jineet/Operating_Systems/Assignment2/user/Evaluator/";
	//search(path);
	chdir(path);
	getcwd(path,2000);
	strcat(path,"/");

	int f=0;

	while((*flag)==0)
	{
		f=0;
		search(path);

		//printf("Starting process****** pid:%d ppid:%d\n",getpid(),getppid());

		int pid;
		pid=fork();
		if(pid==0)
		{
			chdir(path);
			//printf("ls_path=%s\n",path);
			int inp=creat("file1.txt",0777);
			dup2(inp,1);
			close(inp);
			char *arg[]={"ls","-F",0};
			execv("/bin/ls",arg);
		}
		waitpid(pid,NULL,0);
		if(fork()==0)
		{
			chdir(path);
			int inp=open("file1.txt",O_RDONLY);
			int op=creat("result1.txt",0777);
			dup2(inp,0);
			dup2(op,1);
			close(inp);
			close(op);
			char *arg[]={"grep","/",0};
			execv("/bin/grep",arg);
		}
		wait(NULL);
		FILE *ptr;
		ptr=fopen("result1.txt","r");
		char c[1000];
		while(fgets(c,1000,ptr)!=NULL)
		{
			if((*flag)==0)
			{
				int pid2;
				pid2=fork();
				if(pid2==0)
				{
					flag=(int *)shmat(shmid,NULL,0);
					final_path=(char *)shmat(shmid1,NULL,0);
					char c1[1000];
					strncpy(c1,c,strlen(c)-1);
					if(c1==NULL)
						break;
					//printf("c1=%s\n",c1);
					strcat(path,c1);
					int res=chdir(path);
					if(res==-1)
						exit(2);
					//search(path);
					//if(ans==0)
					//	exit(2);
					//printf("Path=%s\n",path);
					char s[500];
					getcwd(s,500);
					//printf("Path in child:%s\n",s);
					f=1;
					break;
				}
				waitpid(pid2,NULL,0);
			}
			else
				break;
		}
		if(f)
			continue;
		while(wait(NULL)!=-1){}

		int g;
		g=fork();
		if(g==0)
		{
			char *arg[]={"rm","file1.txt","result1.txt",0};
			int temp=execv("/bin/rm",arg);
			//printf("rm working in search:%d\n",temp);
		}
		wait(NULL);
		break;

	}
	if(getpid() != parent_pid)
	{	
		shmdt(flag) ; 
		shmdt(final_path) ;
		exit(0);
	}
	//printf("Final path:%s , flag %d \n",final_path,*flag);

	/*code for changing the marks*/
	chdir(final_path);
	int h;
	h=fork();
	if(h==0)
	{
		char *arg[]={"grep","-n",argv[2],filename,0};
		int fd=creat("inter.txt",0777);
		dup2(fd,1);
		close(fd);
		execv("/bin/grep",arg);
	}
	wait(NULL);
	FILE *ptr;
	ptr=fopen("inter.txt","r");
	char num[100];
	//char c;
	int ind=0;
	c=fgetc(ptr);
	while(c!=':')
	{
		num[ind++]=c;
		c=fgetc(ptr);
	}
	num[ind]='\0';
	fclose(ptr);
	int line_number=atoi(num);

	FILE *f1,*f2;
	f1=fopen(filename,"r");
	f2=fopen("res.txt","w+");
	char temp[300];
	for(int i=1;i<=line_number-1;i++)
	{
		fgets(temp,300,f1);
		fprintf(f2,"%s",temp);
	}
	fprintf(f2,"%s %s\n",argv[2],argv[3]);
	fgets(temp,300,f1);
	while(fgets(temp,300,f1)!=NULL)
	{
		fprintf(f2, "%s", temp);
	}
	fclose(f1);
	fclose(f2);

	/*deleting the original file*/
	h=fork();
	if(h==0)
	{
		char *arg[]={"rm","inter.txt",filename,0};
		execv("/bin/rm",arg);
	}
	wait(NULL);
	/*Renaming res.txt*/
	rename("res.txt",filename);

	//printf("Marks changed succesfully\n");





	shmdt(flag) ; 
	shmdt(final_path) ;
	shmctl(shmid,IPC_RMID,0);
	shmctl(shmid1,IPC_RMID,0);
	return 0;
}