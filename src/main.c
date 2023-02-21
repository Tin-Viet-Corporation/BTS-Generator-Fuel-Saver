#include "C:\Program Files (x86)\PICC\Devices\18f26k20.h"
#device adc = 10
#fuses hs, nolvp, protect, MCLR, NOPUT, BORV27
#include "C:\Program Files (x86)\PICC\Drivers\stdlib.h"
#fuses WDT16384
#use delay(clock = 24000000)
#include "lcd_lib_4bit.c"

//============================
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
#define wee(x, y) write_eeprom(x, y);

#define tg_tri_hoan 1
#define tg_chay_lien_tuc 2
#define tg_tam_dung 3
#define do_ac 4
#define ac_bthg 5
#define chay_mpd 6
#define error 7

//===================
#define clear_lcd() lcd_printf(0);
#define yesno() lcd_printf(9);
#define loading() lcd_printf(10);

#define sch_1_s_set() lcd_printf(11);

// luu tru trong rom

#define timer_tri_hoan_ee 0x00
#define counter_restart_mpd_ee timer_tri_hoan_ee + 1
#define timer_chay_lien_tuc_ee counter_restart_mpd_ee + 1
#define timer_tam_dung_ee timer_chay_lien_tuc_ee + 1
#define timer_ktra_AC_ee timer_tam_dung_ee + 1
#define timer_ktra_mn_ee timer_ktra_AC_ee + 1

#define size_pass 5

// khai bao bien
unsigned char val_number_defaul[11] = {"0123456789"};
// con tro phuc vu cai dat
char *sch_1 = 0, *sch_2 = 0, val_sch_1 = 0, val_sch_2 = 0;

char password[size_pass] = {"1111"};
char str_temp[25] = "";
char time_reset_password = 0;

char state_AC = 0;
char mode = 9;
char state_mn = 0;
short refresh_menu = 0;

char timer_exit = 0;
char timer_backlight = 240;

char val_timer_tri_hoan = 60, timer_tri_hoan = 60, timer_tri_hoan_md = 60;             // LONG DELAY 1
char val_counter_restart_mpd = 1, counter_restart_mpd = 4, counter_restart_mpd_md = 4; // LONG SO LAN KHOI DONG LAI MPD
char counter_restart_mpd_current = 4;
char val_timer_chay_lien_tuc = 3, timer_chay_lien_tuc = 3, timer_chay_lien_tuc_md = 3; // LONG DELAY 2
char val_timer_tam_dung = 60, timer_tam_dung = 60, timer_tam_dung_md = 60;             // LONG DELAY 3
char val_timer_ktra_AC = 60, timer_ktra_AC = 60, timer_ktra_AC_md = 60;                // LONG KT AC TIMER
char val_timer_ktra_mn = 30, timer_ktra_mn = 30, timer_ktra_mn_md = 30;                // LONG KT MN TIMER
char val_timer_on_mpd = 20, timer_on_mpd = 20;
char val_timer_off_mpd = 20, timer_off_mpd = 20;

char val_loading = 0;

char flag_error = 0; // LONG FLAG = 0 la ko loi, = 1 la LOI
char flag_mn = 0;    // LONG FLAG = 0 la chay xong check_mn(), = 1 la dang chay

unsigned long counter_timer0 = 0, flag_timer_chay_lien_tuc_3600s = 3600;
char flag_timer_tri_hoan_60s = 60, flag_timer_60s_password = 0, flag_timer_tam_dung_60s = 60;

char pwm_lcd = 0;
char sum_out = 0, sum_out_old = 0, loop_not_display = 0;

//===============================
void init_data(void);
void check_AC(void);
char check_mn(void);
void default_data(void);
void display(char code_print);
char key_scan(void);
void process_menu(void);
void process_up(void);
void process_down(void);
void process_exit(void);
void display_center(void);
void lcd_printf(char code_printf);
void reset_timer_data(void);
void write_data(void);
void read_data(void);

unsigned char read_eeprom(unsigned char addr);
void write_eeprom(unsigned char addr, unsigned char value);

void disable_reset(void);

