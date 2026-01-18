/**
 * @author: Trong Phan Minh
 * @date: 19/01/2026
 * @reference: https://cdn.sparkfun.com/assets/9/5/f/7/b/HD44780.pdf
 * @reference: https://file.thegioiic.com/upload/documents/1740390659_PCF8574AT-3,518.pdf
 * @brief: This is a basic driver for an LCD1602 using PCF8574 I2C module to communicate with STM32, you can replace the communicating functions if needed. Just search for Hardware Abstract Layer functions, status, etc. of STM32 and replace it with your communicating functions.
 */


#ifndef LCD_I2C_H
#define LCD_I2C_H

// Additional library includes
#include "main.h"

// Macros
//// Device address
#define PCF8574_ADDRESS 0x4E // The address of PCF8574, ranges from 0x40 to 0x4E, the LSB is R/~W bit

/*
 * Pin mask of the 8-bit value sent to the LCD
 * | Bit | Pin | Signal | Description        |
 * |-----|-----|--------|--------------------|
 * | 0   | P0  | RS     | Register Select    |
 * | 1   | P1  | R/W    | Read / Write       |
 * | 2   | P2  | EN     | Enable             |
 * | 3   | P3  | BL     | Backlight control  |
 * | 4   | P4  | DB4,DB0| Data Bit 4         |
 * | 5   | P5  | DB5,DB1| Data Bit 5         |
 * | 6   | P6  | DB6,DB2| Data Bit 6         |
 * | 7   | P7  | DB7,DB3| Data Bit 7         |
*/

#define RS_INDEX_PIN 0
#define RW_INDEX_PIN 1
#define EN_INDEX_PIN 2
#define BL_INDEX_PIN 3
#define DB4_INDEX_PIN 4
#define DB5_INDEX_PIN 5
#define DB6_INDEX_PIN 6
#define DB7_INDEX_PIN 7
#define DB0_INDEX_PIN DB4_INDEX_PIN
#define DB1_INDEX_PIN DB5_INDEX_PIN
#define DB2_INDEX_PIN DB6_INDEX_PIN
#define DB3_INDEX_PIN DB7_INDEX_PIN

#define PIN_RS (1 << RS_INDEX_PIN)
#define PIN_RW (1 << RW_INDEX_PIN)
#define PIN_EN (1 << EN_INDEX_PIN)
#define PIN_BL (1 << BL_INDEX_PIN)
#define PIN_DB4 (1 << DB4_INDEX_PIN)
#define PIN_DB5 (1 << DB5_INDEX_PIN)
#define PIN_DB6 (1 << DB6_INDEX_PIN)
#define PIN_DB7 (1 << DB7_INDEX_PIN)
#define PIN_DB0 (1 << DB0_INDEX_PIN)
#define PIN_DB1 (1 << DB1_INDEX_PIN)
#define PIN_DB2 (1 << DB2_INDEX_PIN)
#define PIN_DB3 (1 << DB3_INDEX_PIN)

/**
 * CMD Syntax
 * - [RS][R/~W][DB7][DB6][DB5][DB4][DB3][DB2][DB1][DB0]
 * - RS: Register Select
 * - R/~W: Read or write option
 * - DB[0..7]: Data bits
 */
#define RS_INDEX_MSK    9
#define RW_INDEX_MSK    8
#define DB7_INDEX_MSK   7
#define DB6_INDEX_MSK   6
#define DB5_INDEX_MSK   5
#define DB4_INDEX_MSK   4
#define DB3_INDEX_MSK   3
#define DB2_INDEX_MSK   2
#define DB1_INDEX_MSK   1
#define DB0_INDEX_MSK   0

#define MSK_RS  (1 << RS_INDEX_MSK)
#define MSK_RW  (1 << RW_INDEX_MSK)
#define MSK_DB7 (1 << DB7_INDEX_MSK)
#define MSK_DB6 (1 << DB6_INDEX_MSK)
#define MSK_DB5 (1 << DB5_INDEX_MSK)
#define MSK_DB4 (1 << DB4_INDEX_MSK)
#define MSK_DB3 (1 << DB3_INDEX_MSK)
#define MSK_DB2 (1 << DB2_INDEX_MSK)
#define MSK_DB1 (1 << DB1_INDEX_MSK)
#define MSK_DB0 (1 << DB0_INDEX_MSK)

// Status typedef
typedef HAL_StatusTypeDef LCD1602_I2C_Status_t;

// Global variables


// Global functions declaration
/**
 * @brief Initialize the LCD1602, 2 lines, 5x8 dots, 4-bit mode
 * @name LCD1602_I2C_Init
 * @param hi2c: Pointer to the I2C handle
 * @return Return the function status
 */
extern LCD1602_I2C_Status_t LCD1602_I2C_Init(I2C_HandleTypeDef* hi2c);

/**
 * @brief Clear the LCD1602 display
 * @name LCD1602_I2C_Clear
 * @return Return the function status
 */
extern LCD1602_I2C_Status_t LCD1602_I2C_Clear(void);

/**
 * @brief Move the cursor to specified position
 * @name LCD1602_I2C_MoveCursor
 * @param x: The column position (0-indexed, typically 0 to 39)
 * @param y: The row position (0-indexed, typically 0 or 1)
 * @return Return the function status
 */
extern LCD1602_I2C_Status_t LCD1602_I2C_MoveCursor(int x, int y);

/**
 * @brief Show a character on the LCD1602
 * @name LCD1602_I2C_ShowChar
 * @param c: The character to show
 * @return Return the function status
 */
extern LCD1602_I2C_Status_t LCD1602_I2C_ShowChar(char c);

/**
 * @brief Show a string on the LCD1602
 * @name LCD1602_I2C_ShowString
 * @param str: Pointer to the null-terminated string to show
 * @return Return the function status
 */
extern LCD1602_I2C_Status_t LCD1602_I2C_ShowString(char* str);

/**
 * @brief Shift the entire display left or right
 * @name LCD1602_I2C_ShiftDisplay
 * @param right: Set to 1 to shift right, 0 to shift left
 * @return Return the function status
 */
extern LCD1602_I2C_Status_t LCD1602_I2C_ShiftDisplay(int right);

// Test functions declaration
extern void test_lcd_i2c_display_shift(void);
extern void test_lcd_i2c_cursor_shift(void);

#endif // LCD_I2C_H