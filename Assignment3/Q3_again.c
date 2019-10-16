#include<stdio.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<unistd.h>




int *flag;
int shmid;
char path[512];
//int i=0,j=0;
int main(int c,char *argv[])
{

	int n=atoi(argv[1]);
	int main_pid=getpid();
	int mat[n][n];
	for(int i=0;i<n;i++)
	{
		for(int j=0;j<n;j++)
		{
			scanf("%d",&mat[i][j]);
		}
	}
	shmid=shmget(IPC_PRIVATE,sizeof(int),0777|IPC_CREAT);
	flag=(int *)shmat(shmid,NULL,0);


	(*flag)=0;
	int i=0,j=0;
	sprintf(path,"(0,0)->");
	int k=0;
	while(1)
	{
		//printf("i=%d  j=%d pid:%d\n",i,j,getpid());
		flag=(int *)shmat(shmid,NULL,0);
		int pid,pid1;
		k=0;
		
		if((pid=fork())==0)
		{
			i++;
			k=1;
			flag=(int *)shmat(shmid,NULL,0);
			//if((*flag)==0)
			//{
				if(i>=n || j>=n || mat[i][j]==1){
					//printf("Returning i=%d j=%d\n",i,j);
					exit(1);
				}
				else if(mat[i][j]==2)
				{
					//printf("inside\n");
					strcat(path,"(");
					char temp[20];
					sprintf(temp,"%d",i);
					strcat(path,temp);
					sprintf(temp,"%d",j);
					strcat(path,temp);
					strcat(path,")");
					printf("%s\n",path);
					(*flag)=1;
					exit(0);
				}
				else
				{
					strcat(path,"(");
					char temp1[20];
					char temp2[20];
					sprintf(temp1,"%d",i);
					sprintf(temp2,"%d",j);
					strcat(path,temp1);
					
					strcat(path,",");
					
					strcat(path,temp2);
					strcat(path,")->");
					//printf("%s\n",path);
				}
			//}
			//else
			//	exit(2);
		}
		else if((pid1=fork())==0 && k==0)
		{
			j++;
			flag=(int *)shmat(shmid,NULL,0);
			//if((*flag)==0)
			//{
				if(j>=n || i>=n || mat[i][j]==1){
					//printf("returning i=%d j=%d\n",i,j);
					exit(1);
				}
				else if(mat[i][j]==2)
				{
					//printf("inside\n");
					strcat(path,"(");
					char temp[20];
					sprintf(temp,"%d\0",i);
					strcat(path,temp);
					strcat(path,",");
					sprintf(temp,"%d\0",j);
					strcat(path,temp);
					strcat(path,")");
					printf("%s\n",path);
					(*flag)=1;
					exit(0);
				}
				else
				{
					strcat(path,"(");
					char temp1[20];
					char temp2[20];
					sprintf(temp1,"%d",i);
					sprintf(temp2,"%d",j);
					strcat(path,temp1);
					
					strcat(path,",");
					
					strcat(path,temp2);
					strcat(path,")->");
					//printf("%s\n",path);
				}
			//}
			//else
			//	exit(2);
		}
		else
		{
			//printf("pid=%d pid1=%d parent:%d\n",pid,pid1,getpid());
			//printf("%d\n",wait(NULL));
			//printf("%d\n",wait(NULL));
			while(wait(NULL)!=-1){}
			break;
		}

	}
	
	if((*flag)==0 && getpid()==main_pid)
		printf("Spiderman could not find MJ\n");
	return 0;

}