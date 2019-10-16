#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
/*shared memory ids*/
int shmid,shmid1,shmid2,shmid3;

int main(int c,char *argv[])
{
	printf("%d\n",getpid());
	int process_pid=getpid();
	int noc=atoi(argv[1]);
	int height=atoi(argv[2]);
	int size=1;
	int mid=noc;
	for(int i=1;i<=height;i++)
	{
		size+=mid;
		mid=(mid*(mid+1))/2;
	}
	printf("%d\n",size);
	shmid=shmget(IPC_PRIVATE,size*sizeof(int),0777|IPC_CREAT);
	
	/*shared memory for storing number of children*/
	int *arr=(int *)shmat(shmid,NULL,0);

	/*shared memory for storing pids*/
	shmid1=shmget(IPC_PRIVATE,size*sizeof(int),0777|IPC_CREAT);
	int *a=(int *)shmat(shmid1,NULL,0);
	
	/*shared memory for storing indices of child*/
	shmid2=shmget(IPC_PRIVATE,sizeof(int),0777|IPC_CREAT);
	int *ind=(int *)shmat(shmid2,NULL,0);

	/*shared memory for storing indices of parent*/
	shmid3=shmget(IPC_PRIVATE,sizeof(int),0777|IPC_CREAT);
	int *loc=(int *)shmat(shmid3,NULL,0);

	

	a[0]=getpid();
	
	arr[0]=noc;
	printf("sizeofarr=%d\n",arr[0] );
	int index=1;
	int sum=0;

	/*code for storing number of children of each node*/
	for(int i=1;i<=noc;i++){
		arr[index++]=i;
		sum+=i;
	}
	int k;
	for(int i=2;i<height;i++)
	{
		k=sum;
		sum=0;
		for(int j=1;j<=k;j++)
		{
			arr[index++]=j;
			
			sum+=j;
		}
	}
	for(int i=1;i<=sum;i++)
		arr[index++]=0;
	for(int i=0;i<size;i++)
		printf("%d ",arr[i]);
	printf("\n");

	index=0;
	int pid;
	*ind=0;
	*loc=0;
	while(arr[index]>0)
	{
		//printf("%d\n",arr[index]);
		pid=fork();
		if(pid==0)
		{
			kill(getpid(),SIGSTOP);
			a=shmat(shmid1,NULL,0);
			arr=shmat(shmid,NULL,0);
			ind=shmat(shmid2,NULL,0);
			loc=shmat(shmid3,NULL,0);
			
			index=(*loc);	
			if(index-1>0)
				kill(a[index-1],SIGSTOP);

			
		}
		else
		{
			int status;
			arr[index]--;
			(*ind)=(*ind)+1;
			a[(*ind)]=pid;
			
			waitpid(pid,&status,WUNTRACED);
		}
	}

	*loc=index+1;
	if(index+1<size)
		kill(a[index+1],SIGCONT);
	int status;
	/*while(arr[index]!=0){
		index++;
		if(index>=size)
			break;		
		//printf("Process id:%d return status:%d\n",getpid(),status/256);
	}*/
	while(wait(NULL)!=-1){}
	printf("process id:%d parent id:%d\n",getpid(),getppid());
	
	if(index-1>=0)
		kill(a[index-1],SIGCONT);
	shmdt(arr);
	shmdt(a);
	shmdt(loc);
	shmdt(ind);
	if(process_pid == getpid()){
		shmctl(shmid,IPC_RMID,0) ;
		shmctl(shmid1,IPC_RMID,0) ;
		shmctl(shmid2,IPC_RMID,0) ;
		shmctl(shmid3,IPC_RMID,0) ;
	}
	exit(3);

}