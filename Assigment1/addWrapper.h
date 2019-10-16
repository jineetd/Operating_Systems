#ifndef __ADD_WRAPPER_H_
#define __ADD_WRAPPER_H_
#define add_syscall(num1,num2)(syscall(434,num1,num2))

/*Bitfields data structure implementation using union and structure
IEEE 754 format 1 bit sign,8 bit exponent,23 bit mantissa
number.bitField.exponent gives long int number corresponding to the exponent
of number.f*/
typedef union { 
  
    float f; 
    struct
    { 
        long int mantissa : 23; 
        long int exponent : 8; 
        long int sign : 1; 
  
    } bitField; 
} number;

/*Declarations of the wrapperFunctions*/
long int convertToInt(long int arr,long int low,long int high);
float getFloat(long int n);
long int power(long int a,long int n);
int getBinary(number var);
float addFloats(float num1,float num2); 

#endif