//===============================
void main()
{
   // chong treo xu li
   disable_reset();
   // KHOI DONG NGAT TIMER 0
   SETUP_TIMER_0(T0_INTERNAL | T0_DIV_4);
   enable_interrupts(INT_TIMER0);
   enable_interrupts(GLOBAL);
   clear_interrupt(INT_TIMER0);
   set_timer0(62536); //(24000000/128)/(65535-65160)=187500/375=500us
   lcd_init();        // khoi dong mang hinh lcd
   backligh_on();
   read_data();
   init_data();
   // CHUONG TRINH CHINH
   while (1)
   {
      disable_reset();
      restart_wdt();
      // DIEU HOA TRANG THAI: Tinh nang chinh - nhap menu
      switch (mode)
      {
      case 0: // TINH NANG CHINH
         check_AC();

         switch (state_AC)
         {
         case 0: // Timer delay ktra ac
            break;

         case 1: // co AC ->hien thi LCD
            reset_timer_data();
            output_low(out_fire);
            output_low(out_delay);
            break;
         case 2: // mat AC: DELAY 1
            output_high(out_delay);
            if (val_timer_tri_hoan <= 0)
            {
               state_AC = 3;
            }
            break;
         case 3: // mat AC: DO DIEN AP MPD
            if (val_counter_restart_mpd <= counter_restart_mpd_current)
            {
               display(chay_mpd);
               switch (state_mn)
               {
               case 0: // on remote start delay
                  output_high(out_fire);
                  if (val_timer_on_mpd <= 0)
                  {
                     state_mn = 1;
                     val_timer_on_mpd = timer_on_mpd;
                  }
                  break;
               case 1: // do mpd
                  char mn = check_mn();
                  if (mn == 1)
                  {
                     state_AC = 4;
                  }
                  else if (mn == 0)
                  {
                     state_mn = 2;
                  }
                  break;
               case 2: // off remote start delay
                  output_low(out_fire);
                  if (val_timer_off_mpd <= 0)
                  {
                     state_mn = 0;
                     val_timer_off_mpd = timer_off_mpd;
                     val_counter_restart_mpd++;
                  }
                  break;
               }
            }
            else if (val_counter_restart_mpd > counter_restart_mpd_current)
            {
               state_AC = 10; // error
            }
            break;
         case 4: // DELAY 2 + OFF REMOTE START
            char mn = check_mn();
            if (mn == 0)
            {
               state_AC = 10; // error
               break;
            }
            // OFF REMOTE START
            if (val_timer_chay_lien_tuc <= 0)
            {
               output_low(out_fire);
               flag_error = 0;
               output_low(out_temp);
               output_low(out_error);
               state_AC = 5;
            }
            break;
         case 5: // DELAY 3
            if (val_timer_tam_dung <= 0)
            {
               state_AC = 3;
               reset_timer_data();
            }
            break;
         case 10: // error
            flag_error = 1;
            output_low(out_fire);
            output_high(out_temp);
            output_high(out_error);
            output_low(out_delay);
            break;
         }
         break;
      case 1: // nhap mat khau
         break;
      case 2: // MENU TG TRI HOAN MPD
         break;
      case 3: // MENU TG CHAY LIEN TUC
         break;
      case 4: // MENU TG TAM DUNG MPD
         break;
      case 5: // MENU TG KTRA DA AC
         break;
      case 6: // MENU TG KTRA DA MN
         break;
      case 7: // MENU SO LAN KHOI DONG
         break;
      case 8: // MENU cai dat mat dinh
         break;
      case 9: // Loading screen
         // loading xong chay tinh nang chinh
         break;
      }

      // kiem tra phim nhan
      output_float(sw_down);
      switch (key_scan())
      {
      case 0x01: // nut menu duoc nhan
         lcd_init();
         timer_exit = 30;
         timer_backlight = 250;
         process_menu();
         delay_ms(200);
         break;

      case 0x02: // nut up duoc nhan
         timer_exit = 90;
         timer_backlight = 250;
         process_up();
         if (mode >= 2 && mode <= 7)
            write_data();
         delay_ms(200);
         break;

      case 0x03: // nut down duoc nhan
         timer_exit = 90;
         timer_backlight = 250;
         process_down();
         if (mode >= 2 && mode <= 7)
            write_data();
         delay_ms(200);
         break;

      case 0x04: // nut exit duoc nhan
         timer_exit = 90;
         timer_backlight = 250;
         process_exit();
         delay_ms(200);
         break;
      }

      // chuong trinh hien thi
      sum_out = 0;

      if (sum_out > sum_out_old)
      {
         loop_not_display = 3;
         timer_backlight = 250;
      }
      sum_out_old = sum_out;

      disable_reset();

      switch (loop_not_display)
      {
      case 1:
         lcd_init();
         loop_not_display--;
         if (mode != 0)
            refresh_menu = 1;
         break;
      case 0:
         display_center();
         break;
      }

      disable_reset();
   }
}

