#ifndef PICC_H
#define PICC_H

#include "C:\Program Files (x86)\PICC\Devices\18f26k20.h"
#device adc = 10
#fuses hs, nolvp, protect, MCLR, NOPUT, BORV27
#fuses WDT16384
#use delay(clock = 24000000)
#include "C:\Program Files (x86)\PICC\Drivers\stdlib.h"

#endif