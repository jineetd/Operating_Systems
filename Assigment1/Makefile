addFloats: wrapperFunctions.o
	gcc -o addFloats driver.c wrapperFunctions.o
	@echo executable called addFloats generated.....
	@echo Use clean after using the driver files....

wrapperFunctions.o: wrapperFunctions.c
	gcc -c -w wrapperFunctions.c addWrapper.h
	@echo Object file of wrapper functions generated....

clean: 
	@echo Clearing all the executable and object files......
	rm -f wrapperFunctions.o
	rm -f addFloats
