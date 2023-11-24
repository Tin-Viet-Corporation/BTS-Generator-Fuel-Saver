#ifndef DISPLAY_H
#define DISPLAY_H

#include "picc.h"

#define phong_accu 1
#define tg_chay_lien_tuc 2
#define tg_tam_dung 3
#define do_ac 4
#define ac_bthg 5
#define chay_mpd 6
#define error 7

#define sec_to_sec(time) time % 60
#define sec_to_minute(time) time / 60

#define clear_lcd() lcd_printf(0);
#define yesno() lcd_printf(9);
#define loading() lcd_printf(10);

#define sch_1_s_set() lcd_printf(11);

#endif