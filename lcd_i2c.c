#include "lcd_i2c.h"

// Global variables


// Local variables
I2C_HandleTypeDef *g_hi2c;
__UINT8_TYPE__ g_displayOffset = 0;
__UINT8_TYPE__ g_cursorPos[2] = {0, 0};

// Local functions declaration

/**
 * @brief Clears entire display and sets DDRAM address 0 in address counter. (Implemented in 4 bit mode)
 * @name LCD1602_I2C_Clear_Display
 * @return Return the function status
 */
static LCD1602_I2C_Status_t LCD1602_I2C_Clear_Display(void);

/**
 * @brief  Sets DDRAM address 0 in address counter. Also returns display from being shifted to original position. DDRAM contents remain unchanged. (Implemented in 4 bit mode)
 * @name LCD1602_I2C_ReturnHome
 * @return Return the function status
 */
static LCD1602_I2C_Status_t LCD1602_I2C_ReturnHome(void);

/**
 * @brief  Sets cursor move direction and specifies display shift. These operations are performed during data write and read. (Implemented in 4 bit mode)
 * @name LCD1602_I2C_EntryModeSet
 * @param increment: Set to 1 to increment the cursor position, 0 to decrement
 * @param shift: Set to 1 to shift the display, 0 to not shift
 * @return Return the function status
 */
static LCD1602_I2C_Status_t LCD1602_I2C_EntryModeSet(__UINT8_TYPE__ increment, __UINT8_TYPE__ shift);

/**
 * @brief Sets entire display (D) on/off, cursor on/off (C), and blinking of cursor position character (B). (Implemented in 4 bit mode)
 * @name LCD1602_I2C_DisplayControl
 * @param displayOn: Set to 1 to turn on the display, 0 to turn off
 * @param cursorOn: Set to 1 to turn on the cursor, 0 to turn off
 * @param blinkOn: Set to 1 to turn on the blinking cursor, 0 to turn off
 * @return Return the function status
 */
static LCD1602_I2C_Status_t LCD1602_I2C_DisplayControl(__UINT8_TYPE__ displayOn, __UINT8_TYPE__ cursorOn, __UINT8_TYPE__ blinkOn);

/**
 * @brief  Moves cursor and shifts display without changing DDRAM contents. (Implemented in 4 bit mode)
 * @name LCD1602_I2C_CursorDisplayShift
 * @param shiftDisplay: Set to 1 to shift display, 0 to move cursor
 * @param shiftRight: Set to 1 to shift/move right, 0 to shift/move left
 * @return Return the function status
 */
static LCD1602_I2C_Status_t LCD1602_I2C_CursorDisplayShift(__UINT8_TYPE__ shiftDisplay, __UINT8_TYPE__ shiftRight);

/**
 * @brief Sets number of display lines (N), and character font (F). This function doesn't has data length because it is always 4-bit mode. (Implemented in 4 bit mode)
 * @name LCD1602_I2C_FunctionSet
 * @param numLines: Set to 1 for 2 lines, 0 for 1 line
 * @param fontType: Set to 1 for 5x10 dots, 0 for 5x8 dots
 * @return Return the function status
 */
static LCD1602_I2C_Status_t LCD1602_I2C_FunctionSet(__UINT8_TYPE__ numLines, __UINT8_TYPE__ fontType);

/**
 * @brief Sets CGRAM address. CGRAM data is sent and received after this setting. (Implemented in 4 bit mode)
 * @name LCD1602_I2C_SetCGRAMAddress
 * @param address: The CGRAM address to set (0-63)
 * @return Return the function status
 */
static LCD1602_I2C_Status_t LCD1602_I2C_SetCGRAMAddress(__UINT8_TYPE__ address);

/**
 * @brief Sets DDRAM address. DDRAM data is sent and received after this setting. (Implemented in 4 bit mode)
 * @name LCD1602_I2C_SetDDRAMAddress
 * @param address: The DDRAM address to set (0-79)
 * @return Return the function status
 */
static LCD1602_I2C_Status_t LCD1602_I2C_SetDDRAMAddress(__UINT8_TYPE__ address);