//=========================
void init_data(void)
{
   // con tro phuc vu cai dat
   *sch_1 = 0, *sch_2 = 0, val_sch_1 = 0, val_sch_2 = 0;
   str_temp = "";
   time_reset_password = 0;

   state_AC = 0;
   mode = 9;
   state_mn = 0;
   refresh_menu = 0;

   timer_exit = 0;
   timer_backlight = 240;

   val_timer_tri_hoan = timer_tri_hoan, timer_tri_hoan_md = 60;                                                // LONG DELAY 1
   val_counter_restart_mpd = 1, counter_restart_mpd_current = counter_restart_mpd, counter_restart_mpd_md = 4; // LONG SO LAN KHOI DONG LAI MPD
   val_timer_chay_lien_tuc = timer_chay_lien_tuc, timer_chay_lien_tuc_md = 3;                                  // LONG DELAY 2
   val_timer_tam_dung = timer_tam_dung, timer_tam_dung_md = 60;                                                // LONG DELAY 3
   val_timer_ktra_AC = timer_ktra_AC, timer_ktra_AC_md = 60;                                                   // LONG KT AC TIMER
   flag_error = 0;                                                                                             // LONG FLAG = 0 la ko loi, = 1 la LOI
   flag_mn = 0;
   val_timer_ktra_mn = timer_ktra_mn, timer_ktra_mn_md = 30; // LONG KT MN TIMER
   val_timer_on_mpd = 20, timer_on_mpd = 20;
   val_timer_off_mpd = 20, timer_off_mpd = 20;

   val_loading = 0;

   counter_timer0 = 0, flag_timer_60s_password = 0;
   flag_timer_tri_hoan_60s = 0, flag_timer_chay_lien_tuc_3600s = 0, flag_timer_tam_dung_60s = 0;

   pwm_lcd = 0;
   sum_out = 0, sum_out_old = 0, loop_not_display = 0;
}
char check_mn(void)
{
   flag_mn = 1;
   if (status_mn())
   {
      flag_mn = 0;
      val_timer_ktra_mn = timer_ktra_mn;
      return 1; // do dc d.a mn
   }
   if (val_timer_ktra_mn <= 0)
   {
      flag_mn = 0;
      val_timer_ktra_mn = timer_ktra_mn;
      return status_mn(); // ket thuc delay do mn va return ket qua do
   }
   return 2; // dang chay delay do mn
}
void check_AC(void)
{
   if (state_AC <= 1 && (!(status_AC())))
   {
      state_AC = 0; // chay trang thai dem do AC
      if (val_timer_ktra_AC <= 0)
      {
         if (!(status_AC()))
         {
            state_AC = 2;
         }
         val_timer_ktra_AC = timer_ktra_AC;
      }
   }
   else if (state_AC != 1 && (status_AC()))
   {
      state_AC = 0; // chay trang thai dem do AC
      if (val_timer_ktra_AC <= 0)
      {
         if (status_AC())
         {
            state_AC = 1;
         }
         val_timer_ktra_AC = timer_ktra_AC;
      }
   }
}

