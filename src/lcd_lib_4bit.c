// #include <stddef.h>
#include "C:\Program Files (x86)\PICC\Drivers\stddef.h"

#define LCD_RS PIN_B1
#define LCD_EN PIN_B0
#define LCD_D4 PIN_C7
#define LCD_D5 PIN_C6
#define LCD_D6 PIN_C5
#define LCD_D7 PIN_C4
#define Line_1 0x80
#define Line_2 0xC0
#define Clear_Scr 0x01

void LCD_Init();                       // ham khoi tao LCD
void LCD_SetPosition(unsigned int cX); // Thiet lap vi tri con tro
void LCD_PutChar(unsigned int cX);     // Ham viet1kitu/1chuoi len LCD
void LCD_PutCmd(unsigned int cX);      // Ham gui lenh len LCD
void LCD_PulseEnable(void);            // Xung kich hoat
void LCD_SetData(unsigned int cX);     // Dat du lieu len chan Data

// khoi tao LCD**********************************************
void LCD_Init()
{
    LCD_SetData(0x00);
    delay_ms(20);       /*Cho cho lcd khoi tao */
    output_low(LCD_RS); // che do gui lenh
    LCD_SetData(0x03);  /* khoi tao che do 4 bit */
    LCD_PulseEnable();
    LCD_PulseEnable();
    LCD_PulseEnable();
    LCD_SetData(0x02); /* tao giao dien 4 bit */
    LCD_PulseEnable(); /* send dual nibbles hereafter, MSN first */
    LCD_PutCmd(0x2C);  /* function set (all lines, 5x7 characters) */
    LCD_PutCmd(0x0C);  /* display ON, cursor off, no blink */
    LCD_PutCmd(0x06);  /* entry mode set, increment & scroll left */
    LCD_PutCmd(0x01);  /* clear display */

    // Init for BarGraph
}

void LCD_SetPosition(unsigned int cX)
{
    /* this subroutine works specifically for 4-bit Port A */
    LCD_SetData(swap(cX) | 0x08);
    LCD_PulseEnable();
    LCD_SetData(swap(cX));
    LCD_PulseEnable();
}

void LCD_PutChar(unsigned int cX)
{
    /* this subroutine works specifically for 4-bit Port A */
    output_high(LCD_RS);
    LCD_PutCmd(cX);
    output_low(LCD_RS);
}

void LCD_PutCmd(unsigned int cX)
{
    /* this subroutine works specifically for 4-bit Port A */

    LCD_SetData(swap(cX)); /* send high nibble */
    LCD_PulseEnable();
    LCD_SetData(swap(cX)); /* send low nibble */
    LCD_PulseEnable();
}

void LCD_PulseEnable(void)
{
    output_high(LCD_EN);
    delay_us(3); // was 10
    output_low(LCD_EN);
    delay_ms(3); // was 5
}

void LCD_SetData(unsigned int cX)
{
    output_bit(LCD_D4, cX & 0x01);
    output_bit(LCD_D5, cX & 0x02);
    output_bit(LCD_D6, cX & 0x04);
    output_bit(LCD_D7, cX & 0x08);
}