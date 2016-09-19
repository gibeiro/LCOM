#include "test4.h"
#include <stdio.h>

int main()
{
	sef_startup();

	unsigned short s = 50;
	unsigned short t = 5;
	short l = 400;
	unsigned short tol = 50;



	test_packet(s);
	printf("OK! \n");
	test_async(t);
	printf("OK! \n");
	test_config();
	printf("OK! \n");
	test_gesture(l,tol);

	int i = ms_unsubscribe_int();
	if (i != 0) {
		printf("ms_unsubscribe_int() failed to run. \n");
		return i;
	}

	printf("\nFINISHED! \n");

	return 0;
}