void display(char code_print)
{
   switch (code_print)
   {
   case 0: // ko in
      break;
   case 1: // delay 1
      LCD_PUTCMD(Line_1);
      PRINTF(LCD_PUTCHAR, "TG TRI HOAN MPD");
      clear_lcd();
      LCD_PUTCMD(Line_2);
      PRINTF(LCD_PUTCHAR, "PHUT Long: %01u", val_timer_tri_hoan);
      clear_lcd();
      break;
   case 2: // delay 2
      LCD_PUTCMD(Line_1);
      PRINTF(LCD_PUTCHAR, "TG CHAY LIEN TUC");
      clear_lcd();
      LCD_PUTCMD(Line_2);
      PRINTF(LCD_PUTCHAR, "GIO: %01u", val_timer_chay_lien_tuc);
      clear_lcd();
      break;
   case 3: // delay 3
      LCD_PUTCMD(Line_1);
      PRINTF(LCD_PUTCHAR, "TG TAM DUNG MPD");
      clear_lcd();
      LCD_PUTCMD(Line_2);
      PRINTF(LCD_PUTCHAR, "PHUT: %01u", val_timer_tam_dung);
      clear_lcd();
      break;
   case 4: // Do AC
      LCD_PUTCMD(Line_1);
      PRINTF(LCD_PUTCHAR, "KIEM TRA AC");
      clear_lcd()
          LCD_PUTCMD(Line_2);
      PRINTF(LCD_PUTCHAR, "GIAY: %01u", val_timer_ktra_AC);
      clear_lcd();
      break;
   case 5: // AC BINH THUONG
      LCD_PUTCMD(Line_1);
      PRINTF(LCD_PUTCHAR, "D.AP AC BTHUONG");
      clear_lcd();
      if (flag_error)
      {
         LCD_PUTCMD(Line_2);
         clear_lcd();
         LCD_PUTCMD(Line_2);
         PRINTF(LCD_PUTCHAR, "MPD LOI");
         clear_lcd();
      }
      else
      {
         LCD_PUTCMD(Line_2);
         PRINTF(LCD_PUTCHAR, "MPD TOT");
         clear_lcd();
      }
      break;
   case 6: // CHAY MPD
      LCD_PUTCMD(Line_1);
      PRINTF(LCD_PUTCHAR, "CHAY MPD");
      clear_lcd();
      LCD_PUTCMD(Line_2);
      PRINTF(LCD_PUTCHAR, "LAN: %01u", val_counter_restart_mpd);
      clear_lcd();
      break;
   case 7: // error
      LCD_PUTCMD(Line_1);
      PRINTF(LCD_PUTCHAR, "MPD LOI");
      clear_lcd();
      LCD_PUTCMD(Line_2);
      clear_lcd();
      break;
   }
}

void default_data(void)
{
   timer_tri_hoan = timer_tri_hoan_md;           // LONG DELAY 1
   counter_restart_mpd = counter_restart_mpd_md; // LONG SO LAN KHOI DONG LAI MPD
   timer_chay_lien_tuc = timer_chay_lien_tuc_md; // LONG DELAY 2
   timer_tam_dung = timer_tam_dung_md;           // LONG DELAY 3
   timer_ktra_AC = timer_ktra_AC_md;             // LONG KT AC TIMER
   timer_ktra_mn = timer_ktra_mn_md;             // LONG KT MN TIMER
}
void display_center(void)
{

   unsigned char menu_main[10][17] = {{""},
                                      {"MAT KHAU !"},
                                      {"TG TRI HOAN MPD"},
                                      {"TG CHAY LIEN TUC"},
                                      {"TG TAM DUNG MPD"},
                                      {"TG KTRA D.AP AC"},
                                      {"TG KTRA D.AP MPD"},
                                      {"SO LAN KHOI DONG"},
                                      {"CAI DAT MAC DINH"},
                                      {""}};

   if (refresh_menu)
   {
      LCD_PUTCMD(Line_1); // Dua hien thi chu dau dong hang` duoi
      PRINTF(LCD_PUTCHAR, "%s", menu_main[mode]);
      clear_lcd();
   }
   LCD_PUTCMD(Line_2); // Dua hien thi chu dau dong hang` duoi

   switch (mode)
   {
   case 0: // LCD TINH NANG CHINH
      switch (state_AC)
      {
      case 0:
         display(do_ac);
         break;
      case 1:
         display(ac_bthg);
         break;
      case 2:
         display(tg_tri_hoan);
         break;
      case 3:
         break;
      case 4:
         display(tg_chay_lien_tuc);
         break;
      case 5:
         display(tg_tam_dung);
         break;
      case 10:
         display(error);
         break;
      }
      break;
   case 1:                // nhap mat khau
      LCD_PUTCMD(Line_2); // Dua hien thi chu dau dong hang` duoi
      sch_1 = str_temp;
      sch_1_s_set();
      break;

   case 2:                // TG TRI HOAN MPD
      LCD_PUTCMD(Line_2); // Dua hien thi chu dau dong hang` duoi
      PRINTF(LCD_PUTCHAR, "PHUT: <%01u>", timer_tri_hoan);
      clear_lcd();
      break;

   case 3:                // TG CHAY LIEN TUC
      LCD_PUTCMD(Line_2); // Dua hien thi chu dau dong hang` duoi
      PRINTF(LCD_PUTCHAR, "GIO: <%01u>", timer_chay_lien_tuc);
      clear_lcd();
      break;

   case 4:                // TG TAM DUNG MPD
      LCD_PUTCMD(Line_2); // Dua hien thi chu dau dong hang` duoi
      PRINTF(LCD_PUTCHAR, "PHUT: <%01u>", timer_tam_dung);
      clear_lcd();
      break;
   case 5:                // TG KTRA DA AC
      LCD_PUTCMD(Line_2); // Dua hien thi chu dau dong hang` duoi
      PRINTF(LCD_PUTCHAR, "GIAY: <%01u>", timer_ktra_AC);
      clear_lcd();
      break;
   case 6:                // TG KTRA DA MN
      LCD_PUTCMD(Line_2); // Dua hien thi chu dau dong hang` duoi
      PRINTF(LCD_PUTCHAR, "GIAY: <%01u>", timer_ktra_mn);
      clear_lcd();
      break;
   case 7:                // SO LAN KHOI DONG
      LCD_PUTCMD(Line_2); // Dua hien thi chu dau dong hang` duoi
      PRINTF(LCD_PUTCHAR, "LAN: <%01u>", counter_restart_mpd);
      clear_lcd();
      break;
   case 8: // cai dat mat dinh
      yesno();
      break;
   case 9:
      LCD_PUTCMD(Line_1);
      loading();
      clear_lcd();
      LCD_PUTCMD(Line_2);
      clear_lcd();
      break;
   }
   refresh_menu = 0; // cap nhat du lieu mang hinh xong
}

