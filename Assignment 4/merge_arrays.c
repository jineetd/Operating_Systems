#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

typedef struct{
	int index;
	int isA;
	int num;
} param;

//global variables 
int n,m;
int *A,*B,*res;

//declaration of the function used for getting the inputs
void *getter(void *p);

//declaration of the function used for finding appropriate index in the result array
void *getPosition(void *p);

//declaration of function for printing the result
void *printer(void *p);

int main()
{
	printf("**************************FAST ARRAY MERGING**************************\n");
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&tid,&attr,getter,NULL);
	//getting inputs
	pthread_join(tid,NULL);
	long unsigned int tids[n+m];

	//argument passing for the threads
	param holder[m+n];
	for(int i=0;i<n;i++)
	{
		holder[i].index=i;
		holder[i].isA=1;
		holder[i].num=A[i];
	}
	for(int i=n;i<m+n;i++)
	{
		holder[i].index=i-n;
		holder[i].isA=0;
		holder[i].num=B[i-n];
	}

	for(int i=0;i<m+n;i++)
		pthread_create(&tids[i],&attr,getPosition,&holder[i]);
	for(int i=0;i<m+n;i++)
		pthread_join(tids[i],NULL);

	pthread_create(&tid,&attr,printer,NULL);
	//printing the resultant array
	pthread_join(tid,NULL);
	return 0;

}

void *getter(void *p)
{
	printf("Enter the size of first array:");
	scanf("%d",&n);
	printf("Enter the size of second array:");
	scanf("%d",&m);

	//allocating memory for the arrays
	A=(int *)malloc(n*sizeof(int));
	B=(int *)malloc(m*sizeof(int));
	res=(int *)malloc((n+m)*sizeof(int));

	printf("Enter the %d elements of first array:",n);
	for(int i=0;i<n;i++)
		scanf("%d",&A[i]);

	printf("Enter the %d elements of the second array:",m);
	for(int i=0;i<m;i++)
		scanf("%d",&B[i]);
	
	

	pthread_exit(0);
}


void *getPosition(void *p)
{
	//printf("%d\n",pthread_self());
	param *val;
	val=(param *)p;

	int low,high;
	low=0;
	int i=val->index;
	int number=val->num;
	int *target;
	//int flag=0;
	if(val->isA==1){
		high=m-1;
		target=B;
	}
	else{
		high=n-1;
		target=A;
	}
	if(number>target[high])
	{
		low=high+1;
	}
	//int k=high;
	while(low<high)
	{
		int mid=(low+high)/2;

		if(target[mid]<number)
		{
			low=mid;
		}
		else
			high=mid;
		if(low==high-1)
		{
			if(target[low]<number)
				low=high;
			break;
		}
	}
	
	res[low+i]=number;
	pthread_exit(0);

}


void *printer(void *p)
{
	printf("\nThe merged array is :\n");
	for(int i=0;i<n+m;i++)
		printf("%d ",res[i]);
	printf("\n");
	pthread_exit(0);
}