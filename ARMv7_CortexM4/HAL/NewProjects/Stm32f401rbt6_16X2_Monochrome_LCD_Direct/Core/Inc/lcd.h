/******************************************************************************
 * @file    lcd.h
 * @brief   Header file for 16x2 LCD Driver
 *
 * Description:
 * ---------------------------------------------------
 * This file contains function declarations for
 * LCD driver source file (lcd.c)
 *
 * Include this file in main.c or any file where
 * LCD functions are required.
 *
 ******************************************************************************/

#ifndef LCD_H_
#define LCD_H_

/* Include STM32 main header file */
#include "main.h"

/*=============================================================================
                        LCD FUNCTION PROTOTYPES
=============================================================================*/

/**
 * @brief Initialize LCD in 4-bit mode
 */
void LCD_Init(void);

/**
 * @brief Send command to LCD
 * @param cmd Command byte
 */
void LCD_Command(uint8_t cmd);

/**
 * @brief Send single character data to LCD
 * @param data ASCII data
 */
void LCD_Data(uint8_t data);

/**
 * @brief Display string on LCD
 * @param str Pointer to string
 */
void LCD_String(char *str);

/**
 * @brief Set cursor position
 * @param row Row number (1 or 2)
 * @param col Column number (1 to 16)
 */
void LCD_SetCursor(uint8_t row, uint8_t col);

/**
 * @brief Clear LCD display
 */
void LCD_Clear(void);

#endif /* LCD_H_ */

/******************************************************************************
 * END OF FILE
 ******************************************************************************/
