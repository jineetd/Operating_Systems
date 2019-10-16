#include<stdio.h>
#include<math.h>
#include "addWrapper.h"

#define OVERFLOW 2147483648  //1 00000000 00000000000000000000000


/*converts the float number to the IEEE 754 single precision format*/
int getBinary(number var) 
{ 
    int ans=0 ;
  	
  	int i=0 ;
  	
  	for(;i<23;i++)
  	{
  		ans = ans^((1<<i)&(var.bitField.mantissa)) ;
  	}
  	for(;i<=30;i++)
  	{
  		ans = ans^(((1<<(i-23))&var.bitField.exponent)<<23) ;
  	}
  	ans = ans^(((1<<(i-31))&var.bitField.sign)<<31) ;
    
	return ans ; 
     
} 
/*Returns corresponding integer for high to low bits of binary 
respresentation of arr*/
long int convertToInt(long int arr,long int low,long int high) 
{ 
    long int f = 0, i;
    for (i = high ; i >= low ; i-- ) { 
        if((arr&(1<<(i))))
        f = f +  power(2,(i-low)); 
    } 
    return f; 
} 

/*Returns the float value corresponding to the 32-bit binary representation
contained in long int variable*/
float getFloat(long int n)
{
	long int temp_var = 0 ; 
	for(int i = 0 ; i<=31 ; i++ )
	{
		temp_var ^= ((n&1)<<i) ;  
		n = n>>1 ;
	}
	number var ; 
	long int k = convertToInt(temp_var,0,22) ;
	var.bitField.mantissa=k;
	k=convertToInt(temp_var,23,30);
	var.bitField.exponent=k;
	long int h = n&(1<<31) ;
	if(h) 
		var.bitField.sign=1;
	else
		var.bitField.sign=0;
	return var.f; 
}
/*Power function implemented using modular exponentiation*/
long int power(long int a,long int n)
{
	long int res=1;
	if(n==0)
		return res;
	while(n)
	{
		if(n%2==1)
			res=res*a;
		a=a*a;
		n=n/2;
	}
	return res;
}

/*addFloats is our wrapper function which will be called in the user space*/
float addFloats(float num1,float num2)
{
	number n1,n2;
	n1.f=num1;
	n2.f=num2;
	/*validating the inputs num1 and num2*/
	if(num1<=0 || num2<=0)
	{
		printf("\033[0;31m");
		printf("Error:Inputs are not VALID positive integers...\n");
		printf("\033[0m");
		return 0;
	}
	else if(num1>=INFINITY || num2>=INFINITY)
	{
		printf("\033[0;31m");
		printf("Error:Inputs are not valid. They exceed the range of single precision floating point numbers\n");
		printf("\033[0m");
		return INFINITY;
	}
	int x,y;
	x=getBinary(n1);
	y=getBinary(n2);
	long int res;
	res=add_syscall(x,y);
	//printf("%ld\n",res);
	if(res==0)
	{
		printf("\033[0;31m");
		printf("Error:Floating point Overflow...\n");
		printf("\033[0m");
		return INFINITY;
	}
	else
		return getFloat(res);
}