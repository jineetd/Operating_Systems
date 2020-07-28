#include<stdio.h>
#include<pthread.h>
#include<sys/wait.h>
#include<math.h>
#include<stdlib.h>

void *runner(void *param);

typedef struct{
	int *A;
	int *B;
	int i,j,index;
	int *res;
} matrix;

int main()
{
	printf("**********************FAST NUMBER MULTIPLICATION**********************\n");
	int n;
	printf("Enter n to carry out n digit multiplication:");
	scanf("%d",&n);
	int num1,num2;
	printf("Enter first number to be added:");
	scanf("%d",&num1);
	printf("Enter second number to be added:");
	scanf("%d",&num2);
	int limit=pow(10,n+1);
	if(num1>=limit || num2>=limit)
	{
		printf("Error entering the input!!!!!!\n");
		return 0;
	}
	int *A,*B;
	A=(int *)malloc(sizeof(int)*n);
	B=(int *)malloc(sizeof(int)*n);
	int k=n;
	while(num1)
	{
		A[k-1]=num1%10;
		num1/=10;
		k--;
	}
	while(k)
	{
		A[k-1]=0;
		k--;
	}
	/*for(int i=0;i<n;i++)
		printf("%d ",A[i]);*/
	k=n;
	while(num2)
	{
		B[k-1]=num2%10;
		num2/=10;
		k--;
	}
	while(k)
	{
		B[k-1]=0;
		k--;
	}

	int *res;
	res=(int *)malloc(sizeof(int)*(2*n-1));
	//printf("HI\n");
	pthread_t tid[2*n-1];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	matrix mat[2*n-1];
	int ind=0;
	for(int j=0;j<n;j++)
	{
		mat[ind].A=A;
		mat[ind].B=B;
		mat[ind].i=0;
		mat[ind].j=j;
		//printf("%d %d\n",mat[ind].i,mat[ind].j);
		mat[ind].res=res;
		ind++;
	}
	for(int i=1;i<n;i++)
	{
		mat[ind].A=A;
		mat[ind].B=B;
		mat[ind].i=i;
		mat[ind].j=n-1;
		//printf("%d %d\n",mat[ind].i,mat[ind].j);
		mat[ind].res=res;
		ind++;	
	}
	//printf("pthread_create error\n");
	for(int i=0;i<2*n-1;i++)
	{
		mat[i].index=i;
		pthread_create(&tid[i],&attr,runner,&mat[i]);
	}
	for(int i=0;i<2*n-1;i++)
	{
		pthread_join(tid[i],NULL);
	}
	//printf("%d %d\n",mat[ind].i,mat[ind].j);
	//printf("ind: %d",ind);
	/*for(int i=0;i<2*n-1;i++)
		printf("%d ",res[i]);
	printf("\n");*/
	//printf("Computation inside the main thread:\n");

	int final_res[2*n];
	int carry=0;
	for(int i=2*n-1;i>=0;i--)
	{
		final_res[i]=res[i]+carry;
		carry=final_res[i]/10;
		final_res[i]=final_res[i]%10;
	}
	final_res[2*n-1]=carry;
	printf("\nFinal result of multiplication: \n");
	int leadingZero=1;
	for(int i=0;i<2*n-1;i++)
	{
		if(final_res[i]==0 && leadingZero==1)
			continue;
		leadingZero=0;
		printf("%d ",final_res[i]);
	}
	printf("\n");
	return 0;
}


void *runner(void *param)
{

	matrix *mat;
	mat=(matrix *)param;
	int *A,*B,*res;
	int i,j,index;
	A=mat->A;
	B=mat->B;
	res=mat->res;
	i=mat->i;
	j=mat->j;
	//printf("Inside runner: %d %d\n",i,j);
	index=mat->index;
	int ans;
	while(i<j)
	{
		(ans)+=(B[i]*A[j]);
		ans+=(B[j]*A[i]);
		i++;
		j--;
	}
	if(i==j)
		(ans)+=(B[i]*A[i]);
	res[index]=ans;
	return NULL;
}