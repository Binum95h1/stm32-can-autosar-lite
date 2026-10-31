#include "LcdIf.h"
#include "I2c.h"
#include "OsIf.h"
#include "Reg.h"



static void lcd_send_cmd(char cmd)
{
    uint8_t data_t[4];
    uint8_t data_u = (uint8_t)(cmd & 0xF0U);
    uint8_t data_l = (uint8_t)(((uint8_t)cmd << 4U) & 0xF0U);

    data_t[0] = data_u | 0x0CU;   /* EN=1, RS=0, BL=1, RW=0 */
    data_t[1] = data_u | 0x08U;   /* EN=0, RS=0, BL=1, RW=0 */
    data_t[2] = data_l | 0x0CU;   /* EN=1, RS=0, BL=1, RW=0 */
    data_t[3] = data_l | 0x08U;   /* EN=0, RS=0, BL=1, RW=0 */

    (void)I2c_MasterTransmit(LCD_I2C_7BIT, data_t, 4U);
}



static void lcd_send_data(char data)
{
    uint8_t data_t[4];
    uint8_t data_u = (uint8_t)(data & 0xF0U);
    uint8_t data_l = (uint8_t)(((uint8_t)data << 4U) & 0xF0U);

    data_t[0] = data_u | 0x0DU;   /* EN=1, RS=1, BL=1, RW=0 */
    data_t[1] = data_u | 0x09U;   /* EN=0, RS=1, BL=1, RW=0 */
    data_t[2] = data_l | 0x0DU;   /* EN=1, RS=1, BL=1, RW=0 */
    data_t[3] = data_l | 0x09U;   /* EN=0, RS=1, BL=1, RW=0 */

    (void)I2c_MasterTransmit(LCD_I2C_7BIT, data_t, 4U);
}


void LcdIf_Init(void)
{
    OsIf_DelayMs(50U);
    lcd_send_cmd(0x30); OsIf_DelayMs(5U);
    lcd_send_cmd(0x30); OsIf_DelayMs(1U);
    lcd_send_cmd(0x30); OsIf_DelayMs(10U);
    lcd_send_cmd(0x20); OsIf_DelayMs(10U);   /* Switch to 4-bit mode */
    lcd_send_cmd(0x28); OsIf_DelayMs(1U);    /* Function set: 4-bit, 2 line, 5×8 */
    lcd_send_cmd(0x08); OsIf_DelayMs(1U);    /* Display off */
    lcd_send_cmd(0x01); OsIf_DelayMs(2U);    /* Clear display */
    lcd_send_cmd(0x06); OsIf_DelayMs(1U);    /* Entry mode: inc, no shift */
    lcd_send_cmd(0x0C);                  /* Display ON, cursor OFF */
}



void LcdIf_SendString(const char *str)
{
    while (*str != '\0') { lcd_send_data(*str++); }
}



void LcdIf_SetCursor(int row, int col)
{
    switch (row)
    {
        case 0: col |= 0x80; break;
        case 1: col |= 0xC0; break;
        default: break;
    }
    lcd_send_cmd((char)col);
}



void LcdIf_Clear(void)
{
    lcd_send_cmd(0x01);
    OsIf_DelayMs(2U);
}