/**
 * @brief  Reads busy flag (BF) indicating internal operation is being performed and reads address counter contents. (Implemented in 4 bit mode)
 * @name LCD1602_I2C_Read_BusyFlag_Address
 * @param address: Pointer to store the address counter value, only the lower 7 bits are valid. The busy flag is stored in the highest bit (bit 7).
 * @return Return the function status
 */
static LCD1602_I2C_Status_t LCD1602_I2C_Read_BusyFlag_Address(__UINT8_TYPE__* address);

/**
 * @brief Writes data into DDRAM or CGRAM. (Dependent on the previous setting of DDRAM/CGRAM address). (Implemented in 4 bit mode)
 * @name LCD1602_I2C_Write_Data
 * @param data: The data to write
 * @return Return the function status
 */
static LCD1602_I2C_Status_t LCD1602_I2C_Write_Data(__UINT8_TYPE__ data);

/**
 * @brief Reads data from DDRAM or CGRAM. (Dependent on the previous setting of DDRAM/CGRAM address). (Implemented in 4 bit mode)
 * @name LCD1602_I2C_Read_Data
 * @param data: Pointer to store the read data
 * @return Return the function status
 */
static LCD1602_I2C_Status_t LCD1602_I2C_Read_Data(__UINT8_TYPE__* data);

/**
 * @brief Set the LCD1602 to operate in 4 bits mode.
 * @name LCD1602_I2C_Set4BitMode
 * @return Return the function status
 */
static LCD1602_I2C_Status_t LCD1602_I2C_Set4BitMode(void);

/**
 * @brief This is the main function that sends instructions/datas to the LCD1602. In 4 bits mode, only DB4 to DB7 are used for transfer, while DB0 to DB3 are not used. When using 4 bit mode, instructions/datas are sent in two phases: first the higher nibble (DB7 to DB4), then the lower nibble (DB3 to DB0). Both phases are sent through DB4-DB7, the Enable pulse notifies the HD44780U about the phases. When a read operation is performed, the data read from the LCD1602 is stored back into the variable pointed by "cmd". (Implemented in 4 bit mode)
 * @name LCD1602_I2C_SendToLCD
 * @param data: The data that needs to be sent (cmd, addr, request), only the first 10 bits are valid
 * @param isBacklightOn: Set to 1 to turn on backlight, 0 to turn off backlight
 * @return Return the function status
 */
static LCD1602_I2C_Status_t LCD1602_I2C_SendToLCD(__UINT16_TYPE__* data, __UINT8_TYPE__ isBacklightOn);





// Local functions definition

LCD1602_I2C_Status_t LCD1602_I2C_Clear_Display(void){
    LCD1602_I2C_Status_t status = HAL_OK;
    __UINT8_TYPE__ cmd = 0b0000000001; // Clear display command
    status = LCD1602_I2C_SendToLCD(&cmd, 1);
    return status;
}


LCD1602_I2C_Status_t LCD1602_I2C_ReturnHome(void){
    LCD1602_I2C_Status_t status = HAL_OK;
    __UINT8_TYPE__ cmd = 0b0000000010; // Return home command
    status = LCD1602_I2C_SendToLCD(&cmd, 1);
    return status;
}


LCD1602_I2C_Status_t LCD1602_I2C_EntryModeSet(__UINT8_TYPE__ increment, __UINT8_TYPE__ shift){
    LCD1602_I2C_Status_t status = HAL_OK;
    __UINT8_TYPE__ cmd = 0b0000000100; // Entry mode set command
    if(increment) cmd |= (1 << 1); // Increment cursor
    if(shift) cmd |= (1 << 0); // Shift display
    return LCD1602_I2C_SendToLCD(&cmd, 1);
}


LCD1602_I2C_Status_t LCD1602_I2C_DisplayControl(__UINT8_TYPE__ displayOn, __UINT8_TYPE__ cursorOn, __UINT8_TYPE__ blinkOn){
    LCD1602_I2C_Status_t status = HAL_OK;
    __UINT8_TYPE__ cmd = 0b0000001000; // Display control command
    if(displayOn) cmd |= (1 << 2); // Display ON
    if(cursorOn) cmd |= (1 << 1); // Cursor ON
    if(blinkOn) cmd |= (1 << 0); // Blink ON
    return LCD1602_I2C_SendToLCD(&cmd, 1);
}


