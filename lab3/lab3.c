#include "test3.h"
#include "timer.h"
#include <stdio.h>

int main()
{
	sef_startup();

	unsigned long ass_0 = 0;
	unsigned long ass_1 = 1;
	unsigned short n = 3;
	unsigned short toggle[6] = {0,1,2};
	unsigned short idle = 3;
	/*
	 * This still misses the param definitions, which should be implemented when testing.
	 */

	//kbd_test_scan(ass_0);
	//kbd_test_leds(n, toggle);
	//kbd_test_timed_scan(idle);
	kbd_test_scan(ass_1);

	return 0;
}
