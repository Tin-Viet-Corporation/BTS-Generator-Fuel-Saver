#include "C:\Program Files (x86)\PICC\Devices\18f26k20.h"
#device adc = 10
#fuses hs, nolvp, protect, MCLR, NOPUT, BORV27
#fuses WDT16384
#use delay(clock = 24000000)
#include "C:\Program Files (x86)\PICC\Drivers\stdlib.h"

#define sw_mode PIN_A3
#define sw_up PIN_A2
#define sw_down PIN_A1
#define sw_exit PIN_A0

#define val_mode() !input(sw_mode)
#define val_up() !input(sw_up)
#define val_down() !input(sw_down)
#define val_exit() !input(sw_exit)

#define clock_reset PIN_A3 // TAO XUNG CHONG TREO

#define backlight_lcd PIN_A4
#define backlight_on() output_high(backlight_lcd);
#define backlight_off() output_low(backlight_lcd);

#define cb_mn PIN_C2
#define cb_ac PIN_C3

#define status_mn() !input(cb_mn)
#define status_ac() !input(cb_ac)

#define out_fire PIN_B2
#define out_temp PIN_B3
#define out_error PIN_B5
#define out_delay PIN_B4
#define out_mn PIN_B6
#define out_ac PIN_B7

#define ree(x) read_eeprom(x);
#define wee(x, y) write_eeprom(x, y);
#define ree16(x) read_eeprom16(x);
#define wee16(x, y) write_eeprom16(x, y);

#define clear_lcd() lcd_printf(0);
#define yesno() lcd_printf(9);
#define loading() lcd_printf(10);

#define sch_1_s_set() lcd_printf(11);

#define size_pass 5