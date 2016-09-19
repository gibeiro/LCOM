#include <minix/syslib.h>
#include <minix/drivers.h>

#include "i8254.h"
#include "constants.h"

static int timer_hook_id = TIMER_DEFAULT_HOOK_ID;

int timer_subscribe_int(void);

int timer_unsubscribe_int();
