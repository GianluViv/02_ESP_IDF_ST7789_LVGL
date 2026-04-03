#ifndef DISPLAY_MNG_H
#define DISPLAY_MNG_H

#include "global.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Handles */
extern esp_lcd_touch_handle_t tp;
extern esp_lcd_panel_io_handle_t lcd_io;
extern esp_lcd_panel_handle_t lcd_panel;
extern lv_display_t *lvgl_disp;

/* Function prototypes */
esp_err_t app_lcd_init(void);
esp_err_t app_lvgl_init(void);
void lvgl_touch_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);
void app_lcd_set_brightness(int percentage);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_MNG_H
