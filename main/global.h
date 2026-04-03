#ifndef GLOBAL_H
#define GLOBAL_H

#include "ui.h" // IWYU pragma: keep
#include "driver/gpio.h" // IWYU pragma: keep
#include "driver/i2c.h" // IWYU pragma: keep
#include "driver/spi_master.h" // IWYU pragma: keep
#include "esp_check.h" // IWYU pragma: keep
#include "esp_err.h" // IWYU pragma: keep
#include "esp_lcd_panel_io.h" // IWYU pragma: keep
#include "esp_lcd_panel_ops.h" // IWYU pragma: keep
#include "esp_lcd_panel_vendor.h" // IWYU pragma: keep
#include "esp_lcd_touch_cst816s.h" // IWYU pragma: keep
#include "esp_log.h" // IWYU pragma: keep
#include "esp_lvgl_port.h" // IWYU pragma: keep
#include "freertos/FreeRTOS.h" // IWYU pragma: keep
#include "freertos/task.h" // IWYU pragma: keep
#include "driver/ledc.h" // IWYU pragma: keep
#include "lv_conf.h" // IWYU pragma: keep

/* LVGL Locking Macros */
#define LV_LOCK()     lvgl_port_lock(-1)
#define LV_UNLOCK()   lvgl_port_unlock()
#define LVGL_SAFE(x)  do { LV_LOCK(); x; LV_UNLOCK(); } while(0)

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

/* LEDC PWM settings for Backlight */
#define LCD_BL_LEDC_TIMER      LEDC_TIMER_0
#define LCD_BL_LEDC_MODE       LEDC_LOW_SPEED_MODE
#define LCD_BL_LEDC_CHANNEL    LEDC_CHANNEL_0
#define LCD_BL_LEDC_RESOLUTION LEDC_TIMER_10_BIT
#define LCD_BL_LEDC_FREQ_HZ    5000 // 5 kHz

#define TOUCH_HOST I2C_NUM_0

#define PIN_NUM_TOUCH_SCL (GPIO_NUM_10)
#define PIN_NUM_TOUCH_SDA (GPIO_NUM_11)
#define PIN_NUM_TOUCH_RST (GPIO_NUM_13)
#define PIN_NUM_TOUCH_INT (GPIO_NUM_14)

#endif // GLOBAL_H