/******************************************************************************
 * @file    lcd.c
 * @brief   16x2 LCD Driver Source File (4-Bit Mode)
 *
 * MCU      : STM32
 * LCD Type : HD44780 Compatible 16x2 Character LCD
 *
 * Connection:
 * -----------------------------------
 * LCD D4  -> PB0
 * LCD D5  -> PB1
 * LCD D6  -> PB2
 * LCD D7  -> PB3
 * LCD RS  -> PB4
 * LCD RW  -> PB5
 * LCD EN  -> PB8
 *
 * Description:
 * -----------------------------------
 * This file contains all LCD driver functions:
 * 1. LCD Initialization
 * 2. Send Command
 * 3. Send Data
 * 4. Print String
 * 5. Set Cursor Position
 * 6. Clear Display
 *
 ******************************************************************************/

#include "lcd.h"

/*=============================================================================
                        LCD PIN DEFINITIONS
=============================================================================*/

/* Data Pins */
#define LCD_D4_PIN GPIO_PIN_0
#define LCD_D5_PIN GPIO_PIN_1
#define LCD_D6_PIN GPIO_PIN_2
#define LCD_D7_PIN GPIO_PIN_3

/* Control Pins */
#define LCD_RS_PIN GPIO_PIN_4
#define LCD_RW_PIN GPIO_PIN_5
#define LCD_EN_PIN GPIO_PIN_8

/* LCD Connected Port */
#define LCD_PORT GPIOB


/*=============================================================================
                    PRIVATE FUNCTION PROTOTYPES
=============================================================================*/

/* Generate Enable Pulse */
static void LCD_EnablePulse(void);

/* Send 4-bit nibble to LCD */
static void LCD_SendNibble(uint8_t nibble);


/*=============================================================================
                    FUNCTION : LCD_EnablePulse()
==============================================================================
 * @brief  Generates HIGH to LOW pulse on EN pin.
 *         LCD reads data during this pulse.
 *
 * Steps:
 * 1. EN = HIGH
 * 2. Small delay
 * 3. EN = LOW
 * 4. Small delay
 *===========================================================================*/
static void LCD_EnablePulse(void)
{
    HAL_GPIO_WritePin(LCD_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    HAL_Delay(1);

    HAL_GPIO_WritePin(LCD_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
    HAL_Delay(1);
}


/*=============================================================================
                    FUNCTION : LCD_SendNibble()
==============================================================================
 * @brief  Sends 4-bit data to LCD data pins D4-D7
 *
 * @param  nibble : lower 4 bits data
 *
 * Example:
 * nibble = 0x05 = 0101
 * D4=1 D5=0 D6=1 D7=0
 *===========================================================================*/
static void LCD_SendNibble(uint8_t nibble)
{
    /* Send bit0 to D4 */
    HAL_GPIO_WritePin(LCD_PORT, LCD_D4_PIN,
        (nibble & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* Send bit1 to D5 */
    HAL_GPIO_WritePin(LCD_PORT, LCD_D5_PIN,
        (nibble & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* Send bit2 to D6 */
    HAL_GPIO_WritePin(LCD_PORT, LCD_D6_PIN,
        (nibble & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* Send bit3 to D7 */
    HAL_GPIO_WritePin(LCD_PORT, LCD_D7_PIN,
        (nibble & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* Tell LCD to read data */
    LCD_EnablePulse();
}


/*=============================================================================
                    FUNCTION : LCD_Command()
==============================================================================
 * @brief  Sends command to LCD
 *
 * @param  cmd : command byte
 *
 * Process:
 * RS = 0 -> Command Register
 * RW = 0 -> Write Mode
 *
 * Since LCD is in 4-bit mode:
 * 1. Send upper nibble
 * 2. Send lower nibble
 *===========================================================================*/
void LCD_Command(uint8_t cmd)
{
    /* Select Command Register */
    HAL_GPIO_WritePin(LCD_PORT, LCD_RS_PIN, GPIO_PIN_RESET);

    /* Write Mode */
    HAL_GPIO_WritePin(LCD_PORT, LCD_RW_PIN, GPIO_PIN_RESET);

    /* Send upper 4 bits */
    LCD_SendNibble(cmd >> 4);

    /* Send lower 4 bits */
    LCD_SendNibble(cmd & 0x0F);

    HAL_Delay(2);
}


/*=============================================================================
                    FUNCTION : LCD_Data()
==============================================================================
 * @brief  Sends character data to LCD
 *
 * @param  data : ASCII character
 *
 * Process:
 * RS = 1 -> Data Register
 * RW = 0 -> Write Mode
 *===========================================================================*/
void LCD_Data(uint8_t data)
{
    /* Select Data Register */
    HAL_GPIO_WritePin(LCD_PORT, LCD_RS_PIN, GPIO_PIN_SET);

    /* Write Mode */
    HAL_GPIO_WritePin(LCD_PORT, LCD_RW_PIN, GPIO_PIN_RESET);

    /* Send upper nibble */
    LCD_SendNibble(data >> 4);

    /* Send lower nibble */
    LCD_SendNibble(data & 0x0F);

    HAL_Delay(2);
}


/*=============================================================================
                    FUNCTION : LCD_Init()
==============================================================================
 * @brief  Initializes LCD in 4-bit mode
 *
 * Commands Used:
 * 0x02 -> Return Home / 4-bit mode
 * 0x28 -> 4-bit, 2-line, 5x7 font
 * 0x0C -> Display ON, Cursor OFF
 * 0x06 -> Cursor move right
 * 0x01 -> Clear Display
 *===========================================================================*/
void LCD_Init(void)
{
    HAL_Delay(50);   /* Wait LCD Power ON */

    LCD_Command(0x02);
    LCD_Command(0x28);
    LCD_Command(0x0C);
    LCD_Command(0x06);
    LCD_Command(0x01);

    HAL_Delay(5);
}


/*=============================================================================
                    FUNCTION : LCD_Clear()
==============================================================================
 * @brief Clears LCD screen
 *===========================================================================*/
void LCD_Clear(void)
{
    LCD_Command(0x01);
    HAL_Delay(2);
}


/*=============================================================================
                    FUNCTION : LCD_SetCursor()
==============================================================================
 * @brief  Sets cursor position
 *
 * @param row : 1 or 2
 * @param col : 1 to 16
 *
 * Row1 starts from 0x80
 * Row2 starts from 0xC0
 *===========================================================================*/
void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t pos;

    if(row == 1)
    {
        pos = 0x80 + (col - 1);
    }
    else
    {
        pos = 0xC0 + (col - 1);
    }

    LCD_Command(pos);
}


/*=============================================================================
                    FUNCTION : LCD_String()
==============================================================================
 * @brief  Displays string on LCD
 *
 * @param str : pointer to string
 *
 * Example:
 * LCD_String("HELLO");
 *===========================================================================*/
void LCD_String(char *str)
{
    while(*str)
    {
        LCD_Data(*str++);
    }
}

/******************************************************************************
 * END OF FILE
 ******************************************************************************/
