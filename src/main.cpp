/*
  ESP32-CYD LVGL Boilerplate
  ==========================

  Ivan Tarozzi (itarozzi@gmail.com) 2024




  Lib dependencies
  -----------------

  - "XPT2046_Touchscreen" library by Paul Stoffregen to use the Touchscreen - https://github.com/PaulStoffregen/XPT2046_Touchscreen
  - "TFT_eSPI" library by Bodmer to use the TFT display - https://github.com/Bodmer/TFT_eSPI
  - "lvgl" library by LVGL - https://github.com/lvgl/lvgl
*/

#include <Arduino.h>
#include <app_config.h> // Incluye las configuraciones del proyecto
// ============================================================
// Para el servidor NPT público
#include <WiFi.h>
#include <time.h>

#define WIFI_SSID "URGOSAC"
#define WIFI_PASS "SanPellegrino"

// #define WIFI_SSID "MOVISTAR_SC33"
// #define WIFI_PASS "uK7nym27NDmnxdakuPDH"

// Servidores NTP (se usan en orden de prioridad)
#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.google.com"
#define NTP_SERVER3 "es.pool.ntp.org"
// #define NTP_SERVER1 "caca.delavaca.org"
// #define NTP_SERVER2 "caca.delavaca.com"
// #define NTP_SERVER3 "caca.delavaca.ntp.org"

#define NTP_SYNC_TIMEOUT_MS 10000 // Tiempo máximo de espera para sincronizar

#define SW_NAME_REV "MyApp v1.0"

//************* software serial pins used for debug (if serial0 is used for communication) *************
// #define RXPIN 27
// #define TXPIN 22

// ----------------------------------------------------------------
// LED RGB de la CYD de 2,4" — LA ESP32-8048S070C NO TIENE ESTE LED.
// Además GPIO4 y GPIO16 están usados por el panel RGB (B4 y G4),
// así que este bloque debe permanecer deshabilitado en esta placa.
// ----------------------------------------------------------------
// #define CYD_LED_RED 4
// #define CYD_LED_GREEN 16
// #define CYD_LED_BLUE 17

//************* lvgl and UI includes  *************
#include <lvgl.h>
#include "ui/ui.h"
#include "ui/vars.h"
#include "ui/screens.h"
#include "ui/styles.h"
#include "ui/actions.h"
#include "ui/images.h"

//************* TFT display and includes  *************
#include <Arduino_GFX_Library.h>
#include <TAMC_GT911.h>
// #include <TFT_eSPI.h>
// #include <XPT2046_Touchscreen.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480

// ----------------------------------------------------------------
// Panel RGB (ESP32-8048S070C, driver EK9716, pinout Sunton estándar)
// ----------------------------------------------------------------
#define GFX_BL 2 // pin de backlight

Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
    41 /* DE */, 40 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
    14 /* R0 */, 21 /* R1 */, 47 /* R2 */, 48 /* R3 */, 45 /* R4 */,
    9 /* G0 */, 46 /* G1 */, 3 /* G2 */, 8 /* G3 */, 16 /* G4 */, 1 /* G5 */,
    15 /* B0 */, 7 /* B1 */, 6 /* B2 */, 5 /* B3 */, 4 /* B4 */,
    0 /* hsync_polarity */, 180 /* hsync_front_porch */, 30 /* hsync_pulse_width */, 16 /* hsync_back_porch */,
    0 /* vsync_polarity */, 12 /* vsync_front_porch */, 13 /* vsync_pulse_width */, 10 /* vsync_back_porch */);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
    SCREEN_WIDTH, SCREEN_HEIGHT, rgbpanel, 0 /* rotation */, true /* auto_flush */);

// ----------------------------------------------------------------
// Touch capacitivo GT911 (I2C)
// ----------------------------------------------------------------
#define GT911_SDA 19
#define GT911_SCL 20
#define GT911_RST 38
#define GT911_INT -1 // no conectado en esta placa por defecto -> TAMC_GT911 hace polling

TAMC_GT911 ts(
    GT911_SDA,
    GT911_SCL,
    GT911_INT,
    GT911_RST,
    SCREEN_WIDTH,
    SCREEN_HEIGHT);

// ----------------------------------------------------------------
// Buffer de dibujo de LVGL — reservado en PSRAM en setup() (ver más abajo),
// porque con 800px de ancho no interesa usar RAM interna estática.
// ----------------------------------------------------------------
#define DRAW_BUF_LINES 40
#define DRAW_BUF_SIZE (SCREEN_WIDTH * DRAW_BUF_LINES) // en píxeles
lv_color_t *draw_buf = nullptr;

