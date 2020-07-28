//This code is a basic implementation of reader writer problem
#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>

sem_t mutex,wr,bound;
int data=0,rcount=0;
int n,m;
char *seq;
void *reader(void *arg)
{
	int val=(int)arg;
	sem_wait(&mutex);
	rcount++;
	if(rcount==1)
		sem_wait(&wr);

	
	sem_post(&mutex);

	sem_wait(&bound);
	printf("\nEntering critical section...\n");
	//critical section
	printf("The data read by reader%d is %d\n",val,data);
	sleep(2);
	//read section
	printf("Exiting critical section...\n");
	sem_post(&bound);
	sem_wait(&mutex);
	rcount--;
	if(rcount==0)
		sem_post(&wr);
	
	sem_post(&mutex);

}

void *writer(void *arg)
{
	int val=(int)arg;
	sem_wait(&wr);
	printf("\nEntering critical section...\n");
	data++;
	printf("The data written by writter%d is %d\n",val,data);
	sleep(5);
	printf("Exiting critical section...\n");
	sem_post(&wr);
}

int main()
{
	
	printf("Enter number of readers:");
	scanf("%d",&n);
	printf("Enter number of writers:");
	scanf("%d",&m);
	pthread_t rtid[n],wtid[m];

	sem_init(&mutex,0,1);
	sem_init(&wr,0,1);
	sem_init(&bound,0,5);

	seq=(char *)malloc(sizeof(char)*(n+m));
	printf("Enter the sequence of reader/writers:");
	for(int i=0;i<n+m;i++)
		scanf("%c",&seq[i]);

	int r=0,w=0;
	for(int i=0;i<n+m;i++)
	{
		if(seq[i]=='r')
		{
			pthread_create(&rtid[r],NULL,reader,(void *)r);
			r++;
		}
		else
		{
			pthread_create(&wtid[w],NULL,writer,(void *)w);
			w++;
		}
	}
	for(int i=0;i<r;i++)
		pthread_join(rtid[i],NULL);
	for(int i=0;i<w;i++)
	{
		pthread_join(wtid[i],NULL);
	}
	
	return 0;

}