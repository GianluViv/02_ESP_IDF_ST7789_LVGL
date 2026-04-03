#include "display_mng.h" // Gestione display, touch e LVGL

static const char *TAG = "MAIN"; // Tag per i log di sistema

void app_main(void) {
  /* Inizializzazione Hardware dell'LCD (bus SPI e driver ST7789) */
  ESP_ERROR_CHECK(app_lcd_init());

  /* Configurazione del bus I2C per il Touch Controller */
  ESP_LOGI(TAG, "Initialize I2C bus");
  esp_log_level_set("lcd_panel.io.i2c", ESP_LOG_NONE);
  esp_log_level_set("CST816S", ESP_LOG_NONE);

  const i2c_config_t i2c_conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = PIN_NUM_TOUCH_SDA,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_io_num = PIN_NUM_TOUCH_SCL,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = 100 * 1000,
  };
  i2c_param_config(TOUCH_HOST, &i2c_conf); // Applica configurazione I2C
  i2c_driver_install(TOUCH_HOST, i2c_conf.mode, 0, 0, 0); // Installa driver I2C

  /* Configurazione dell'interfaccia IO I2C per il chip Touch */
  esp_lcd_panel_io_handle_t tp_io_handle = NULL;
  const esp_lcd_panel_io_i2c_config_t tp_io_config =
      ESP_LCD_TOUCH_IO_I2C_CST816S_CONFIG();
  
  // Collega il chip TOUCH al bus I2C appena creato
  esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)TOUCH_HOST, &tp_io_config,
                           &tp_io_handle);

  /* Configurazione parametri specifici del pannello Touch */
  const esp_lcd_touch_config_t tp_cfg = {
      .x_max = LCD_H_RES, // Risoluzione massima X
      .y_max = LCD_V_RES, // Risoluzione massima Y
      .rst_gpio_num = PIN_NUM_TOUCH_RST, // Pin Reset
      .int_gpio_num = PIN_NUM_TOUCH_INT, // Pin Interrupt
      .levels = {
          .reset = 0,
          .interrupt = 0,
      },
      .flags = {
          .swap_xy = 0,
          .mirror_x = 0,
          .mirror_y = 0,
      },
  };

  /* Inizializzazione del controller touch CST816S */
  ESP_LOGI(TAG, "Initialize touch controller");
  esp_lcd_touch_new_i2c_cst816s(tp_io_handle, &tp_cfg, &tp);

  /* Inizializzazione della libreria grafica LVGL */
  ESP_ERROR_CHECK(app_lvgl_init());

  /* Registrazione del driver di input (Touch) in LVGL */
  static lv_indev_drv_t indev_drv; 
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER; // Dispositivo di puntamento
  indev_drv.disp = lvgl_disp; // Associa al display principale
  indev_drv.read_cb = lvgl_touch_cb; // Funzione di callback per leggere i dati
  indev_drv.user_data = tp; // Passa l'handle del touch alla callback
  lv_indev_drv_register(&indev_drv); // Registra il driver in LVGL

  /* Inizializzazione dell'interfaccia utente generata da EEZ Studio */
  LVGL_SAFE(ui_init());

  app_lcd_set_brightness(25);

  /* Loop principale dell'applicazione */
  while (1) {
    /* Esecuzione della logica UI in modo thread-safe (tick di EEZ Studio) */
    LVGL_SAFE(ui_tick());

    /* Ritardo per non saturare la CPU e permettere lo scheduling */
    vTaskDelay(pdMS_TO_TICKS(10));
  }
} // Fine di app_main
