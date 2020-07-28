#include<stdio.h>
#include<semaphore.h>
#include<pthread.h>

sem_t waiting_room_mutex,barber_room_mutex,barber;

int no_of_waiting_chairs;
sem_t no_of_barber_chairs_free;
int k;
int barber_chair_states[1024];
sem_t barber_chair[1024];


void *barber_fun(void *param)
{
	while(1)
	{
		sem_wait(&barber);
		sem_wait(&barber_room_mutex);
		int mychair;
		for(int i=0;i<k;i++)
		{
			if(barber_chair_states[i]==1)
			{
				mychair=i;
				break;
			}
		}
		barber_chair_states[mychair]=2;//hair cutting
		sem_post(&barber_room_mutex);

		printf("Barber cutting hair...\n");

		sem_wait(&barber_room_mutex);
		barber_chair_states[mychair]=0;//finished
		sem_post(&barber_chair[mychair]);
		sem_post(&barber_room_mutex);
	}
}

void *customer_fun(void *param)
{
	sem_wait(waiting_room_mutex);
	if(no_of_waiting_chairs==0)
	{
		printf("Customer leaves due to no chairs...\n");
		sem_post(waiting_room_mutex);
	}
	no_of_waiting_chairs--;
	sem_post(&waiting_room_mutex);

	//wait for free barber chair
	sem_wait(&no_of_barber_chairs_free);

	//a chair for barber is free
	//remove a chair from waiting queue
	sem_wait(&barber_room_mutex);
	sem_wait(&waiting_room_mutex);
	no_of_waiting_chairs++;
	sem_post(&waiting_room_mutex);

	//grab a chair
	int mychair;
	for(int i=0;i<k;i++)
	{
		if(barber_chair_states[i]==0){
			mychair=i;
			break;
		}
	}
	barber_chair_states[mychair]=1;//hair to be cut
	sem_post(&barber_room_mutex);

	sem_post(&barber);
	sem_wait(&barber_chair[mychair]);

	printf("Hair cut done\n");
	sem_post(&no_of_barber_chairs_free);
}



int main()
{
	printf("Enter no of barber chairs:");
	scanf("%d",&k);
	sem_init(&no_of_barber_chairs_free,0,k);
	printf("Enter no of waiting chairs:");
	scanf("%d",&no_of_waiting_chairs);

	for(int i=0;i<k;i++){
		barber_chair_states[i]=0;
		sem_init(&barber_chair[i],0,0);
	}
	sem_init(&waiting_room_mutex,0,1);
	sem_init(&barber_room_mutex,0,1);
	sem_init(&barber,0,0);

	
}