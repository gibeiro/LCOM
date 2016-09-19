#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>

#include "i8254.h"
#include "i8042.h"
#include "constants.h"

int kbd_subscribe_int();

int kbd_unsubscribe_int();

unsigned long kbd_read();