// PRE-SRAD variables
#define GPIO_SRAD 22
const uint16_t SHUTDOWN_TIME = 120;        // segundos
uint16_t pre_srad_countdown = 3 * 60 + 30; // 210s (valor de prueba)

// Hora "hh:mm:ss" en que se solicitó el SRAD y en que se produjo la
// desconexión, calculadas una vez en PRE_SRAD y reutilizadas después
// en la pantalla SRAD, para no tener que releer el RTC (y así no
// desajustar los segundos respecto a lo mostrado en PRE_SRAD).
char hora_solicitud_srad[9] = "";
char hora_desconexion_srad[9] = "";

// SRAD variables
uint32_t srad_countdown = 0; // segundos que faltan para fin de SRAD
uint8_t srad_end_hour = 0;   // hora de finalización de SRAD

// END SRAD variables
#define GPIO_ENDSRAD 27             // Salir prematuramente del SRAD
uint16_t post_srad_duration_s = 20; // segundos en POST_SRAD antes de pasar a SHOW_CLOCK
uint16_t post_srad_countdown = 0;

// If logging is enabled, it will inform the user about what is happening in the library
void log_print(lv_log_level_t level, const char *buf)
{
  LV_UNUSED(level);
  debugPrintln("%s", buf);
  // Serial.flush();
}

// Get the Touchscreen data (GT911 capacitivo, coordenadas ya en pixeles reales)
void touchscreen_read(lv_indev_t *indev, lv_indev_data_t *data)
{
  ts.read();

  if (ts.isTouched)
  {
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = ts.points[0].x;
    data->point.y = ts.points[0].y;
#if DEBUG
    debugPrintln("X = %d  Y = %d", ts.points[0].x, ts.points[0].y);
#endif
  }
  else
  {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

// Flush callback de LVGL 9 hacia Arduino_GFX (debe estar a nivel de fichero,
// C++ no permite funciones anidadas dentro de otra función)
void my_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);

  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);

  lv_display_flush_ready(disp);
}

// lvgl initialization for esp32 board
void lv_init_esp32(void)
{
  // Register print function for debugging
  // lv_log_register_print_cb(log_print); *** ... *** elimino los logs

  // --- Display: panel RGB via Arduino_GFX ---
  if (!gfx->begin())
  {
    debugPrintln("gfx->begin() failed!");
  }
  gfx->fillScreen(RGB565_BLACK);

  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);

  // --- Touch: GT911 ---
  ts.begin(); // si no detecta toques, probar pasando GT911_ADDR2 como argumento
  ts.setRotation(ROTATION_NORMAL);

  // --- Reservar el buffer de dibujo de LVGL en PSRAM ---
  draw_buf = (lv_color_t *)heap_caps_malloc(DRAW_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
  if (draw_buf == nullptr)
  {
    debugPrintln("ERROR: no se pudo reservar draw_buf en PSRAM");
  }

  lv_display_t *disp =
      lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);

  lv_display_set_buffers(
      disp,
      draw_buf,
      NULL,
      DRAW_BUF_SIZE * sizeof(lv_color_t),
      LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_display_set_flush_cb(disp, my_flush_cb);

  // Nota: no hace falta llamar a lv_tick_set_cb() aqui - lv_conf.h ya tiene
  // LV_TICK_CUSTOM=1 con millis() como fuente de tiempo (LV_TICK_CUSTOM_SYS_TIME_EXPR).

  // Initialize an LVGL input device object (Touchscreen)
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);

  // Set the callback function to read Touchscreen input
  lv_indev_set_read_cb(indev, touchscreen_read);

  // you can define TFT_INVERTED as compiler param in platformio.ini
  // #ifdef TFT_INVERTED
  //  gfx->invertDisplay(true);
  // #else
  //  gfx->invertDisplay(false);
  // #endif
}

// ============================================================
// Obtener y mantener la hora

char buf_hora[16];
char buf_fecha[16];

// Ajuste automático de la hora en España
static int getSpainOffset()
{
  // Último domingo de marzo: empieza verano (UTC+2)
  // Último domingo de octubre: empieza invierno (UTC+1)
  time_t now = time(nullptr);
  struct tm t;
  gmtime_r(&now, &t);

  int month = t.tm_mon + 1; // 1-12

  if (month < 3 || month > 10)
    return 3600; // invierno
  if (month > 3 && month < 10)
    return 7200; // verano

  // Calcular último domingo del mes (marzo y octubre tienen 31 días)
  int lastSunday = t.tm_mday - t.tm_wday;
  while (lastSunday + 7 <= 31)
    lastSunday += 7;

  if (month == 3) // marzo: verano desde último domingo
    return (t.tm_mday > lastSunday || (t.tm_mday == lastSunday && t.tm_hour >= 1)) ? 7200 : 3600;
  else // octubre: invierno desde último domingo
    return (t.tm_mday > lastSunday || (t.tm_mday == lastSunday && t.tm_hour >= 1)) ? 3600 : 7200;
}

