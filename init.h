#ifndef init_h
#define init_h

#include "util.h"

#include <stdint.h>
#include <stdbool.h>

bool wlan_init_from_config(void);
void wlan_init_start_recovery(void);

#endif
