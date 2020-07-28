#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<stdlib.h>
sem_t rqueue[100],wqueue[100];
sem_t mutex,wr,bound,check,cont;
int n,m;
char *seq;
typedef struct{
	int index;
	int num;
}number;
int readCount=0;
int data=0;
int flag=0;
void *reader(void *par)
{
	number *ptr=(number *)par;
	int index=ptr->index;
	int num=ptr->num;
	if(num!=0)
		sem_wait(&rqueue[num]);
	sem_wait(&mutex);
	readCount++;
	if(readCount==1)
		sem_wait(&wr);
	if(num<n-1)
		sem_post(&rqueue[num+1]);
	sem_post(&mutex);
	int y=index-1;
	int cnt=0;

	//sem_wait(&check);

	while(y>=0)
	{
		if(seq[y]=='w')
			break;
		else{
			y--;
			cnt++;
		}
	}

	if(cnt>2 && flag==1){
		printf("Control to write given\n");
		sem_post(&wr);
		sem_wait(&cont);
	}
	//else
	//	sem_post(&check);

	sem_wait(&bound);
	//critical section
	//flag=1;
	printf("Data read by reader%d is %d\n",num,data);

	sem_post(&bound);


	sem_wait(&mutex);
	readCount--;
	if(readCount==0){
		//flag=0;
		sem_post(&wr);
	}
	
	sem_post(&mutex);
}


void *writer(void *par)
{
	number *ptr=(number *)par;
	int index=ptr->index;
	int num=ptr->num;
	flag=1;
	sem_wait(&wr);
	data++;
	printf("Data written by writer%d is %d\n",num,data);
	//sleep(5);
	
	flag=0;
	if(readCount!=0)
		sem_post(&cont);
	sem_post(&wr);

}

int main()
{
	printf("Enter number of readers:");
	scanf("%d",&n);

	printf("Enter number of writers:");
	scanf("%d",&m);

	char c;
	scanf("%c",&c);

	seq=(char *)malloc(sizeof(char)*(n+m));
	printf("Enter the sequence of r/w");
	for(int i=0;i<n+m;i++)
		scanf("%c",&seq[i]);

	sem_init(&mutex,0,1);
	sem_init(&bound,0,5);
	sem_init(&wr,0,1);
	sem_init(&check,0,1);
	sem_init(&cont,0,0);
	number mat[m+n];

	int r=0,w=0;
	pthread_t rtid[n],wtid[m];
	for(int i=0;i<n+m;i++)
	{
		if(seq[i]=='r')
		{
			sem_init(&rqueue[r],0,0);
			mat[i].index=i;
			mat[i].num=r;
			r++;
		}
		else
		{
			sem_init(&wqueue[w],0,0);
			mat[i].index=i;
			mat[i].num=w;
			w++;
		}
	}
	r=0;w=0;
	for(int i=0;i<n+m;i++)
	{
		if(seq[i]=='r')
		{
			pthread_create(&rtid[r],NULL,reader,&mat[i]);
			//sleep(2);
			r++;
		}
		else
		{
			pthread_create(&wtid[w],NULL,writer,&mat[i]);
			//sleep(2);
			w++;
		}
	}
	for(int i=0;i<n;i++)
	{
		pthread_join(rtid[i],NULL);
	}
	for(int i=0;i<m;i++)
	{
		pthread_join(wtid[i],NULL);
	}
}