//=========================
void reset_timer_data(void)
{
   val_timer_tri_hoan = timer_tri_hoan;
   val_counter_restart_mpd = 1;
   counter_restart_mpd_current = counter_restart_mpd;
   val_timer_chay_lien_tuc = timer_chay_lien_tuc;
   val_timer_tam_dung = timer_tam_dung;
   val_timer_on_mpd = timer_on_mpd;
   val_timer_off_mpd = timer_off_mpd;
   val_timer_ktra_mn = timer_ktra_mn;
   state_mn = 0;
   flag_timer_tri_hoan_60s = 60, flag_timer_chay_lien_tuc_3600s = 3600, flag_timer_tam_dung_60s = 60;
}

void lcd_printf(char code_printf)
{
   switch (code_printf)
   {
   case 0:
      PRINTF(LCD_PUTCHAR, "                 ");
      break;

   case 9:
      PRINTF(LCD_PUTCHAR, "  <YES || NO>");
      break;

   case 10:

      if (++val_loading > 5)
      {
         val_loading = 0;
         mode = 0;
      }
      PRINTF(LCD_PUTCHAR, "Loading ");
      switch (val_loading)
      {
      case 0:
         PRINTF(LCD_PUTCHAR, ".  ");
         break;

      case 1:
         PRINTF(LCD_PUTCHAR, ".. ");
         break;

      case 2:
         PRINTF(LCD_PUTCHAR, "...");
         break;

      case 3:
         PRINTF(LCD_PUTCHAR, " ..");
         break;

      case 4:
         PRINTF(LCD_PUTCHAR, "  .");
         break;

      case 5:
         PRINTF(LCD_PUTCHAR, "   ");
         break;
      }
      break;

   case 11:
      PRINTF(LCD_PUTCHAR, "%s<", sch_1);
      break;
   }
}
//=======================================
char key_scan(void)
{
   unsigned long bounce = 200;

   if (val_mode() == 1)
   {
      while (bounce--)
      {
         if (val_mode() == 0)
            return 0x00;
      }
      bounce = 1024;
      while (bounce--)
      {
         if (val_mode() == 0)
            return 0x01;
      }
      return 0x01;
   }

   else if (val_up() == 1)
   {
      while (bounce--)
      {
         if (val_up() == 0)
            return 0x00;
      }
      bounce = 256;
      while (bounce--)
      {
         if (val_up() == 0)
            return 0x02;
      }
      return 0x02;
   }

   else if (val_down() == 1)
   {
      while (bounce--)
      {
         if (val_down() == 0)
            return 0x00;
      }
      bounce = 256;
      while (bounce--)
      {
         if (val_down() == 0)
            return 0x03;
      }
      return 0x03;
   }

   else if (val_exit() == 1)
   {
      while (bounce--)
      {
         if (val_exit() == 0)
            return 0x00;
      }
      while (val_exit() == 0)
         ;
      return 0x04;
   }
   return 0x00;
}

