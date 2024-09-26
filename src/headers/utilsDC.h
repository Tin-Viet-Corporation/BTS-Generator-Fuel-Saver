#ifndef UTILSDC_H
#define UTILSDC_H

#include "./const.h"

#define isBelowDCLowLv1AndAboveLv2(adcAccu, DCLowLv1, delta) adcAccu > DC_LOW_LVL_2 && (adcAccu <= DCLowLv1 - delta)

#endif