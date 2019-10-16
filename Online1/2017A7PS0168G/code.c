#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
char *inputfile;
int shmid,shmid1,shmid2;
int *pid;
int *val;
int *status;
int main(int were,char *argv[])
{
	inputfile=argv[1];
	int main_pid=getpid();
	FILE *ptr;
	ptr=fopen(inputfile,"r");
	char num[100];
	int k=0;
	char c=fgetc(ptr);
	while(c!='\t')
	{
		num[k++]=c;
		c=fgetc(ptr);
	}
	num[k]='\0';
	fclose(ptr);
	printf("Number of orders are: %d\n",atoi(num));
	int total_orders=atoi(num);
	int orders[total_orders];
	/*storing the orders*/
	ptr=fopen(inputfile,"r");
	fgets(num,100,ptr);
	char *tok;
	tok=strtok(num,"	");
	tok=strtok(0,"	");
	k=0;
	while(tok!=NULL)
	{
		orders[k++]=atoi(tok);
		tok=strtok(0,"	");
	}
	fclose(ptr);
	

	shmid=shmget(IPC_PRIVATE,4*sizeof(int),IPC_CREAT|0777);
	pid=(int *)shmat(shmid,NULL,0);
	pid[0]=getpid();
	int index=0;
	shmid1=shmget(IPC_PRIVATE,total_orders*sizeof(int),IPC_CREAT|0777);
	val=(int *)shmat(shmid1,NULL,0);

	shmid2=shmget(IPC_PRIVATE,total_orders*sizeof(int),IPC_CREAT|0777);
	status=(int *)shmat(shmid2,NULL,0);
	/*creating the 3 processes A,B,C */
	for(int i=1;i<=3;i++)
	{
		if(fork()==0)
		{
			kill(getpid(),SIGSTOP);
			pid=(int *)shmat(shmid,NULL,0);
			val=(int *)shmat(shmid1,NULL,0);
			status=(int *)shmat(shmid2,NULL,0);
			pid[i]=getpid();
			if(i==1)
			{
				//printf("A pid:%d\n",getpid());
				for(int j=0;j<total_orders;j++)
				{
					//printf("pid1:%d\n",pid[1]);
					int N=orders[j]%4+1;
					int ppid=pid[1];
					int ith=pid[1]%N+1;
					val[j]=orders[j];
					
					for(int k=1;k<=N;k++)
					{
						if(fork()==0)
						{
							pid=(int *)shmat(shmid,NULL,0);
							val=(int *)shmat(shmid1,NULL,0);
							if(k==ith)
							{
								val[j]+=(pid[1]%5);
								exit(1);
							}
							else
							{
								printf("pid:%d	ppid:%d \n",getpid(),getppid());
								exit(1);
							}
						}
					}
					while(wait(NULL)!=-1){}
					status[j]=2;
					//printf("status[j]=%d\n",status[j]);
				}
				break;

			}
			else if(i==2)
			{
				pid=(int *)shmat(shmid,NULL,0);
				val=(int *)shmat(shmid1,NULL,0);
				status=(int *)shmat(shmid2,NULL,0);
				for(int j=0;j<total_orders;j++)
				{
					//printf("total_orders:%d\n",total_orders);
					int parent=getpid();
					while(status[j]!=2){

					}
					int p;
					int temp=val[j];
					for(int k=0;k<3;k++)
					{
						if((p=fork())==0)
						{
							pid=(int *)shmat(shmid,NULL,0);
							val=(int *)shmat(shmid1,NULL,0);

							if(k==0){
								val[j]*=2;
								//exit(0);
							}
							else if(k==1){
								val[j]*=3;
								//exit(0);
							}
							else{
								val[j]*=4;
								//exit(0);
							}
							//printf("pid:%d	ppid:%d temp:%d\n",getpid(),getppid(),temp);
						}
						else
						{
							waitpid(p,NULL,0);
							//printf("Breaking after wait\n");
							break;
						}
					}
					//printf("Out\n");
					while(wait(NULL)!=-1){}
					if(getpid()!=parent)
						exit(1);
					//printf("Out 111\n");
					//val[j]=temp;
					status[j]=3;
				}
				//printf("out here\n");
				break;
			}
			else if(i==3)
			{
				pid=(int *)shmat(shmid,NULL,0);
				val=(int *)shmat(shmid1,NULL,0);
				status=(int *)shmat(shmid2,NULL,0);
				/*creating file for output concatenation*/

				FILE *f;
				f=fopen("Finaloutput","w");
				fclose(f);				
				f=fopen("Finaloutput","a+");
				
				for(int j=0;j<total_orders;j++)
				{
					while(status[j]!=3){

					}
					fprintf(f,"\n************%d*************\n",j+1);
					int J=val[j]%5+1;
					val[j]=val[j]+J;
					FILE *fptr=fopen("command.in","r");
					char command[500];
					fgets(command,500,fptr);
					char *com;
					//strncpy(com,command,strlen(command)-1);
					com=strtok(command,"\n");
					//printf("%s\n",com);
					char *words[20];
					char *tok;
					int x=0;
					tok=strtok(com,"\t");
					while(tok!=NULL)
					{
						words[x++]=tok;
						tok=strtok(0,"\t");
					}
					words[x]=NULL;//1st input
					int g;
					g=fork();
					if(g==0)
					{
						int fd=creat("inter.txt",0777);
						dup2(fd,1);
						close(fd);
						execvp(words[0],words);
						printf("Error....\n");
					}
					wait(NULL);
					//fclose(fptr);
					/*char c;
					FILE *gd=fopen("inter.txt","r");
					c=fgetc(gd);
					while(c!=EOF)
					{
						fputc(c,f);
						c=fgetc(gd);
					}
					fclose(gd);*/
					for(int k=2;k<=J;k++)
					{
						char command[500];
						fgets(command,500,fptr);
						char *com;
						//strncpy(com,command,strlen(command)-1);
						com=strtok(command,"\n");
						//printf("%s\n",com);
						char *words[20];
						char *tok;
						int x=0;
						tok=strtok(com,"\t");
						while(tok!=NULL)
						{
							words[x++]=tok;
							tok=strtok(0,"\t");
						}
						words[x]=NULL;
						int g;
						g=fork();
						if(g==0)
						{
							if(k==2 || k==4)
							{
								int inp=open("inter.txt",O_RDONLY);
								int op=creat("inter1.txt",0777);
								dup2(inp,0);
								dup2(op,1);
								close(inp);
								close(op);
								execvp(words[0],words);
								printf("Error...\n");
							}
							else
							{
								int inp=open("inter1.txt",O_RDONLY);
								int op=creat("inter.txt",0777);
								dup2(inp,0);
								dup2(op,1);
								close(inp);
								close(op);
								execvp(words[0],words);
								printf("Error...\n");
							}
						}
						wait(NULL);
					}
					while(wait(NULL)!=-1){}
					fclose(fptr);
					if(J%2==1)
					{
						char c;
						FILE *gd=fopen("inter.txt","r");
						c=fgetc(gd);
						while(c!=EOF)
						{
							fputc(c,f);
							c=fgetc(gd);
						}
						fclose(gd);
					}
					else
					{
						char c;
						FILE *gd=fopen("inter1.txt","r");
						c=fgetc(gd);
						while(c!=EOF)
						{
							fputc(c,f);
							c=fgetc(gd);
						}
						fclose(gd);	
					}


				}
				fclose(f);
				int g;
				g=fork();
				if(g==0)
				{
					char *arg[]={"rm","inter.txt","inter1.txt",0};
					execvp("rm",arg);
				}
				wait(NULL);
				break;
			}


		}
		else
			waitpid(-1,NULL,WUNTRACED);
	}
	kill(-1,SIGCONT);
	while(wait(NULL)!=-1){}
	if(getpid()==main_pid){
	printf("Initial values:\t");
	for(int i=0;i<total_orders;i++)
	{
		printf("%d\t",orders[i]);
	}
	printf("\nFinal values:\t");
	for(int i=0;i<total_orders;i++)
	{
		printf("%d\t",val[i]);
	}
	printf("\n");
}
	return 0;

}