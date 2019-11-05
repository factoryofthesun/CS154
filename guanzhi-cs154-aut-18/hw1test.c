#include <stdio.h>

/*True = big, false = little */ 
int main() {
	int endian, foo; 
	foo = 0x0100;
	endian = ((char*) &foo)[0]; 
	printf("value: %d\n", endian);
	printf("hello world, from a %s-endian machine\n",
		endian ? "big" : "little"); 
	
	//At least one bit is 0 
	int x = 0xFFFFFFFF;
	int a = !!~x;
	printf("A test 1: %d\n", a); 
	x = 0x01459284; 
	a = !!~x; 
	printf("A test 2: %d\n", a); 
	x = 0xFF; 
	a = !!~x; 
	printf("A test 3: %d\n", a); 
	//At least one bit is 1 
	x = 0x00; 
	int b = !!x;
	printf("B test 1: %d\n", b);
	x = 0x093812; 
	b = !!x; 
	printf("B test 2: %d\n", b); 
	//Any of bits in most significant nibble of x is 0
	//In little endian, this is the FIRST nibble at the LAST byte 
	x = 0xF4000000;
	int c = !!((x >>(((sizeof(int)-1)<<3)+4) & 0xF)^0xF); 
	printf("C test 1: %d\n", c); 
	x = 0xA2000000;
	c = !!((x >>(((sizeof(int)-1)<<3)+4) & 0xF)^0xF); 
	printf("C test 2: %d\n", c); 
	//Any of bits in least significant nibble of x is 1 
	x = 0x00000001; 
	int d = !!(x & 0x0F); 
	printf("D test 1: %d\n", d); 
	x = 0xFFFFFFF0; 
	d = !!(x & 0x0F); 
	printf("D test 2: %d\n", d);  
	//Most significant and least significant bytes are equal 
	x = 0xFF000055;
	int e = (x >> ((sizeof(int)-1)<<3) & 0xFF) == (x & 0xFF);
	x = 0x01000001;
	printf("E test 1: %d\n", e); 
	e = (x >> ((sizeof(int)-1)<<3) & 0xFF) == (x & 0xFF);
	x = 0x29230029;
	printf("E test 2: %d\n", e);
	e = (x >> ((sizeof(int)-1)<<3) & 0xFF) == (x & 0xFF);	 
	printf("E test 3: %d\n", e);
	//Any even bit of x is 1 
	x = 0x00100000; 
	int f = !!(x & 0x55555555); 
	printf("F test 1: %d\n", f); 
	x = 0xAAAAAAAA; 
	f = !!(x & 0x55555555); 
	printf("F test 2: %d\n", f); 
	} 
