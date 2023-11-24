#ifndef CONTROL_H
#define CONTROL_H

#include "port.h"
#include "picc.h"

#define val_mode() !input(sw_mode)
#define val_up() !input(sw_up)
#define val_down() !input(sw_down)
#define val_exit() !input(sw_exit)

#define backlight_on() output_high(backlight_lcd);
#define backlight_off() output_low(backlight_lcd);

#define status_mn() !input(cb_mn)
#define status_ac() !input(cb_ac)

#endif