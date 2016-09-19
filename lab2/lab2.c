#include "timer.h"

int main(){
	sef_startup();

	unsigned long time = 3, timer = 0, freq = 60;

	timer_test_square(freq);
	timer_test_int(time);

	return 0;
}
