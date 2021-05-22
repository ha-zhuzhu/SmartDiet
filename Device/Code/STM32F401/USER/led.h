#ifndef __LED_H
#define __LED_H
#include "sys.h"

#define LED_BLU PDout(2)
#define LED_RED PCout(10)
#define LED_GRN PAout(5)

void LED_Init(void);

#endif