bool getRTCTime(struct tm &timeInfo)
{
  // Timeout corto (no el de 5000ms por defecto): aquí solo queremos leer
  // el valor ACTUAL del reloj del sistema, esté ya sincronizado por NTP o
  // no. Con el timeout por defecto, si el reloj todavía no es "válido"
  // (p.ej. justo tras invalidarlo a época 0 antes de un intento de NTP),
  // getLocalTime() se queda reintentando en un bucle bloqueante hasta
  // agotar esos 5s, congelando la UI innecesariamente.
  return getLocalTime(&timeInfo, 10);
}

void updateClock()
{
  struct tm t;
  if (getRTCTime(t))
  {
    snprintf(buf_hora, sizeof(buf_hora), "%02d:%02d:%02d",
             t.tm_hour, t.tm_min, t.tm_sec);
    snprintf(buf_fecha, sizeof(buf_fecha), "%02d/%02d/%04d",
             t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
  }
}
// ============================================================

// true si la hora se ha puesto manualmente (NTP no disponible), false si viene de NTP
bool relojAjustadoManualmente = true;

// Muestra/oculta y activa/desactiva el botón "ir a SetClock" de la pantalla main
// según si el reloj se ha ajustado manualmente o por NTP
void updateSetClockButtonState()
{
  if (relojAjustadoManualmente)
  {
    lv_obj_clear_flag(objects.btn_scnmain_setclock, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_state(objects.btn_scnmain_setclock, LV_STATE_DISABLED);
  }
  else
  {
    lv_obj_add_flag(objects.btn_scnmain_setclock, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_state(objects.btn_scnmain_setclock, LV_STATE_DISABLED);
  }
}

// ==== MÁQUINA DE ESTADOS ====================================
unsigned long wifi_start_ms = 0;
uint32_t wifi_synced_ms = 0;

enum EstadoPrograma
{
  IDLE,
  CONNECTING_WIFI,
  SYNCING_NTP,
  SET_CLOCK,
  SHOW_CLOCK,
  PRE_SRAD,
  SRAD,
  POST_SRAD
};
EstadoPrograma estadoActual = IDLE;

// ============================================================

// ==== RUTINA DE INTERRUPCIÓN TEMPORIZADA ====================

// Variable compartida entre ISR y loop
volatile bool timer1s_flag = false;

// Handle del timer
hw_timer_t *timer1s = nullptr;

// ISR: sólo activa el flag
void IRAM_ATTR onTimer1s()
{
  timer1s_flag = true;
}

// Inicialización del timer
void initTimer1s()
{
  timer1s = timerBegin(1000000); // frecuencia del timer: 1 MHz
  timerAttachInterrupt(timer1s, &onTimer1s);
  timerAlarm(timer1s, 1000000, true, 0);
}

void doTimer1s()
{
  // Todo lo que haya que hacer cada segundo
  if (timer1s_flag)
  {
    timer1s_flag = false;
    updateClock();
    // ...
  }
}

// ============================================================

// ==== CALLBACK DE LOS BOTONES ===============================

// Variable global para rastrear el spinbox con foco
lv_obj_t *focused_spinbox = nullptr;

void btn_incr_set_clock_handler(lv_event_t *e)
{
  if (focused_spinbox)
    lv_spinbox_increment(focused_spinbox);
}

void btn_decr_set_clock_handler(lv_event_t *e)
{
  if (focused_spinbox)
    lv_spinbox_decrement(focused_spinbox);
}

uint32_t cursor_pos = 0;

void btn_right_set_clock_handler(lv_event_t *e)
{
  if (!focused_spinbox)
    return;
  if (cursor_pos > 0)
    cursor_pos--;
  lv_spinbox_set_cursor_pos(focused_spinbox, cursor_pos);
}

void btn_left_set_clock_handler(lv_event_t *e)
{
  if (!focused_spinbox)
    return;
  uint32_t digits = lv_spinbox_get_digit_count(focused_spinbox);
  if (cursor_pos < digits - 1)
    cursor_pos++;
  lv_spinbox_set_cursor_pos(focused_spinbox, cursor_pos);
}

void btn_ok_set_clock_handler(lv_event_t *e)
{
  // Leer valores de los spinboxes
  struct tm t = {};
  t.tm_mday = lv_spinbox_get_value(objects.spnbx_scn_set_clock_day);
  t.tm_mon = lv_spinbox_get_value(objects.spnbx_scn_set_clock_month) - 1; // tm_mon: 0-11
  t.tm_year = lv_spinbox_get_value(objects.spnbx_scn_set_clock_year) - 1900;
  t.tm_hour = lv_spinbox_get_value(objects.spnbx_scn_set_clock_hour);
  t.tm_min = lv_spinbox_get_value(objects.spnbx_scn_set_clock_min);
  t.tm_sec = lv_spinbox_get_value(objects.spnbx_scn_set_clock_sec);
  t.tm_isdst = -1;

  // Convertir a time_t y escribir al RTC interno
  time_t epoch = mktime(&t);
  struct timeval tv = {.tv_sec = epoch, .tv_usec = 0};
  settimeofday(&tv, nullptr);

  debugPrint("RTC ajustado: %02d/%02d/%04d %02d:%02d:%02d\n",
             t.tm_mday, t.tm_mon + 1, t.tm_year + 1900,
             t.tm_hour, t.tm_min, t.tm_sec);

  estadoActual = SHOW_CLOCK;
  lv_scr_load(objects.main);
  debugPrintln("Cambiando a: SHOW_CLOCK");
}

// Estilos para los spin box
void set_focused_spinbox(lv_obj_t *new_focus)
{
  // Quitar estilo del anterior
  if (focused_spinbox)
  {
    remove_style_spnbx_focused(focused_spinbox);
    add_style_spnbx_unfocused(focused_spinbox);
  }

  cursor_pos = 0;

  // Aplicar estilo al nuevo
  focused_spinbox = new_focus;
  if (focused_spinbox)
  {
    remove_style_spnbx_unfocused(focused_spinbox);
    add_style_spnbx_focused(focused_spinbox);
  }
}

// Callback genérico para cualquier spinbox
void spinbox_focus_handler(lv_event_t *e)
{
  set_focused_spinbox((lv_obj_t *)lv_event_get_target(e));
}

// Carga la pantalla de ajuste de fecha/hora, dejando los spinbox
// inicializados con la fecha/hora que tiene actualmente el RTC
// (aunque esa hora no sea correcta, p.ej. recién encendido sin NTP).
void enter_SET_CLOCK()
{
  struct tm t;
  if (getRTCTime(t))
  {
    lv_spinbox_set_value(objects.spnbx_scn_set_clock_day, t.tm_mday);
    lv_spinbox_set_value(objects.spnbx_scn_set_clock_month, t.tm_mon + 1);
    lv_spinbox_set_value(objects.spnbx_scn_set_clock_year, t.tm_year + 1900);
    lv_spinbox_set_value(objects.spnbx_scn_set_clock_hour, t.tm_hour);
    lv_spinbox_set_value(objects.spnbx_scn_set_clock_min, t.tm_min);
    lv_spinbox_set_value(objects.spnbx_scn_set_clock_sec, t.tm_sec);
  }

  // Foco siempre en el primer campo al entrar
  set_focused_spinbox(objects.spnbx_scn_set_clock_day);

  lv_scr_load(objects.scn_set_clock);
  estadoActual = SET_CLOCK;
  debugPrintln("Cambiando a: SET_CLOCK");
}

// Botón de la pantalla main para ir a ajustar la hora manualmente
void btn_goto_set_clock_handler(lv_event_t *e)
{
  // Reiniciar de verdad el intento de conexión WiFi (igual que en setup())
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Reiniciar los cronómetros de timeout; si no, el timeout de 15s
  // (heredado del arranque) ya estaría vencido y se saltaría a SET_CLOCK
  // casi de inmediato.
  wifi_start_ms = millis();
  wifi_synced_ms = 0;
  lv_scr_load(objects.scn_wifi_conn);
  lv_label_set_text(objects.lbl_scn_wi_fi_conn_network, WIFI_SSID);
  // lv_label_set_text(objects.lbl_scn_set_clock_title_1, "Ajuste manual de la hora");
  // set_focused_spinbox(objects.spnbx_scn_set_clock_day);
  estadoActual = CONNECTING_WIFI;
  debugPrintln("Cambiando a: CONNECTING_WIFI (manual desde pantalla principal)");
}

// ============================================================

void setup()
{
  Serial.begin(115200);
  String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  debugPrintln(SW_NAME_REV);
  debugPrintln("%s", LVGL_Arduino);

  // LED RGB de la CYD de 2,4" eliminado: la ESP32-8048S070C no lo tiene,
  // y sus pines (4, 16) están ocupados por el panel RGB (B4, G4).

  // GPIO22: entrada con pull-up interno para señal SRAD
  pinMode(GPIO_SRAD, INPUT_PULLUP);
  // GPIO27: entrada con pull-up interno para salida prematura de SRAD
  pinMode(GPIO_ENDSRAD, INPUT_PULLUP);

  // Start LVGL
  lv_init();
  // Init TFT and Touch for esp32
  lv_init_esp32();
  // Integrate EEZ Studio GUI
  ui_init();

  updateSetClockButtonState();

  // Eventos lvgl
  lv_obj_add_event_cb(objects.btn_scnmain_setclock, btn_goto_set_clock_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.btn_scn_set_clock_incr, btn_incr_set_clock_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.btn_scn_set_clock_decr, btn_decr_set_clock_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.btn_scn_set_clock_ok, btn_ok_set_clock_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.btn_scn_set_clock_left, btn_left_set_clock_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.btn_scn_set_clock_right, btn_right_set_clock_handler, LV_EVENT_CLICKED, NULL);

  // Eventos de los spin box de ajustar la hora
  lv_obj_add_event_cb(objects.spnbx_scn_set_clock_day, spinbox_focus_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.spnbx_scn_set_clock_month, spinbox_focus_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.spnbx_scn_set_clock_year, spinbox_focus_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.spnbx_scn_set_clock_hour, spinbox_focus_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.spnbx_scn_set_clock_min, spinbox_focus_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.spnbx_scn_set_clock_sec, spinbox_focus_handler, LV_EVENT_CLICKED, NULL);

  // Estilo inicial: todos sin foco
  add_style_spnbx_unfocused(objects.spnbx_scn_set_clock_day);
  add_style_spnbx_unfocused(objects.spnbx_scn_set_clock_month);
  add_style_spnbx_unfocused(objects.spnbx_scn_set_clock_year);
  add_style_spnbx_unfocused(objects.spnbx_scn_set_clock_hour);
  add_style_spnbx_unfocused(objects.spnbx_scn_set_clock_min);
  add_style_spnbx_unfocused(objects.spnbx_scn_set_clock_sec);

  // Foco inicial en el primero
  set_focused_spinbox(objects.spnbx_scn_set_clock_day);

  initTimer1s();

  // Arrancar WiFi sin bloquear
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  wifi_start_ms = millis();
  estadoActual = CONNECTING_WIFI;
  lv_scr_load(objects.scn_wifi_conn);
  lv_label_set_text(objects.lbl_scn_wi_fi_conn_network, WIFI_SSID);
  debugPrintln("Cambiando a: CONNECTING_WIFI");
}

// ==== ESTADOS DE estadoAcual ================================
// DETECCIÓN SEÑAL SRAD

void start_blink_label(lv_obj_t *label)
{
  // Por si el label ya tenía una animación de parpadeo en marcha
  // (p.ej. un retrigger de SRAD antes de que acabe el ciclo anterior),
  // la borramos primero para no acumular animaciones duplicadas.
  lv_anim_del(label, NULL);
  lv_obj_set_style_opa(label, LV_OPA_COVER, LV_PART_MAIN);

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, label);
  lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
  lv_anim_set_duration(&a, 500);          // 500ms por semiciclo
  lv_anim_set_playback_duration(&a, 500); // 500ms de vuelta
  lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
  lv_anim_set_exec_cb(&a, [](void *obj, int32_t val)
                      { lv_obj_set_style_opa((lv_obj_t *)obj, val, LV_PART_MAIN); });
  lv_anim_start(&a);
}

void checkSRADTrigger()
{
  if (digitalRead(GPIO_SRAD) == LOW)
  {
    pre_srad_countdown = 3 * 60 + 30; // reiniciar cuenta atrás
    lv_scr_load(objects.scn_pre_srad);
    lv_label_set_text(objects.lbl_scn_pre_srad_message, "SRAD SOLICITADO");
    lv_obj_set_style_bg_color(objects.scn_pre_srad, lv_color_hex(0x00FF00), LV_PART_MAIN); // fondo verde
    start_blink_label(objects.lbl_scn_pre_srad_message);

    // Hora de entrada al estado, fijada UNA sola vez (no se toca
    // luego en do_PRE_SRAD, así que no parpadea ni se actualiza).
    struct tm t;
    if (getRTCTime(t))
    {
      snprintf(hora_solicitud_srad, sizeof(hora_solicitud_srad), "%02d:%02d:%02d",
               t.tm_hour, t.tm_min, t.tm_sec);
      char buf_hora_srad[16];
      snprintf(buf_hora_srad, sizeof(buf_hora_srad), "A LAS %s", hora_solicitud_srad);
      lv_label_set_text(objects.lbl_scn_pre_srad_hora_srad, buf_hora_srad);
    }

    // Los labels de desconexión permanecen ocultos hasta SHUTDOWN_TIME.
    // Se ocultan aquí también por si se retriggerea el SRAD y habían
    // quedado visibles de un ciclo anterior.
    lv_anim_del(objects.lbl_scn_pre_srad_desconexion, NULL);
    lv_obj_add_flag(objects.lbl_scn_pre_srad_desconexion, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.lbl_scn_pre_srad_hora_desconexion, LV_OBJ_FLAG_HIDDEN);

    estadoActual = PRE_SRAD;
    debugPrintln("Cambiando a: PRE_SRAD");
  }
}

void enter_POST_SRAD()
{
  // Cargar pantalla y fijar texto con parpadeo
  lv_scr_load(objects.scn_post_srad);
  lv_label_set_text(objects.lbl_scn_post_srad_message, "FIN SRAD");
  start_blink_label(objects.lbl_scn_post_srad_message);

  // Mostrar fecha y hora actuales
  updateClock();
  lv_label_set_text(objects.lbl_scnpost_srad_date, buf_fecha);
  lv_label_set_text(objects.lbl_scnpost_srad_time, buf_hora);

  // Reiniciar cuenta atrás de permanencia en POST_SRAD
  post_srad_countdown = post_srad_duration_s;

  estadoActual = POST_SRAD;
  debugPrintln("Cambiando a: POST_SRAD");
}

void checkENDSRADTrigger()
{
  if (digitalRead(GPIO_ENDSRAD) == LOW)
  {
    // pre_srad_countdown = 3 * 60 + 30; // reiniciar cuenta atrás
    lv_scr_load(objects.scn_post_srad);
    /*lv_label_set_text(objects.lbl_scn_pre_srad_message, "SRAD SOLICITADO");
    lv_obj_set_style_bg_color(objects.scn_pre_srad, lv_color_hex(0x00FF00), LV_PART_MAIN); // fondo verde
    start_blink_label(objects.lbl_scn_pre_srad_message);*/
    estadoActual = POST_SRAD;
    debugPrintln("Cambiando a: POST_SRAD");
    enter_POST_SRAD();
  }
}

void enter_SRAD()
{
  struct tm t;
  getRTCTime(t);

  // Segundos que faltan para que acabe la hora actual
  uint32_t secs_to_next_hour = (59 - t.tm_min) * 60 + (60 - t.tm_sec);

  // Cuenta atrás = tiempo hasta fin de hora actual + 1 hora completa
  srad_countdown = secs_to_next_hour + 3600;

  // Hora final = hora actual + 2 (siempre en punto)
  srad_end_hour = (t.tm_hour + 2) % 24;

  // Formatear label hora final
  char buf_end[24];
  snprintf(buf_end, sizeof(buf_end), "FIN DE SRAD: %02d:00:00", srad_end_hour);

  // Cargar pantalla y establecer textos iniciales
  lv_scr_load(objects.scn_srad);
  lv_label_set_text(objects.lbl_scn_srad_end_time, buf_end);
  lv_label_set_text(objects.lbl_scn_srad_message, "SRAD ACTIVO");

  // Formatear countdown inicial
  uint32_t h = srad_countdown / 3600;
  uint32_t m = (srad_countdown % 3600) / 60;
  uint32_t s = srad_countdown % 60;
  char buf_cd[10];
  snprintf(buf_cd, sizeof(buf_cd), "%02d:%02d:%02d", h, m, s);
  lv_label_set_text(objects.lbl_scn_srad_countdown, buf_cd);

  // Labels informativos fijos (no parpadean, se fijan una sola vez aquí
  // y do_SRAD() nunca vuelve a tocarlos).
  char buf_solicitud[32];
  snprintf(buf_solicitud, sizeof(buf_solicitud), "SOLICITADO A LAS %s", hora_solicitud_srad);
  lv_label_set_text(objects.lbl_scn_srad_solicitud_time, buf_solicitud);

  char buf_desconexion[32];
  snprintf(buf_desconexion, sizeof(buf_desconexion), "DESCONEXION A LAS %s", hora_desconexion_srad);
  lv_label_set_text(objects.lbl_scn_srad_desconexion_time, buf_desconexion);

  // Duración = valor inicial del contador descendente (buf_cd, recién
  // calculado arriba, antes de que empiece a decrementarse).
  char buf_duracion[24];
  snprintf(buf_duracion, sizeof(buf_duracion), "DURACION: %s", buf_cd);
  lv_label_set_text(objects.lbl_scn_srad_duracion_time, buf_duracion);

  // Hora de entrada a la pantalla SRAD (t ya se leyó del RTC al
  // principio de esta función). Fijo, no parpadea.
  char buf_inicio[24];
  snprintf(buf_inicio, sizeof(buf_inicio), "INICIO A LAS %02d:%02d:%02d",
           t.tm_hour, t.tm_min, t.tm_sec);
  lv_label_set_text(objects.lbl_scn_srad_inicio_time, buf_inicio);

  // Blink en "SRAD ACTIVO"
  start_blink_label(objects.lbl_scn_srad_message);

  estadoActual = SRAD;
  debugPrintln("Cambiando a: SRAD");
}

void do_IDLE()
{
}

void do_CONNECTING_WIFI()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    if (wifi_synced_ms == 0)
    {
      debugPrintln("WiFi conectado, esperando 2s...");
      wifi_synced_ms = millis();
    }

    // Ya conectado, esperar 2 segundos
    if (millis() - wifi_synced_ms >= 2000)
    {
      wifi_synced_ms = 0;

      // Invalidar el reloj del sistema (época 0 = año 1970) antes de
      // lanzar la sincronización NTP. Si no lo hacemos, y el reloj ya
      // tenía una fecha válida (p.ej. por un ajuste manual previo),
      // getLocalTime() en do_SYNCING_NTP() puede devolver "true" de
      // forma prematura usando esa hora antigua (el sync SNTP real es
      // asíncrono y tarda en llegar), aplicando el offset de España
      // sobre un valor incorrecto. Poco después, el SNTP real
      // sobrescribe el reloj con la hora UTC real "por su cuenta",
      // perdiendo el ajuste horario.
      struct timeval tv_invalid = {0, 0};
      settimeofday(&tv_invalid, nullptr);
      configTime(0, 0, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);
      wifi_start_ms = millis(); // reinicia el timeout para NTP
      lv_scr_load(objects.scn_rtc_sync);
      estadoActual = SYNCING_NTP;
      debugPrintln("Cambiando a: SYNCING_NTP");
    }
    // else: seguir en este estado, el spinner sigue girando
  }
  else if (millis() - wifi_start_ms > 15000)
  {
    debugPrintln("WiFi: timeout");
    wifi_synced_ms = 0;
    lv_label_set_text(objects.lbl_scn_set_clock_title_1, "WiFi no disponible");
    enter_SET_CLOCK();
  }
  lv_task_handler(); // Para la animación del spinner
}

