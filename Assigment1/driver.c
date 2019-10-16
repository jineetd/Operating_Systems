#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include "addWrapper.h"
#define MY_SYSCALL 434

int main(int argc,char *argv[])
{
	float num1,num2,res;
	printf("Enter NUmber 1 and NUmber 2\n");
	scanf("%f %f",&num1,&num2);
	
	res=addFloats(num1,num2);
	if(res!=NAN && res!=INFINITY){
	printf("The result of Adding %f with %f is\n",num1,num2);
	
	printf("%f\n",res);
	}
	return 0;
}
