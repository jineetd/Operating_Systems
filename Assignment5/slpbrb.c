#include<stdio.h>
#include<semaphore.h>
#include<pthread.h>

sem_t customer,mutex,barber,lock0,lock1;

int max_customers;
int waiting =0;
//int index=0;
int *seq;
int stcount=0,staffcount=0;
sem_t queue[1024];

typedef struct{
	int num;
	int type;
}cust;
void *barber_fun(void *param)
{
	while(1)
	{
		sem_wait(&customer);
		sem_wait(&mutex);
		waiting=waiting-1;

		sem_post(&barber);
		sem_post(&mutex);
	}
}

void *customer_fun(void *param)
{
	cust *ptr=(cust *)param;
	int type=ptr->type;
	sem_wait(&mutex);
	if(waiting<max_customers)
	{
		waiting=waiting+1;
		sem_post(&customer);

		sem_post(&mutex);

		//sem_wait(&mutex);

		if(type==0)
			stcount++;
		else if(stcount>=1 && type==1)
			sem_wait(&lock1);


		//sem_post(&mutex);
		if(type==0)
			printf("student waiting for haircut...\n");
		else
			printf("Staff waiting for haircut...\n");
		sem_wait(&barber);
		if(type==0)
			stcount--;
		if(stcount==0 && type==0)
			sem_post(&lock1);
		printf("Hair cut\n");
	}
	else
	{
		if(type==0)
			printf("Student leaving due to no space\n");
		else
			printf("Staff leaving due to no space\n");
		sem_post(&mutex);
	}
}

int main()
{
	printf("Enter size of queue:");
	scanf("%d",&max_customers);

	for(int i=0;i<max_customers;i++)
		sem_init(&queue[i],0,0);
	sem_init(&barber,0,0);
	sem_init(&customer,0,0);
	sem_init(&mutex,0,1);
	sem_init(&lock0,0,0);
	sem_init(&lock1,0,0);
	//int input[10];
	cust array[10];
	for(int i=0;i<10;i++)
	{
		int x;
		scanf("%d",&x);
		if(x==0)
		{
			array[i].type=0;
		}
		else
		{
			array[i].type=1;
		}
	}
	pthread_t cid[10];
	//index=0;
	for(int i=0;i<10;i++)
	{
		pthread_create(&cid[i],NULL,customer_fun,&array[i]);
	}
	pthread_t tid;
	pthread_create(&tid,NULL,barber_fun,NULL);
	printf("Threads initialized....\n");
	for(int i=0;i<10;i++)
		pthread_join(cid[i],NULL);
	pthread_join(tid,NULL);

	return 0;

}