static uint32_t ntp_synced_ms = 0;

void do_SYNCING_NTP()
{
  struct tm t;

  if (ntp_synced_ms == 0 && getLocalTime(&t))
  {
    time_t utcNow = time(nullptr);
    time_t localNow = utcNow + getSpainOffset();
    struct timeval tv = {.tv_sec = localNow, .tv_usec = 0};
    settimeofday(&tv, nullptr);
    debugPrintln("NTP sincronizado, esperando 2s...");
    ntp_synced_ms = millis();
  }

  if (ntp_synced_ms > 0)
  {
    // Ya sincronizado, esperar 2 segundos
    if (millis() - ntp_synced_ms >= 2000)
    {
      ntp_synced_ms = 0;
      relojAjustadoManualmente = false;
      updateSetClockButtonState();
      lv_scr_load(objects.main);
      estadoActual = SHOW_CLOCK;
      debugPrintln("Cambiando a: SHOW_CLOCK");
    }
    // else: seguir en este estado, el spinner sigue girando
  }
  else if (millis() - wifi_start_ms > NTP_SYNC_TIMEOUT_MS)
  {
    debugPrintln("NTP: timeout");
    lv_label_set_text(objects.lbl_scn_set_clock_title_1, "NTP Server no disponible");
    enter_SET_CLOCK();
  }
  lv_task_handler(); // Para la animación del spiner
}

