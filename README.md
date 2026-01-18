# LCD1602 with PCF8574 I2C module — Driver

**Overview**
- **Description:** A lightweight C driver for HD44780-compatible LCD1602 displays connected via a PCF8574 I2C I/O expander. Designed for STM32 using the HAL I2C API; the HAL calls can be replaced to port the driver to other platforms.

**Features**
- **4-bit mode:** Uses PCF8574 to send nibbles (DB4..DB7) to the LCD.
- **Common operations:** Init, clear, move cursor, write char/string, and shift display.
- **Backlight:** Controlled through the PCF8574; default behavior keeps backlight on (can be changed in code).

**Files**
- **Driver:** [lcd_i2c.c](lcd_i2c.c) and [lcd_i2c.h](lcd_i2c.h)

**Requirements**
- **Toolchain:** Any C toolchain for your MCU (examples use STM32 HAL).
- **HAL:** Uses `HAL_I2C_Master_Transmit`, `HAL_I2C_IsDeviceReady`, `HAL_Delay` and `HAL_StatusTypeDef` from STM32 HAL. Replace these calls in the driver if you use another platform.

**Hardware**
- **I2C expander:** PCF8574 (or compatible). Default `PCF8574_ADDRESS` is `0x4E` in the driver — change this macro if your module uses a different address.
- **LCD:** Standard HD44780-compatible 16x2 (LCD1602) in 4-bit mode.

**Wiring (typical)**
- **I2C lines:** Connect the module SDA -> MCU SDA, SCL -> MCU SCL, VCC -> 5V (or 3.3V depending on module), GND -> GND.
- **PCF8574 pins mapping to LCD (driver expects):**
	- P0: `RS`
	- P1: `R/W` (driver uses write operations only)
	- P2: `EN` (Enable)
	- P3: `BL` (Backlight)
	- P4: `DB4`
	- P5: `DB5`
	- P6: `DB6`
	- P7: `DB7`

**Build & Usage**
- **Include:** Add [lcd_i2c.h](lcd_i2c.h) to your project and compile `lcd_i2c.c` with your firmware.
- **I2C handle:** The driver expects an `I2C_HandleTypeDef*` (STM32 HAL). Pass your bus handle to `LCD1602_I2C_Init`.

**API (important functions)**
- `LCD1602_I2C_Init(I2C_HandleTypeDef* hi2c)`: Initialize the PCF8574-backed LCD. Returns `HAL_StatusTypeDef` style status.
- `LCD1602_I2C_Clear(void)`: Clear the display.
- `LCD1602_I2C_MoveCursor(int x, int y)`: Move cursor to column `x` (0..39) and row `y` (0..1).
- `LCD1602_I2C_ShowChar(char c)`: Write a single character at the current cursor.
- `LCD1602_I2C_ShowString(char* str)`: Write a null-terminated string starting at the current cursor.
- `LCD1602_I2C_ShiftDisplay(int right)`: Shift the entire display; pass `1` to shift right, `0` to shift left.

**Example (STM32 HAL)**
```c
// Assuming hi2c1 is configured elsewhere (CubeMX or manual init)
extern I2C_HandleTypeDef hi2c1;

void app_main(void){
		if(LCD1602_I2C_Init(&hi2c1) != HAL_OK){
				// handle error
		}

		LCD1602_I2C_Clear();
		LCD1602_I2C_MoveCursor(0, 0);
		LCD1602_I2C_ShowString("Hello, world!");

		LCD1602_I2C_MoveCursor(0, 1);
		LCD1602_I2C_ShowString("PCF8574 I2C");
}
```

**Notes & Troubleshooting**
- **Address:** If the display does not respond, verify the PCF8574 I2C address and adjust `PCF8574_ADDRESS` in [lcd_i2c.h](lcd_i2c.h). The driver uses `0x4E` (8-bit form; module/address wiring may use 0x27 or other 7-bit addresses depending on representation).
- **Voltage levels:** Many PCF8574 modules and LCDs require 5V for reliable contrast/backlight. Ensure logic levels are compatible with your MCU or use a level shifter.
- **Busy flag / reads:** This driver implements write operations and sets timings using `HAL_Delay`. For more robust timing, replace delays with a non-blocking timer if required.
- **Customization:** To adapt to non-STM32 platforms, replace HAL I2C calls in `lcd_i2c.c` with your platform's I2C transmit/is-ready equivalents.

**License**
- **License:** MIT (feel free to add a `LICENSE` file if you prefer another license).

If you'd like, I can also add a small example project or a Makefile to build a minimal test firmware.