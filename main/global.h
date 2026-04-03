#include "../ui/ui.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/spi_master.h" // IWYU pragma: keep
#include "esp_check.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h" // IWYU pragma: keep
#include "esp_lcd_touch_cst816s.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h"
#include "lv_conf.h"

/* LCD size */
#define LCD_H_RES (240)
#define LCD_V_RES (280)

/* LCD settings */
#define LCD_SPI_NUM (SPI2_HOST)
#define LCD_PIXEL_CLK_HZ (40 * 1000 * 1000)
#define LCD_CMD_BITS (8)
#define LCD_PARAM_BITS (8)
#define LCD_COLOR_SPACE (ESP_LCD_COLOR_SPACE_RGB)
#define LCD_BITS_PER_PIXEL (16)
#define LCD_DRAW_BUFF_DOUBLE (1)
#define LCD_DRAW_BUFF_HEIGHT (50)
#define LCD_BL_ON_LEVEL (1)

/* LCD pins */
#define LCD_GPIO_SCLK (GPIO_NUM_6)
#define LCD_GPIO_MOSI (GPIO_NUM_7)
#define LCD_GPIO_RST (GPIO_NUM_8)
#define LCD_GPIO_DC (GPIO_NUM_4)
#define LCD_GPIO_CS (GPIO_NUM_5)
#define LCD_GPIO_BL (GPIO_NUM_15)

#define TOUCH_HOST I2C_NUM_0

#define PIN_NUM_TOUCH_SCL (GPIO_NUM_10)
#define PIN_NUM_TOUCH_SDA (GPIO_NUM_11)
#define PIN_NUM_TOUCH_RST (GPIO_NUM_13)
#define PIN_NUM_TOUCH_INT (GPIO_NUM_14)