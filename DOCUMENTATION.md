# Documentazione Progetto ESP32-ST7789-LVGL

Questo progetto implementa un'interfaccia grafica su un display ST7789 (240x280) utilizzando la libreria 
**LVGL v8** e il framework **ESP-IDF**. L'interfaccia è stata progettata con **EEZ Studio**.

## Struttura del Software

Il progetto è diviso principalmente in due parti:
1.  **Hardware & Middleware (`main/main.c`)**: Gestisce l'inizializzazione delle periferiche (SPI, I2C), 
    del display e del porting di LVGL su ESP32.
2.  **Interfaccia Utente (`main/ui/`)**: Contiene il codice generato da EEZ Studio che definisce gli 
    schermi, gli stili e la logica della UI.

---

## Flusso Dettagliato del Programma

### 1. Inizializzazione (`app_main`)
Il punto di ingresso del programma esegue le seguenti operazioni in sequenza:
- **`app_lcd_init()`**: Configura il bus SPI e inizializza il controller del display ST7789.
- **Inizializzazione I2C**: Configura il bus I2C per la comunicazione con il controller touch CST816S.
- **Inizializzazione Touch**: Crea l'istanza del driver touch e la collega al bus I2C.
- **`app_lvgl_init()`**: Inizializza il core di LVGL e crea un task dedicato per la gestione del refresh 
  dello schermo.
- **`app_main_display()`**: Avvia la logica della UI.

### 2. Ciclo Principale
Dopo l'inizializzazione, `app_main` entra in un ciclo infinito:
- Acquisisce il lock della porta LVGL per garantire la thread-safety.
- Chiama **`ui_tick()`** per permettere a EEZ Studio di elaborare la propria logica interna 
  (variabili, animazioni, transizioni).
- Rilascia il lock e attende 10ms.

---

## Descrizione delle Funzioni

### `app_lcd_init()`
- Configura i pin GPIO per la retroilluminazione (BL).
- Inizializza il bus SPI (SCLK, MOSI).
- Crea l'interfaccia IO SPI per il pannello LCD.
- Configura e inizializza il driver specifico **ST7789**.
- Imposta i parametri fisici del display come il mirroring, l'inversione dei colori e il gap 
  dei pixel.

### `app_lvgl_init()`
- Utilizza il componente `esp_lvgl_port` per inizializzare LVGL.
- Crea i buffer di disegno necessari per il rendering.
- Registra il display LCD presso LVGL, definendo la risoluzione (240x280) e le impostazioni 
  di rotazione.

### `app_main_display()`
- Funge da ponte tra il sistema di base e la UI.
- Chiama **`ui_init()`**, la funzione principale generata da EEZ Studio che crea gli oggetti 
  LVGL (bottoni, label, schermi) definiti nel progetto grafico.

### `ui_init()` (in `ui/ui.c`)
- Inizializza le strutture dati di EEZ Studio.
- Crea tutti gli schermi definiti.
- Carica lo schermo principale (`SCREEN_ID_MAIN`).

### `ui_tick()` (in `ui/ui.c`)
- Gestisce l'aggiornamento dinamico degli oggetti UI basato sulle variabili definite in 
  EEZ Studio.

---

## Configurazione Hardware (Pinout)

| Funzione | Pin GPIO |
| :--- | :--- |
| **LCD SCLK** | GPIO 6 |
| **LCD MOSI** | GPIO 7 |
| **LCD RST** | GPIO 8 |
| **LCD DC** | GPIO 4 |
| **LCD CS** | GPIO 5 |
| **LCD BL** | GPIO 15 |
| **TOUCH SCL** | GPIO 10 |
| **TOUCH SDA** | GPIO 11 |
| **TOUCH RST** | GPIO 13 |
| **TOUCH INT** | GPIO 14 |

---

## Note Tecniche
- **Thread Safety**: Tutte le chiamate alle funzioni LVGL (incluse `ui_init` e `ui_tick`) devono 
  essere racchiuse tra `lvgl_port_lock()` e `lvgl_port_unlock()` poiché LVGL gira in un task separato.
- **Risoluzione**: Il display ST7789 utilizzato ha una risoluzione di 240x280 pixel.
