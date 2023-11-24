#ifndef EEPROM_H
#define EEPROM_H

#define counter_restart_mpd_ee 0x00
#define timer_chay_lien_tuc_ee counter_restart_mpd_ee + 1
#define timer_ktra_AC_ee timer_chay_lien_tuc_ee + 1
#define timer_ktra_mn_ee timer_ktra_AC_ee + 1
#define flag_error_ee timer_ktra_mn_ee + 1
#define delta_dc_ee flag_error_ee + 1
#define input_dc_lv2_ee delta_dc_ee + 2

#endif