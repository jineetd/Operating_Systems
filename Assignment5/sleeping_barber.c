#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>

int seq[1024];

typedef struct{
	int index;
	int type;
}cust;
//semaphore variables
sem_t barber,customer,mutex;
int no_of_chairs;
int waiting=0;
int priority_waiting=0;
int pcount=0;
void *barber_fun(void *param)
{
	while(1)
	{
		sem_wait(&customer); //check if the barber is sleeping
		sem_wait(&mutex);
		waiting=waiting-1;
		sem_post(&barber);//barber takes a person

		printf("Barber doing a hair cut\n");
		sem_post(&mutex);

		
	}
}


void *customer_fun(void *param)
{
	cust *ptr;
	ptr=(cust *)param;
	int cust_number=ptr->index;
	int type=ptr->type;

	sem_wait(&mutex);
	if(waiting<no_of_chairs)
	{
		waiting=waiting+1;
		sem_post(&customer);
		printf("Customer%d Waiting for barber...\n",cust_number);

		sem_post(&mutex);
		
		sem_wait(&barber);//wait for the barber to be free
		

	}
	else{
		printf("Customer%d leaving due to no chairs...\n",cust_number);
		sem_post(&mutex);
	}
	pthread_exit(0);
}

int main()
{
	printf("Enter the number of chairs in the waiting room:");
	scanf("%d",&no_of_chairs);

	int no_of_customers;
	printf("Enter number of customers:");
	scanf("%d",&no_of_customers);

	printf("Enter sequence of staff(1) and student(0):");
	for(int i=0;i<no_of_customers;i++)
		scanf("%d",&seq[i]);

	cust mat[no_of_customers];
	for(int i=0;i<no_of_customers;i++)
	{
		mat[i].index=i;
		mat[i].type=seq[i];
	}

	sem_init(&barber,0,0);
	sem_init(&customer,0,0);
	sem_init(&mutex,0,1);
	sem_init(&priority_waiting,0,0);


	unsigned long int btid,ctid[no_of_customers];
	

	for(int i=0;i<no_of_customers;i++){
		pthread_create(&ctid[i],NULL,customer_fun,&mat[i]);
	}

	pthread_create(&btid,NULL,barber_fun,NULL);

	for(int i=0;i<no_of_customers;i++)
		pthread_join(ctid[i],NULL);
	pthread_join(btid,NULL);

	return 0;


}