LCD1602_I2C_Status_t LCD1602_I2C_CursorDisplayShift(__UINT8_TYPE__ shiftDisplay, __UINT8_TYPE__ shiftRight){
    LCD1602_I2C_Status_t status = HAL_OK;
    __UINT8_TYPE__ cmd = 0b0000010000; // Cursor or display shift command

    if(shiftDisplay){ // Shift display
        cmd |= (1 << 3);
        if(shiftRight){ // Shift right
            cmd |= (1 << 2);
            g_displayOffset = (g_displayOffset + 1) >= 40 ? 0 : g_displayOffset + 1; // Wrap around at 40
        } else { // Shift left
            g_displayOffset = (g_displayOffset - 1) < 0  ? 39 : g_displayOffset - 1; // Wrap around at 0
        }
    } else { // Move cursor
        if(shiftRight){ // Move right
            cmd |= (1 << 2);
            g_cursorPos[0]++; // Update cursor column position
            if(g_cursorPos[0] >= 40){ // If exceeds column limit
                g_cursorPos[0] = 0;
                g_cursorPos[1] = (g_cursorPos[1] + 1) % 2; // Move to next row
            }
        } else { // Move left
            if(g_cursorPos[0] == 0){ // If at the beginning of the line
                g_cursorPos[0] = 39;
                g_cursorPos[1] = (g_cursorPos[1] - 1) < 0 ? 1 : g_cursorPos[1] - 1; // Move to previous row
            } else {
                g_cursorPos[0]--;
            }
        }
    }
    return LCD1602_I2C_SendToLCD(&cmd, 1);
}


LCD1602_I2C_Status_t LCD1602_I2C_FunctionSet(__UINT8_TYPE__ numLines, __UINT8_TYPE__ fontType){
    __UINT8_TYPE__ cmd = 0b0000100000; // Function set command
    if(numLines) cmd |= (1 << 3); // 2 lines
    if(fontType) cmd |= (1 << 2); // 5x10 dots
    return LCD1602_I2C_SendToLCD(&cmd, 1);
}


LCD1602_I2C_Status_t LCD1602_I2C_SetCGRAMAddress(__UINT8_TYPE__ address){
    LCD1602_I2C_Status_t status = HAL_OK;
    __UINT8_TYPE__ cmd = 0b0001000000; // Set CGRAM address command
    cmd |= (address & 0x3F); // Set address (6 bits)
    status = LCD1602_I2C_SendToLCD(&cmd, 1);
    return status;
}


LCD1602_I2C_Status_t LCD1602_I2C_SetDDRAMAddress(__UINT8_TYPE__ address){
    LCD1602_I2C_Status_t status = HAL_OK;
    __UINT8_TYPE__ cmd = 0b0010000000; // Set DDRAM address command
    cmd |= (address & 0x7F); // Set address (7 bits)
    status = LCD1602_I2C_SendToLCD(&cmd, 1);
    return status;
}


LCD1602_I2C_Status_t LCD1602_I2C_Read_BusyFlag_Address(__UINT8_TYPE__* address){
    return HAL_OK;
}


LCD1602_I2C_Status_t LCD1602_I2C_Write_Data(__UINT8_TYPE__ data){
    LCD1602_I2C_Status_t status = HAL_OK;
    __UINT16_TYPE__ cmd = 0b1000000000; // Data write command
    cmd |= (data & 0xFF); // Set data (8 bits)
    status = LCD1602_I2C_SendToLCD(&cmd, 1);
    return status;
}


LCD1602_I2C_Status_t LCD1602_I2C_Read_Data(__UINT8_TYPE__* data){
    return HAL_OK;
}


