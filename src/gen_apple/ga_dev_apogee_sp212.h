#include <Arduino.h>

#define _PIN_GA_APOGEE_SP212_ A2

#ifndef GA_DEV_APOGEE_SP212_H
#define GA_DEV_APOGEE_SP212_H
void ga_dev_apogee_sp212_open(void);
int ga_dev_apogee_sp212_read(void);
#endif

