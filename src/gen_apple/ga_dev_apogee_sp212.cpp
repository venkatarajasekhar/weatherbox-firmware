#include "ga_dev_apogee_sp212.h"

void ga_dev_apogee_sp212_open(void){
    pinMode(_PIN_GA_APOGEE_SP212_, INPUT);
}

int ga_dev_apogee_sp212_read(void){
    int v = analogRead(_PIN_GA_APOGEE_SP212_);
    return v;
}