LCD1602_I2C_Status_t LCD1602_I2C_Set4BitMode(void){
    LCD1602_I2C_Status_t status = HAL_OK;
    __UINT8_TYPE__ data = 0b00000000;

    data |= (1 << 5); // Function set command
    data |= (1 << EN_INDEX_PIN); // Toggle Enable pin
    data |= (1 << BL_INDEX_PIN); // Toggle Backlight pin
    status = HAL_I2C_Master_Transmit(g_hi2c, PCF8574_ADDRESS, (unsigned char*)&data, 1, HAL_MAX_DELAY);
    
    if(status != HAL_OK) return status;
    
    data &= ~(1 << EN_INDEX_PIN); // Toggle Enable pin
    status = HAL_I2C_Master_Transmit(g_hi2c, PCF8574_ADDRESS, (unsigned char*)&data, 1, HAL_MAX_DELAY);

    return status;
}


LCD1602_I2C_Status_t LCD1602_I2C_SendToLCD(__UINT16_TYPE__* cmd, __UINT8_TYPE__ isBacklightOn){
    LCD1602_I2C_Status_t status = HAL_OK;
    __UINT8_TYPE__ data = 0x00 | (isBacklightOn ? (1 << BL_INDEX_PIN) : 0x00);

    data |= (*cmd & MSK_RS) >> (RS_INDEX_MSK - RS_INDEX_PIN); // Set RS pin
    data |= (*cmd & MSK_RW) >> (RW_INDEX_MSK - RW_INDEX_PIN); // Set R/~W pin
    data |= (*cmd & MSK_DB7) >> (DB7_INDEX_MSK - DB7_INDEX_PIN); // Set DB7 pin
    data |= (*cmd & MSK_DB6) >> (DB6_INDEX_MSK - DB6_INDEX_PIN); // Set DB6 pin
    data |= (*cmd & MSK_DB5) >> (DB5_INDEX_MSK - DB5_INDEX_PIN); // Set DB5 pin
    data |= (*cmd & MSK_DB4) >> (DB4_INDEX_MSK - DB4_INDEX_PIN); // Set DB4 pin
    data |= 0x01 << EN_INDEX_PIN; // Toggle Enable pin

    status = HAL_I2C_IsDeviceReady(g_hi2c, PCF8574_ADDRESS, 3, HAL_MAX_DELAY);
    if(status != HAL_OK) return status;

    status = HAL_I2C_Master_Transmit(g_hi2c, PCF8574_ADDRESS, (unsigned char*)&data, 1, HAL_MAX_DELAY); // Send higher nibble
    if(status != HAL_OK) return status;

    data &= ~(0x01 << EN_INDEX_PIN); // Toggle Enable pin
    status = HAL_I2C_Master_Transmit(g_hi2c, PCF8574_ADDRESS, (unsigned char*)&data, 1, HAL_MAX_DELAY);
    if(status != HAL_OK) return status;

    data |= 0x01 << EN_INDEX_PIN; // Toggle Enable pin
    data &= 0x0F; // Clear data bits
    data |= (*cmd & MSK_DB3) << (DB3_INDEX_PIN - DB3_INDEX_MSK); // Set DB3 pin
    data |= (*cmd & MSK_DB2) << (DB2_INDEX_PIN - DB2_INDEX_MSK); // Set DB2 pin
    data |= (*cmd & MSK_DB1) << (DB1_INDEX_PIN - DB1_INDEX_MSK); // Set DB1 pin
    data |= (*cmd & MSK_DB0) << (DB0_INDEX_PIN - DB0_INDEX_MSK); // Set DB0 pin
    status = HAL_I2C_Master_Transmit(g_hi2c, PCF8574_ADDRESS, (unsigned char*)&data, 1, HAL_MAX_DELAY); // Send lower nibble
    if(status != HAL_OK) return status;

    data &= ~(0x01 << EN_INDEX_PIN); // Toggle Enable pin
    status = HAL_I2C_Master_Transmit(g_hi2c, PCF8574_ADDRESS, (unsigned char*)&data, 1, HAL_MAX_DELAY);

    return status;
}





// Global functions definition

