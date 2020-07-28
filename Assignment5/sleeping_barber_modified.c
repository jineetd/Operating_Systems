#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<stdlib.h>
int staff,student,wq;
int *seq;
sem_t stq[10],sdq[10];
sem_t barber,mutex,customer;
int waiting=0;


typedef struct{
	int num;
	int type;
}cust;

void *barber_fun(void *param)
{
	while(1)
	{
		sem_wait(&customer);//barber sleeps 
		sem_wait(&mutex);
		waiting=waiting-1;
		printf("Barber doing a hair cut...\n");
		sem_post(&barber);//barber releases the lock
		sem_post(&mutex);
	}

}


void *cust_fun(void *param)
{
	cust *ptr=(cust *)param;
	int type=ptr->type;
	int num=ptr->num;

	sem_wait(&mutex);
	if(waiting<wq)
	{
		
		waiting=waiting+1;
		sem_post(&customer);
		printf("Customer of type %d num %d waiting in queue...\n",type,num);
		sem_post(&mutex);

		sem_wait(&barber);//waiting for the barber

	}
	else
	{
		printf("Customer of type %d num %d leaving due to no space\n",type,num);
		sem_post(&mutex);
	}
	pthread_exit(0);
}
int main()
{
	printf("*******************SLEEPING BARBER SIMULATION*******************\n");
	printf("Enter size of waiting queue:");
	scanf("%d",&wq);
	printf("Enter the number of students:");
	scanf("%d",&student);
	printf("Enter the number of staff:");
	scanf("%d",&staff);
	printf("\n Enter the sequence of staff(1) and student(0) approaching the barber:");
	sem_init(&barber,0,0);
	sem_init(&customer,0,0);
	sem_init(&mutex,0,1);
	seq=(int *)malloc(sizeof(int)*(staff+student));
	for(int i=0;i<staff+student;i++)
		scanf("%d",&seq[i]);
	cust holder[staff+student];
	int st=0,sd=0;
	for(int i=0;i<staff+student;i++)
	{
		if(seq[i]==0)
		{
			holder[i].num=st;
			holder[i].type=0;
			sem_init(&stq[st],0,0);
			st++;
		}
		else
		{
			holder[i].num=sd;
			holder[i].type=1;
			sem_init(&sdq[sd],0,0);
			sd++;
		}

	}


	pthread_t tid;
	pthread_create(&tid,NULL,barber_fun,NULL);

	pthread_t ids[staff+student];
	for(int i=0;i<staff+student;i++)
	{
		pthread_create(&ids[i],NULL,cust_fun,&holder[i]);
		//sleep(1);
	}
	

	for(int i=0;i<staff+student;i++)
	{
		pthread_join(ids[i],NULL);
	}
	pthread_join(tid,NULL);

	return 0;

}