void do_SET_CLOCK()
{
  // Nothing to do
}

void do_SHOW_CLOCK()
{
  doTimer1s();
  lv_label_set_text(objects.lbl_scnmain_time, buf_hora);
  lv_label_set_text(objects.lbl_scnmain_date, buf_fecha);
  checkSRADTrigger();
}

void do_PRE_SRAD()
{
  if (timer1s_flag)
  {
    timer1s_flag = false;

    // Actualizar countdown
    if (pre_srad_countdown > 0)
      pre_srad_countdown--;

    // Formatear mm:ss y actualizar label
    uint16_t mins = pre_srad_countdown / 60;
    uint16_t secs = pre_srad_countdown % 60;
    char buf[8];
    snprintf(buf, sizeof(buf), "%02d:%02d", mins, secs);
    lv_label_set_text(objects.lbl_scn_pre_srad_countdown, buf);

    // Cambio de mensaje y fondo al llegar a SHUTDOWN_TIME
    if (pre_srad_countdown == SHUTDOWN_TIME)
    {
      lv_obj_set_style_bg_color(objects.scn_pre_srad, lv_color_hex(0xFFA500), LV_PART_MAIN);

      // lbl_scn_pre_srad_message mantiene "SRAD SOLICITADO" y su
      // parpadeo; el aviso de desconexión ahora vive en su propio label.
      lv_label_set_text(objects.lbl_scn_pre_srad_desconexion, "DESCONEXION");
      lv_obj_clear_flag(objects.lbl_scn_pre_srad_desconexion, LV_OBJ_FLAG_HIDDEN);
      start_blink_label(objects.lbl_scn_pre_srad_desconexion);

      // Hora a la que se ha alcanzado SHUTDOWN_TIME, fijada una sola
      // vez (no parpadea).
      struct tm t_desc;
      if (getRTCTime(t_desc))
      {
        snprintf(hora_desconexion_srad, sizeof(hora_desconexion_srad), "%02d:%02d:%02d",
                 t_desc.tm_hour, t_desc.tm_min, t_desc.tm_sec);
        char buf_hora_desconexion[16];
        snprintf(buf_hora_desconexion, sizeof(buf_hora_desconexion), "A LAS %s", hora_desconexion_srad);
        lv_label_set_text(objects.lbl_scn_pre_srad_hora_desconexion, buf_hora_desconexion);
      }
      lv_obj_clear_flag(objects.lbl_scn_pre_srad_hora_desconexion, LV_OBJ_FLAG_HIDDEN);
    }

    // Transición a SRAD al llegar a cero
    if (pre_srad_countdown == 0)
    {
      enter_SRAD();
    }
  }
}

