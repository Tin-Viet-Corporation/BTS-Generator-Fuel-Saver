#include "C:\Program Files (x86)\PICC\Devices\18f26k20.h"
#device adc = 10
#fuses hs, nolvp, protect, MCLR, NOPUT, BORV27
#include "C:\Program Files (x86)\PICC\Drivers\stdlib.h"
#fuses WDT16384
#use delay(clock = 24000000)
#include "lcd_lib_4bit.c"

#define sw_mode PIN_A3
#define sw_up PIN_A2
#define sw_down PIN_A1
#define sw_exit PIN_A0

#define val_mode() !input(sw_mode)
#define val_up() !input(sw_up)
#define val_down() !input(sw_down)
#define val_exit() !input(sw_exit)

#define clock_reset PIN_A3 // TAO XUNG CHONG TREO

#define backligh_lcd PIN_A4
#define backligh_on() output_high(backligh_lcd);
#define backligh_off() output_low(backligh_lcd);

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
#define ree16(x) read_eeprom16(x);
#define wee(x, y) write_eeprom(x, y);
#define wee16(x, y) write_eeprom16(x, y);

#define phong_accu 1
#define tg_chay_lien_tuc 2
#define tg_tam_dung 3
#define do_ac 4
#define ac_bthg 5
#define chay_mpd 6
#define error 7

#define sec_to_sec(time) time % 60
#define sec_to_minute(time) time / 60
//===================
#define clear_lcd() lcd_printf(0);
#define yesno() lcd_printf(9);
#define loading() lcd_printf(10);

#define sch_1_s_set() lcd_printf(11);

// luu tru trong rom

#define counter_restart_mpd_ee 0x00
#define timer_chay_lien_tuc_ee counter_restart_mpd_ee + 1
#define timer_ktra_AC_ee timer_chay_lien_tuc_ee + 1
#define timer_ktra_mn_ee timer_ktra_AC_ee + 1
#define flag_error_ee timer_ktra_mn_ee + 1
#define delta_dc_ee flag_error_ee + 2
#define input_dc_lv2_ee = delta_dc_ee + 2

#define size_pass 5