LCD1602_I2C_Status_t LCD1602_I2C_Init(I2C_HandleTypeDef* hi2c){
    LCD1602_I2C_Status_t status = HAL_OK;
    __UINT16_TYPE__ cmd = 0b00000000;
    g_hi2c = hi2c;

    // Wait for the LCD to power up, we can replace all HAL_Delay functions with a more precise timer if needed
    HAL_Delay(100); // in ms
    
    HAL_I2C_Master_Transmit(g_hi2c, PCF8574_ADDRESS, (unsigned char*)(0b00111100), 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Transmit(g_hi2c, PCF8574_ADDRESS, (unsigned char*)(0b00111000), 1, HAL_MAX_DELAY);
    HAL_Delay(7); // in ms

    HAL_I2C_Master_Transmit(g_hi2c, PCF8574_ADDRESS, (unsigned char*)(0b00111100), 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Transmit(g_hi2c, PCF8574_ADDRESS, (unsigned char*)(0b00111000), 1, HAL_MAX_DELAY);
    HAL_Delay(1); // in ms

    HAL_I2C_Master_Transmit(g_hi2c, PCF8574_ADDRESS, (unsigned char*)(0b00111100), 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Transmit(g_hi2c, PCF8574_ADDRESS, (unsigned char*)(0b00111000), 1, HAL_MAX_DELAY);

    // Set 4-bit operation mode
    status = LCD1602_I2C_Set4BitMode();
    if(status != HAL_OK) return status;

    // Function set: 2 lines, 5x8 dots
    status = LCD1602_I2C_FunctionSet(1, 0);
    if(status != HAL_OK) return status;

    // Display ON, Cursor ON, Blink OFF
    status = LCD1602_I2C_DisplayControl(1, 1, 0);
    if(status != HAL_OK) return status;

    status = LCD1602_I2C_Clear_Display();
    if(status != HAL_OK) return status;

    status = LCD1602_I2C_EntryModeSet(1, 1); // Increment cursor, with display shift
    if(status != HAL_OK) return status;
    // Wait for more than 39us
    HAL_Delay(1);

    return status;
}


LCD1602_I2C_Status_t LCD1602_I2C_Clear(void){
    return LCD1602_I2C_Clear_Display();
}


LCD1602_I2C_Status_t LCD1602_I2C_MoveCursor(int x, int y){
    __UINT8_TYPE__ addr = 0b00000000;

    if(x < 0 || x >= 40 || y < 0 || y >= 2){
        return HAL_ERROR; // Invalid position
    }

    g_cursorPos[0] = (__UINT8_TYPE__)x;
    g_cursorPos[1] = (__UINT8_TYPE__)y;

    if(y == 1){
        addr |= 0x40;
    }
    addr |= (0x27 - g_displayOffset + 1 + (__UINT8_TYPE__)x) % 0x28;
    return LCD1602_I2C_SetDDRAMAddress(addr);
}


LCD1602_I2C_Status_t LCD1602_I2C_ShowChar(char c){
    return LCD1602_I2C_Write_Data(c);
}


LCD1602_I2C_Status_t LCD1602_I2C_ShowString(char* str){
    LCD1602_I2C_Status_t status = HAL_OK;
    while(*str){
        status = LCD1602_I2C_Write_Data((__UINT8_TYPE__)(*str));
        if(status != HAL_OK) return status;
        str++;
    }
    return status;
}


LCD1602_I2C_Status_t LCD1602_I2C_ShiftDisplay(int right){
    if(right != 0 && right != 1){
        return HAL_ERROR; // Invalid parameter
    }
    return LCD1602_I2C_CursorDisplayShift(1, right);
}


// Test functions definition
void test_lcd_i2c_display_shift(void){
    LCD1602_I2C_CursorDisplayShift(1, 0); // Shift display left
    HAL_Delay(400);
}

void test_lcd_i2c_cursor_shift(void){
    LCD1602_I2C_CursorDisplayShift(0, 1); // Move cursor right
    HAL_Delay(400);
}

void test_lcd_i2c_char_write_spam(void){
    for(__UINT8_TYPE__ i = 0; i < 100; i++){
        LCD1602_I2C_Write_Data('A' + (i % 26));
        HAL_Delay(200);
    }
}

void test_lcd_i2c_ddram_addressing(void){
    LCD1602_I2C_SetDDRAMAddress(0x0A);
    LCD1602_I2C_Write_Data('X');
}

void test_lcd_i2c_busy_flag_address(__UINT8_TYPE__* address){
    LCD1602_I2C_Read_BusyFlag_Address(address);
}