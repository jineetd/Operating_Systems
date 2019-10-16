#include<linux/kernel.h>  
#include<linux/types.h>
#include<linux/syscalls.h>
#include "add_syscall.h"

#define OVERFLOW 2147483648 //1 00000000 00000000000000000000000
/*ERROR */


/*Implementing the power function through modular exponentiation*/
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

/*Gives the number corresponding to high to low bit of binary form of arr*/
long int convertToInt(long int arr,long int low,long int high) 
{ 
    long int f = 0, i;
    for (i = high ; i >= low ; i-- ) { 
        if((arr&(1<<(i))))
        f = f +  power(2,(i-low)); 
    } 
    return f; 
} 

/*Precondition: num1>0 && num2>0
Also num1 and num2 must have valid IEEE 754 floating point representation
They must not be infinite or NaN*/


/*Post condition:  The long int returned to the system call must have
a valid IEEE 754 binary representation or else it must return a error
code*/
SYSCALL_DEFINE2(add_syscall,int,num1,int,num2)
{
	
      int exp1 , exp2 , man1 , man2 ; 
      int man_res; 
      int res ; 
      /*Extracting the 8 bit exponent of num1 and num2*/
      exp1 = convertToInt(num1,23,30) ;
      exp2 = convertToInt(num2,23,30) ;
      
      /*Extracting the 23 bit mantissa of num1 and num2*/
      man1 = convertToInt(num1,0,22) ; 
      man2 = convertToInt(num2,0,22) ;
      
      /*inputs are already validated in wrapper function*/

      /*Converting to 1.fraction format by setting 23rd bit*/
      man1 = man1^(1<<23) ;
      man2 = man2^(1<<23) ;
      

      /*We make the exponents of numbers equal by shifting mantissa of
      one of the numbers*/
      if(exp1>exp2)
      {
        int diff;
        diff = exp1 - exp2 ; 
        man2 = (man2>>diff) ;
        exp2 = exp1 ;  
      } 
      else
      {
        int diff;
        diff = exp2 - exp1 ; 
        man1 = (man1>>diff) ;
        exp1 = exp2 ; 
      }

      
      man_res = man1+man2 ; 
      if(man_res&(1<<24))
      {
        exp1 += (1) ; 
        man_res = man_res>>1 ;
      }
      /*1.fraction format to mantissa*/
      if(man_res&(1<<23))
          man_res = man_res^(1<<23) ;

      /*checking for appropriate output floating number*/
      if(exp1==255)
        return 0;//infinity or NAN
     /*system call would not return 0 as a valid floating point.
     So we can use it for returning error*/

      exp1 = exp1<<23 ;
      
      
      /*Concatenating the exponent and mantissa to give the binary 
      form of the resultant floating number*/
      
      res = exp1|man_res ; 
      return res; 
	
}