void do_SRAD()
{
  if (timer1s_flag)
  {
    timer1s_flag = false;

    if (srad_countdown > 0)
      srad_countdown--;

    // Actualizar label cuenta atrás
    uint32_t h = srad_countdown / 3600;
    uint32_t m = (srad_countdown % 3600) / 60;
    uint32_t s = srad_countdown % 60;
    char buf[10];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", h, m, s);
    lv_label_set_text(objects.lbl_scn_srad_countdown, buf);

    // Transición a POST_SRAD al llegar a cero
    if (srad_countdown == 0)
    {
      lv_scr_load(objects.scn_post_srad);
      estadoActual = POST_SRAD;
      debugPrintln("Cambiando a: POST_SRAD");
      enter_POST_SRAD();
    }
  }
  checkENDSRADTrigger();
}

void do_POST_SRAD()
{
  if (timer1s_flag)
  {
    timer1s_flag = false;
    updateClock();

    // Actualizar fecha y hora igual que en SHOW_CLOCK
    lv_label_set_text(objects.lbl_scnpost_srad_date, buf_fecha);
    lv_label_set_text(objects.lbl_scnpost_srad_time, buf_hora);

    // Cuenta atrás de permanencia en POST_SRAD
    if (post_srad_countdown > 0)
      post_srad_countdown--;

    // Transición a SHOW_CLOCK al agotarse el tiempo
    if (post_srad_countdown == 0)
    {
      updateSetClockButtonState();
      lv_scr_load(objects.main);
      estadoActual = SHOW_CLOCK;
      debugPrintln("Cambiando a: SHOW_CLOCK");
    }
  }
}
// ============================================================

void loop()
{
  switch (estadoActual)
  {
  case IDLE:
    do_IDLE();
    break;
  case CONNECTING_WIFI:
    do_CONNECTING_WIFI();
    break;
  case SYNCING_NTP:
    do_SYNCING_NTP();
    break;
  case SET_CLOCK:
    do_SET_CLOCK();
    break;
  case SHOW_CLOCK:
    do_SHOW_CLOCK();
    break;
  case PRE_SRAD:
    do_PRE_SRAD();
    break;
  case SRAD:
    do_SRAD();
    break;
  case POST_SRAD:
    do_POST_SRAD();
    break;
  default:
    break;
  }

  lv_task_handler();
  delay(5);
}