//=========================
void process_menu(void)
{
   refresh_menu = 1;
   loop_not_display = 1;
   switch (mode)
   {
   case 1:
      if (strlen(str_temp) == strlen(password))
      {
         if (strcmp(password, str_temp) == 0)
         {
            mode++;
            time_reset_password = 20; // 4   phut
         }
      }
      val_sch_1 = 0;
      sch_1 = str_temp;
      *sch_1 = 0;
      break;

   default:
      if (++mode > 8)
         mode = 9; // ve loading screen
      if (time_reset_password != 0 && mode == 1)
         mode = 2;
      break;
   }
}

//=========================
void process_up(void)
{
   switch (mode)
   {
   case 0: // mang hinh chinh
      break;

   case 1: // nhap mat khau
      sch_2 = val_number_defaul;
      if (++val_sch_2 > 10)
         val_sch_2 = 0;
      sch_2 = sch_2 + val_sch_2;
      sch_1 = sch_1 + val_sch_1;
      *sch_1++ = *sch_2;
      *sch_1 = 0;
      break;

   case 2: // TG TRI HOAN
      if (++timer_tri_hoan > 254)
      {
         timer_tri_hoan = 1;
      }
      break;
   case 3: // TG CHAY LIEN TUC
      if (++timer_chay_lien_tuc > 24)
      {
         timer_chay_lien_tuc = 1;
      }
      break;

   case 4: // TG TAM DUNG MPD
      if (++timer_tam_dung > 254)
      {
         timer_tam_dung = 1;
      }
      break;

   case 5: // TG KTRA DA AC
      if (++timer_ktra_AC > 240)
      {
         timer_ktra_AC = 10;
      }
      break;

   case 6: // TG KTRA DA MN
      if (++timer_ktra_mn > 240)
      {
         timer_ktra_mn = 10;
      }
      break;

   case 7: // SO LAN KHOI DONG
      if (++counter_restart_mpd > 4)
      {
         counter_restart_mpd = 2;
      }
      break;
   case 8:      // cai dat mat dinh
      mode = 9; // ve loading screen
      default_data();
      write_data();
      reset_cpu();
      break;
   }
}

//=========================
void process_down(void)
{
   switch (mode)
   {
   case 0: // mang hinh chinh
      break;

   case 1: // nhap mat khau
      sch_2 = val_number_defaul;
      if (--val_sch_2 > 10)
         val_sch_2 = 10;
      sch_2 = sch_2 + val_sch_2;
      sch_1 = sch_1 + val_sch_1;
      *sch_1++ = *sch_2;
      *sch_1 = 0;
      break;

   case 2: // TG TRI HOAN
      if (--timer_tri_hoan < 1)
      {
         timer_tri_hoan = 254;
      }
      break;

   case 3: // TG CHAY LIEN TUC
      if (--timer_chay_lien_tuc < 1)
      {
         timer_chay_lien_tuc = 24;
      }
      break;

   case 4: // TG TAM DUNG MPD
      if (--timer_tam_dung < 1)
      {
         timer_tam_dung = 254;
      }
      break;

   case 5: // TG KTRA DA AC
      if (--timer_ktra_AC < 10)
      {
         timer_ktra_AC = 240;
      }
      break;

   case 6: // TG KTRA DA MN
      if (--timer_ktra_mn < 10)
      {
         timer_ktra_mn = 240;
      }
      break;

   case 7: // SO LAN KHOI DONG
      if (--counter_restart_mpd < 2)
      {
         counter_restart_mpd = 4;
      }
      break;
   case 8:      // cai dat mat dinh
      mode = 9; // ve loading screen
      break;
   }
}

//=========================
void process_exit(void)
{

   char val_null;
   val_sch_2 = 0;
   loop_not_display = 1;
   switch (mode)
   {
   case 0: // mang hinh chinh
      time_reset_password = 0;
      break;

   case 1: // nhap mat khau
      sch_1 = sch_1 + val_sch_1;
      val_null = *sch_1;
      if (val_null != 0 && val_null != 0XFF)
      {
         if (++val_sch_1 > 16)
            val_sch_1 = 0;
         sch_1++;
         *sch_1++ = 0XFF;
         *sch_1 = 0;
      }
      break;
   }
}

