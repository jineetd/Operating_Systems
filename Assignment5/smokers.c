#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<stdlib.h>

sem_t mutex,ag;
sem_t asem[5],bsem[5],csem[5];
unsigned long int aqueue[5],bqueue[5],cqueue[5];
int aind,bind,cind;
int na,nb,nc;
void *agent(void *param)
{
	while(1)
	{
		sem_wait(&mutex);//lock
		int random=(rand()%3);
		if(random==0)
		{
			printf("Put item2 and item3 on table\n");
			sem_post(&asem[(aind++)%na]);
		}
		else if(random==1)
		{
			printf("Put item1 and item3 on table\n");
			printf("%d\n",bind);
			sem_post(&bsem[(bind++)%nb]);
		}
		else
		{
			printf("Put item1 and item2 on table\n");
			sem_post(&csem[(cind++)%nc]);
		}
		sem_post(&mutex);
		sem_wait(&ag);
	}
}

void *smokerA(void *param)
{
		int f=(int)param;
	while(1)
	{
		sem_wait(&asem[f]);

		sem_wait(&mutex);
		printf("Pickup item2 and item3 by smokerA%d\n",f);
		sem_post(&ag);
		sem_post(&mutex);
		printf("A%d smoking\n",f);
	}
}

void *smokerB(void *param)
{
	int f=(int)param;
	printf("Inside smokerB %d\n",f);
	while(1)
	{
		printf("Wait\n");
		sem_wait(&bsem[f]);

		sem_wait(&mutex);
		printf("Pickup item1 and item3 by smokerB%d\n",f);
		sem_post(&ag);
		sem_post(&mutex);
		printf("B%d smoking\n",f);
	}
}

void *smokerC(void *param)
{
	int f=(int)param;
	while(1)
	{
		sem_wait(&csem[f]);

		sem_wait(&mutex);
		printf("Pickup item1 and item2 by smokerC%d\n",f);
		sem_post(&ag);
		sem_post(&mutex);
		printf("C%d smoking\n",f);
	}
}


int main()
{
	pthread_t agid,aid,bid,cid;

	sem_init(&mutex,0,1);
	for(int i=0;i<5;i++)
		sem_init(&asem[i],0,0);
	for(int i=0;i<nb;i++)
		sem_init(&bsem[i],0,0);
	for(int i=0;i<nc;i++)
		sem_init(&csem[i],0,0);
	//sem_init(&a,0,0);
	//sem_init(&b,0,0);
	//sem_init(&c,0,0);
	printf("Enter sequence of smoker types(0,1,2):");
	aind=0;bind=0;cind=0;
	int tna=0,tnb=0,tnc=0;
	for(int i=0;i<10;i++)
	{
		int x;
		scanf("%d",&x);
		if(x==0){
			pthread_create(&aqueue[tna],NULL,smokerA,(void *)(tna));
			tna++;
		}
		else if(x==1){
			pthread_create(&bqueue[tnb],NULL,smokerB,(void *)(tnb));
			tnb++;
		}
		else{
			pthread_create(&cqueue[tnc],NULL,smokerC,(void *)(tnc));
			tnc++;
		}
	}
	na=tna;
	nb=tnb;
	nc=tnc;
	

	//na=aind;
	//nb=bind;
	//nc=cind;
	aind=0;bind=0;cind=0;
	pthread_create(&agid,NULL,agent,NULL);

	for(int i=0;i<na;i++)
		pthread_join(aqueue[i],NULL);
	for(int i=0;i<nb;i++)
		pthread_join(bqueue[i],NULL);
	for(int i=0;i<nc;i++)
		pthread_join(cqueue[i],NULL);
	return 0;
}