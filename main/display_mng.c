#include "display_mng.h"

static const char *TAG = "DISP_MNG";

/* LCD and display handles */
esp_lcd_touch_handle_t tp = NULL;
esp_lcd_panel_io_handle_t lcd_io = NULL;
esp_lcd_panel_handle_t lcd_panel = NULL;
lv_display_t *lvgl_disp = NULL;

esp_err_t app_lcd_init(void) {
  esp_err_t ret = ESP_OK;

  /* LCD backlight init (PWM) */
  const ledc_timer_config_t ledc_timer = {
      .speed_mode = LCD_BL_LEDC_MODE,
      .timer_num = LCD_BL_LEDC_TIMER,
      .duty_resolution = LCD_BL_LEDC_RESOLUTION,
      .freq_hz = LCD_BL_LEDC_FREQ_HZ,
      .clk_cfg = LEDC_AUTO_CLK};
  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  const ledc_channel_config_t ledc_channel = {
      .speed_mode = LCD_BL_LEDC_MODE,
      .channel = LCD_BL_LEDC_CHANNEL,
      .timer_sel = LCD_BL_LEDC_TIMER,
      .intr_type = LEDC_INTR_DISABLE,
      .gpio_num = LCD_GPIO_BL,
      .duty = 0, // Inizia spento
      .hpoint = 0};
  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

  /* LCD initialization */
  ESP_LOGD(TAG, "Initialize SPI bus");
  const spi_bus_config_t buscfg = {
      .sclk_io_num = LCD_GPIO_SCLK,
      .mosi_io_num = LCD_GPIO_MOSI,
      .miso_io_num = GPIO_NUM_NC,
      .quadwp_io_num = GPIO_NUM_NC,
      .quadhd_io_num = GPIO_NUM_NC,
      .max_transfer_sz =
          LCD_H_RES * LCD_DRAW_BUFF_HEIGHT * sizeof(uint16_t),
  };
  ESP_RETURN_ON_ERROR(
      spi_bus_initialize(LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO), TAG,
      "SPI init failed");

  ESP_LOGD(TAG, "Install panel IO");
  const esp_lcd_panel_io_spi_config_t io_config = {
      .dc_gpio_num = LCD_GPIO_DC,
      .cs_gpio_num = LCD_GPIO_CS,
      .pclk_hz = LCD_PIXEL_CLK_HZ,
      .lcd_cmd_bits = LCD_CMD_BITS,
      .lcd_param_bits = LCD_PARAM_BITS,
      .spi_mode = 0,
      .trans_queue_depth = 10,
  };
  ESP_GOTO_ON_ERROR(
      esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_SPI_NUM,
                               &io_config, &lcd_io),
      err, TAG, "New panel IO failed");

  ESP_LOGD(TAG, "Install LCD driver");
  const esp_lcd_panel_dev_config_t panel_config = {
      .reset_gpio_num = LCD_GPIO_RST,
      .color_space = LCD_COLOR_SPACE,
      .bits_per_pixel = LCD_BITS_PER_PIXEL,
  };
  ESP_GOTO_ON_ERROR(esp_lcd_new_panel_st7789(lcd_io, &panel_config, &lcd_panel),
                    err, TAG, "New panel failed");

  esp_lcd_panel_reset(lcd_panel);
  esp_lcd_panel_init(lcd_panel);
  esp_lcd_panel_mirror(lcd_panel, true, true);
  esp_lcd_panel_disp_on_off(lcd_panel, true);

  /* LCD backlight on (70% brightness default) */
  app_lcd_set_brightness(70);

  esp_lcd_panel_set_gap(lcd_panel, 0, 20);
  esp_lcd_panel_invert_color(lcd_panel, true);

  return ret;

err:
  if (lcd_panel) {
    esp_lcd_panel_del(lcd_panel);
  }
  if (lcd_io) {
    esp_lcd_panel_io_del(lcd_io);
  }
  spi_bus_free(LCD_SPI_NUM);
  return ret;
}

void lvgl_touch_cb(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  esp_lcd_touch_handle_t touch_handle = (esp_lcd_touch_handle_t)drv->user_data;
  assert(touch_handle);

  uint16_t tp_x;
  uint16_t tp_y;
  uint8_t tp_cnt = 0;
  /* Read data from touch controller into memory */
  esp_lcd_touch_read_data(touch_handle);
  /* Read data from touch controller */
  bool tp_pressed =
      esp_lcd_touch_get_coordinates(touch_handle, &tp_x, &tp_y, NULL, &tp_cnt, 1);
  if (tp_pressed && tp_cnt > 0) {
    data->point.x = tp_x;
    data->point.y = tp_y;
    data->state = LV_INDEV_STATE_PRESSED;
    ESP_LOGD(TAG, "Touch position: %d,%d", tp_x, tp_y);
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

esp_err_t app_lvgl_init(void) {
  /* Initialize LVGL */
  const lvgl_port_cfg_t lvgl_cfg = {
      .task_priority = 4,  /* LVGL task priority */
      .task_stack = 4096,  /* LVGL task stack size */
      .task_affinity = -1, /* LVGL task pinned to core (-1 is no affinity) */
      .task_max_sleep_ms = 500, /* Maximum sleep in LVGL task */
      .timer_period_ms = 5      /* LVGL timer tick period in ms */
  };
  ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG,
                       "LVGL port initialization failed");

  /* Add LCD screen */
  ESP_LOGD(TAG, "Add LCD screen");
  const lvgl_port_display_cfg_t disp_cfg = {
      .io_handle = lcd_io,
      .panel_handle = lcd_panel,
      .buffer_size =
          LCD_H_RES * LCD_DRAW_BUFF_HEIGHT * sizeof(uint16_t),
      .double_buffer = LCD_DRAW_BUFF_DOUBLE,
      .hres = LCD_H_RES,
      .vres = LCD_V_RES,
      .monochrome = false,
      /* Rotation values must be same as used in esp_lcd for initial settings of
         the screen */
      .rotation =
          {
              .swap_xy = false,
              .mirror_x = false,
              .mirror_y = false,
          },
      .flags = {
          .buff_dma = true,
      }};
  lvgl_disp = lvgl_port_add_disp(&disp_cfg);

  return ESP_OK;
}

/**
 * @brief Set LCD backlight brightness
 * 
 * @param percentage 0-100
 */
void app_lcd_set_brightness(int percentage) {
  if (percentage > 100) percentage = 100;
  if (percentage < 0) percentage = 0;

  uint32_t duty = (uint32_t)((1023) * percentage / 100);
  
  /* Invert duty if BL_ON_LEVEL is 0 (not typical for most modules) */
#if !LCD_BL_ON_LEVEL
  duty = 1023 - duty;
#endif

  ESP_ERROR_CHECK(ledc_set_duty(LCD_BL_LEDC_MODE, LCD_BL_LEDC_CHANNEL, duty));
  ESP_ERROR_CHECK(ledc_update_duty(LCD_BL_LEDC_MODE, LCD_BL_LEDC_CHANNEL));
}
