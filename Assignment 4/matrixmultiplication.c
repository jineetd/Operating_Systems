#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
typedef struct matr{
	int *a;
	int *b;
	int *c;
	int m;
	int k;
	int n;
	int i,j;
} matrix;
void *subtask(void *param);
void *calc(void *param);
int main()
{
	int m,k1,k2,n;
	printf("\n***************FAST MATRIX MULTIPLICATION*****************\n");
	printf("Enter the dimensions of first matrix:\n");
	scanf("%d %d",&m,&k1);
	printf("Enter the dimensions of second matrix:\n");
	scanf("%d %d",&k2,&n);
	if(k1!=k2)
	{
		printf("Invalid dimensions of matrix.......\n");
		return 0;
	}
	int *A,*B,*C;
	A=(int *)malloc(m*k1*sizeof(int));
	B=(int *)malloc(k2*n*sizeof(int));
	C=(int *)malloc(m*n*sizeof(int));
	printf("Enter values of %d X %d matrix:\n",m,k1);
	for(int i=0;i<m;i++)
	{
		for(int j=0;j<k1;j++)
		{
			scanf("%d",A+(i*k1)+j);
		}
	}
	printf("Enter values of %d X %d matrix:\n",k2,n);
	for(int i=0;i<k2;i++)
	{
		for(int j=0;j<n;j++)
		{
			scanf("%d",B+(i*n)+j);
		}
	}
	//printf("%ld %ld\n",A,B);
	matrix mat;
	//printf("%ld\n",sizeof(mat));
	

	mat.m=m;
	//printf("Hi\n");
	mat.k=k1;
	mat.n=n;
	mat.a=A;
	//printf("Hi\n");
	mat.b=B;
	mat.c=C;
	//printf("%d %d %d\n",mat->m,*(mat->b),mat->k);
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	//printf("Calling subtask....\n");
	pthread_create(&tid,&attr,subtask,&mat);
	//printf("tid:%d\n",tid);
	pthread_join(tid,NULL);
	printf("\033[0;31m");
	printf("Resultant matrix:\n");
	printf("\033[0m");
	printf("\033[1;34m");
	for(int i=0;i<m;i++)
	{
		for(int j=0;j<n;j++)
		{
			printf("%d \t",*(C+i*n+j));
		}
		printf("\n");
	}
	printf("\033[0m");
	//printf("%d\n",*(mat->a+(2*k1)+1));
	return 0;

}


void *subtask(void *param)
{
	matrix *mat_pointer;
	mat_pointer=(matrix *)param;
	int m=mat_pointer->m;
	int k=mat_pointer->k;
	int n=mat_pointer->n;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	long unsigned int tid[m][n];
	matrix holder[m][n];
	for(int i=0;i<m;i++)
	{
		for(int j=0;j<n;j++)
		{
			holder[i][j].m=m;
			holder[i][j].n=n;
			holder[i][j].k=k;
			holder[i][j].a=mat_pointer->a;
			holder[i][j].b=mat_pointer->b;
			holder[i][j].c=mat_pointer->c;
			holder[i][j].i=i;
			holder[i][j].j=j;
		}
	}
	for(int i=0;i<m;i++)
	{
		for(int j=0;j<n;j++)
		{
			//printf("Calcaulating %d %d indices....\n",i,j);
			pthread_create(&tid[i][j],&attr,calc,&holder[i][j]);
			//pthread_join(tid[i][j],NULL);
		}
		/*for(int j=0;j<n;j++)
			pthread_join(tid[i][j],NULL);*/
	}
	for(int i=0;i<m;i++)
	{
		for(int j=0;j<n;j++)
			pthread_join(tid[i][j],NULL);
	}
	pthread_exit(0);

}

void *calc(void *param)
{

	matrix *mat_pointer;
	mat_pointer=(matrix *)param;
	int *C,*B,*A;
	A=mat_pointer->a;
	B=mat_pointer->b;
	C=mat_pointer->c;
	int ix,jx,mx,nx;
	mx=mat_pointer->m;
	nx=mat_pointer->n;
	ix=mat_pointer->i;
	jx=mat_pointer->j;
	//printf(" i:%d j:%d \n",ix,jx);
	int kx=mat_pointer->k;
	int sum=0;
	for(int y=0;y<kx;y++)
	{
		int v1=*(A+(ix*kx)+y);
		int v2=*(B+(nx*y)+jx);
		sum+=(v1*v2);
		//printf("Sum:%d\n",sum);
	}
	*(C+(ix*nx)+jx)=sum;
	pthread_exit(0);
}