void write_data(void)
{
   wee(timer_tri_hoan_ee, timer_tri_hoan);
   wee(timer_chay_lien_tuc_ee, timer_chay_lien_tuc);
   wee(timer_tam_dung_ee, timer_tam_dung);
   wee(timer_ktra_mn_ee, timer_ktra_mn);
   wee(timer_ktra_AC_ee, timer_ktra_AC);
   wee(counter_restart_mpd_ee, counter_restart_mpd);
}

//=========================
void read_data(void)
{
   timer_tri_hoan = ree(timer_tri_hoan_ee);
   timer_chay_lien_tuc = ree(timer_chay_lien_tuc_ee);
   timer_tam_dung = ree(timer_tam_dung_ee);
   timer_ktra_mn = ree(timer_ktra_mn_ee);
   timer_ktra_AC = ree(timer_ktra_AC_ee);
   counter_restart_mpd = ree(counter_restart_mpd_ee);
}

void disable_reset(void)
{
   output_high(clock_reset);
   delay_ms(1);
   output_low(clock_reset);
   delay_ms(1);
   output_high(clock_reset);
   delay_ms(1);
   output_low(clock_reset);
   delay_ms(1);
   output_high(clock_reset);
   delay_ms(1);
   output_low(clock_reset);
   delay_ms(1);
   output_float(clock_reset);
}

#int_timer0
void interrupt_timer0()
{
   clear_interrupt(INT_TIMER0);
   set_timer0(62536); // 500us => 65536 - (0.0005/(4/24000000))

   if (mode == 0 && state_AC == 1) // DIMMER LCD
   {
      switch (pwm_lcd)
      {
      case 0:
         backligh_on();
         break;

      case 1:
         if (timer_backlight < 30)
            backligh_off();
         break;

      case 7:
         if (timer_backlight < 120)
            backligh_off();
         break;
      }
   }
   else
   {
      backligh_on();
   }
   if (++pwm_lcd > 10)
      pwm_lcd = 0;

   if (++counter_timer0 > 500)
   { // timer 1s
      counter_timer0 = 0;
      DISABLE_INTERRUPTS(INT_TIMER0);

      switch (mode)
      {
      case 0: // TINH NANG CHINH
         if (flag_mn == 1 && val_timer_ktra_mn > 0)
         {
            val_timer_ktra_mn--;
         }
         switch (state_AC)
         {
         case 0: // TIMER DEM DO AC
            if (val_timer_ktra_AC > 0)
               val_timer_ktra_AC--;
            break;
         case 1:
            break;
         case 2: // TIMER TRI HOAN
            if (--flag_timer_tri_hoan_60s > 59)
            {
               flag_timer_tri_hoan_60s = 59;
               if (val_timer_tri_hoan > 0)
                  val_timer_tri_hoan--;
            }
            break;
         case 3:
            switch (state_mn)
            {
            case 0:
               if (val_timer_on_mpd > 0)
               {
                  val_timer_on_mpd--;
               }
               break;
            case 1:
               break;
            case 2:
               if (val_timer_off_mpd > 0)
               {
                  val_timer_off_mpd--;
               }
               break;
            }
            break;
         case 4:
            if (--flag_timer_chay_lien_tuc_3600s > 3599)
            {
               flag_timer_chay_lien_tuc_3600s = 3599;
               if (val_timer_chay_lien_tuc > 0)
                  val_timer_chay_lien_tuc--;
            }
            break;
         case 5:
            if (--flag_timer_tam_dung_60s > 59)
            {
               flag_timer_tam_dung_60s = 59;
               if (val_timer_tam_dung > 0)
                  val_timer_tam_dung--;
            }
            break;
         }
         break;
      }
      // TRO VE MANG HINH CHINH KHI KHONG NHAN NUT
      if (timer_exit != 0)
         timer_exit--;
      if (timer_exit == 0)
      {
         mode = 0; // ve loading screen
      }

      // DEN LCD
      if (timer_backlight != 0)
         timer_backlight--;

      // timer 1 phut
      if (--flag_timer_60s_password > 59)
      {
         flag_timer_60s_password = 59;
         loop_not_display = 1;
         if (time_reset_password != 0)
            time_reset_password--;

         // nhan nut up mot luc reset ve mat dinh
      }
   }
   enable_interrupts(INT_TIMER0);
}