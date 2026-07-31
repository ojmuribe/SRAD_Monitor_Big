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
#include <LittleFS.h>

#define WIFI_SSID "URGOSAC"
#define WIFI_PASS "SanPellegrino"

// Servidores NTP (se usan en orden de prioridad)
#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.google.com"
#define NTP_SERVER3 "es.pool.ntp.org"

#define NTP_SYNC_TIMEOUT_MS 10000 // Tiempo máximo de espera para sincronizar

#define SW_NAME_REV "MyApp v1.0"

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
    0 /* hsync_polarity */, 210 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 43 /* hsync_back_porch */,
    0 /* vsync_polarity */, 22 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 12 /* vsync_back_porch */,
    1 /* pclk_active_neg */, 16000000 /* prefer_speed */, false /* useBigEndian */,
    0 /* de_idle_high */, 0 /* pclk_idle_high */,
    10 * SCREEN_WIDTH /* bounce_buffer_size_px — 10 líneas de bounce buffer */);

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
// Buffer de dibujo de LVGL — reservado en PSRAM en setup()
// ----------------------------------------------------------------
#define DRAW_BUF_LINES 40
#define DRAW_BUF_SIZE (SCREEN_WIDTH * DRAW_BUF_LINES) // en píxeles
lv_color_t *draw_buf = nullptr;

// PRE-SRAD variables
#define GPIO_SRAD 12
const uint16_t SHUTDOWN_TIME = 120;        // segundos
uint16_t pre_srad_countdown = 3 * 60 + 30; // 210s (valor de prueba)

// Variables para almacenar las marcas de tiempo del SRAD
char hora_solicitud_srad[9] = "";
char hora_desconexion_srad[9] = "";

char srad_req_date[12] = "--/--/----";
char srad_req_time[10] = "--:--:--";

char srad_break_date[12] = "--/--/----";
char srad_break_time[10] = "--:--:--";
bool has_srad_break = false;

char srad_start_date[12] = "--/--/----";
char srad_start_time[10] = "--:--:--";

char srad_stimated_end_date[12] = "--/--/----";
char srad_stimated_end_time[10] = "--:--:--";
char srad_stimated_duration[16] = "--:--:--";

char srad_real_end_date[12] = "--/--/----";
char srad_real_end_time[10] = "--:--:--";
char srad_real_duration[16] = "--:--:--";

#define SRAD_STATS_FILE "/srad_stats.txt"

char bk_srad_req_date[12] = "--/--/----";
char bk_srad_req_time[10] = "--:--:--";

char bk_srad_break_date[12] = "--/--/----";
char bk_srad_break_time[10] = "--:--:--";

char bk_srad_start_date[12] = "--/--/----";
char bk_srad_start_time[10] = "--:--:--";

char bk_srad_stimated_end_date[12] = "--/--/----";
char bk_srad_stimated_end_time[10] = "--:--:--";
char bk_srad_stimated_duration[16] = "--:--:--";

char bk_srad_real_end_date[12] = "--/--/----";
char bk_srad_real_end_time[10] = "--:--:--";
char bk_srad_real_duration[16] = "--:--:--";

time_t time_srad_start = 0; // Para calcular la duración real en segundos

// SRAD variables
uint32_t srad_countdown = 0; // segundos que faltan para fin de SRAD
uint8_t srad_end_hour = 0;   // hora de finalización de SRAD

// Modo "cuenta adelante": la cuenta atrás llegó a 0 pero GPIO_SRAD sigue activo
bool srad_overtime = false;
uint32_t srad_overtime_s = 0; // segundos transcurridos en cuenta adelante

// END SRAD variables
#define GPIO_ENDSRAD 13             // Salir prematuramente del SRAD
uint16_t post_srad_duration_s = 20; // segundos en POST_SRAD antes de pasar a SHOW_CLOCK
uint16_t post_srad_countdown = 0;

// ============================================================
// Prototipos (forward declarations)
// Evitan errores de "was not declared in this scope" al reordenar,
// renombrar o mover funciones más abajo en este mismo archivo.
// ============================================================
void log_print(lv_log_level_t level, const char *buf);
void touchscreen_read(lv_indev_t *indev, lv_indev_data_t *data);
void my_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
void lv_init_esp32(void);
bool getRTCTime(struct tm &timeInfo);
void updateClock();
void updateSetClockButtonState();
void onTimer1s();
void initTimer1s();
void doTimer1s();
void btn_incr_set_clock_handler(lv_event_t *e);
void btn_decr_set_clock_handler(lv_event_t *e);
void btn_right_set_clock_handler(lv_event_t *e);
void btn_left_set_clock_handler(lv_event_t *e);
void btn_ok_set_clock_handler(lv_event_t *e);
void set_focused_spinbox(lv_obj_t *new_focus);
void spinbox_focus_handler(lv_event_t *e);
void enter_SET_CLOCK();
void btn_goto_set_clock_handler(lv_event_t *e);
void enter_LAST_SRAD();
void btn_main_info_handler(lv_event_t *e);
void btn_scn_last_srad_back_handler(lv_event_t *e);
void loadSradStatsFromLittleFS();
void start_blink_label(lv_obj_t *label);
void checkSRADTrigger();
void saveSradStatsToLittleFS();
void enter_POST_SRAD();
void checkENDSRADTrigger();
void enter_SRAD();
void do_IDLE();
void do_CONNECTING_WIFI();
void do_SYNCING_NTP();
void do_SET_CLOCK();
void do_SHOW_CLOCK();
void do_PRE_SRAD();
void do_SRAD();
void do_POST_SRAD();
void do_LAST_SRAD();

void log_print(lv_log_level_t level, const char *buf)
{
  LV_UNUSED(level);
  debugPrintln("%s", buf);
}

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

void my_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);

  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);

  lv_display_flush_ready(disp);
}

void lv_init_esp32(void)
{
  if (!gfx->begin())
  {
    debugPrintln("gfx->begin() failed!");
  }
  gfx->fillScreen(RGB565_BLACK);

  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);

  ts.begin();
  ts.setRotation(ROTATION_INVERTED);

  draw_buf = (lv_color_t *)heap_caps_malloc(DRAW_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
  if (draw_buf == nullptr)
  {
    debugPrintln("ERROR: no se pudo reservar draw_buf en PSRAM");
  }

  lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);

  lv_display_set_buffers(
      disp,
      draw_buf,
      NULL,
      DRAW_BUF_SIZE * sizeof(lv_color_t),
      LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_display_set_flush_cb(disp, my_flush_cb);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touchscreen_read);
}

// ============================================================
// Obtener y mantener la hora

char buf_hora[16];
char buf_fecha[16];

static int getSpainOffset()
{
  time_t now = time(nullptr);
  struct tm t;
  gmtime_r(&now, &t);

  int month = t.tm_mon + 1; // 1-12

  if (month < 3 || month > 10)
    return 3600; // invierno
  if (month > 3 && month < 10)
    return 7200; // verano

  int lastSunday = t.tm_mday - t.tm_wday;
  while (lastSunday + 7 <= 31)
    lastSunday += 7;

  if (month == 3)
    return (t.tm_mday > lastSunday || (t.tm_mday == lastSunday && t.tm_hour >= 1)) ? 7200 : 3600;
  else
    return (t.tm_mday > lastSunday || (t.tm_mday == lastSunday && t.tm_hour >= 1)) ? 3600 : 7200;
}

bool getRTCTime(struct tm &timeInfo)
{
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

bool relojAjustadoManualmente = true;

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
  POST_SRAD,
  LAST_SRAD
};
EstadoPrograma estadoActual = IDLE;

// ==== RUTINA DE INTERRUPCIÓN TEMPORIZADA ====================
volatile bool timer1s_flag = false;
hw_timer_t *timer1s = nullptr;

void IRAM_ATTR onTimer1s()
{
  timer1s_flag = true;
}

void initTimer1s()
{
  timer1s = timerBegin(1000000);
  timerAttachInterrupt(timer1s, &onTimer1s);
  timerAlarm(timer1s, 1000000, true, 0);
}

void doTimer1s()
{
  if (timer1s_flag)
  {
    timer1s_flag = false;
    updateClock();
  }
}

// ==== CALLBACK DE LOS BOTONES ===============================
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
  struct tm t = {};
  t.tm_mday = lv_spinbox_get_value(objects.spnbx_scn_set_clock_day);
  t.tm_mon = lv_spinbox_get_value(objects.spnbx_scn_set_clock_month) - 1;
  t.tm_year = lv_spinbox_get_value(objects.spnbx_scn_set_clock_year) - 1900;
  t.tm_hour = lv_spinbox_get_value(objects.spnbx_scn_set_clock_hour);
  t.tm_min = lv_spinbox_get_value(objects.spnbx_scn_set_clock_min);
  t.tm_sec = lv_spinbox_get_value(objects.spnbx_scn_set_clock_sec);
  t.tm_isdst = -1;

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

void set_focused_spinbox(lv_obj_t *new_focus)
{
  if (focused_spinbox)
  {
    remove_style_spnbx_focused(focused_spinbox);
    add_style_spnbx_unfocused(focused_spinbox);
  }

  cursor_pos = 0;

  focused_spinbox = new_focus;
  if (focused_spinbox)
  {
    remove_style_spnbx_unfocused(focused_spinbox);
    add_style_spnbx_focused(focused_spinbox);
  }
}

void spinbox_focus_handler(lv_event_t *e)
{
  set_focused_spinbox((lv_obj_t *)lv_event_get_target(e));
}

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

  set_focused_spinbox(objects.spnbx_scn_set_clock_day);

  lv_scr_load(objects.scn_set_clock);
  estadoActual = SET_CLOCK;
  debugPrintln("Cambiando a: SET_CLOCK");
}

void btn_goto_set_clock_handler(lv_event_t *e)
{
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  wifi_start_ms = millis();
  wifi_synced_ms = 0;
  lv_scr_load(objects.scn_wifi_conn);
  lv_label_set_text(objects.lbl_scn_wi_fi_conn_network, WIFI_SSID);
  estadoActual = CONNECTING_WIFI;
  debugPrintln("Cambiando a: CONNECTING_WIFI (manual desde pantalla principal)");
}

void enter_LAST_SRAD()
{
  lv_scr_load(objects.scn_last_srad);
  lv_label_set_text(objects.lbl_scn_last_srad_message, "ULTIMO SRAD");
  // A diferencia de scn_post_srad, este título no debe parpadear

  char buf_lbl[64];

  // 1. RequestDateTime
  snprintf(buf_lbl, sizeof(buf_lbl), "SOLICITADO EL %s A LAS %s", bk_srad_req_date, bk_srad_req_time);
  lv_label_set_text(objects.lbl_scn_last_srad_request_date_time, buf_lbl);

  // 2. BreakDateTime
  snprintf(buf_lbl, sizeof(buf_lbl), "DESCONEXION EL %s A LAS %s", bk_srad_break_date, bk_srad_break_time);
  lv_label_set_text(objects.lbl_scn_last_srad_break_date_time, buf_lbl);

  // 3. StartDateTime
  snprintf(buf_lbl, sizeof(buf_lbl), "INICIADO EL %s A LAS %s", bk_srad_start_date, bk_srad_start_time);
  lv_label_set_text(objects.lbl_scn_last_srad_start_date_time, buf_lbl);

  // 4. StimatedEndDateTime
  snprintf(buf_lbl, sizeof(buf_lbl), "FIN PREVISTO EL %s A LAS %s", bk_srad_stimated_end_date, bk_srad_stimated_end_time);
  lv_label_set_text(objects.lbl_scn_last_srad_stimated_end_date_time, buf_lbl);

  // 5. StimatedDuration
  snprintf(buf_lbl, sizeof(buf_lbl), "DURACION PREVISTA: %s", bk_srad_stimated_duration);
  lv_label_set_text(objects.lbl_scn_last_srad_stimated_duration, buf_lbl);

  // 6. RealEndDateTime
  snprintf(buf_lbl, sizeof(buf_lbl), "FIN REAL EL %s A LAS %s", bk_srad_real_end_date, bk_srad_real_end_time);
  lv_label_set_text(objects.lbl_scn_last_srad_real_end_date_time, buf_lbl);

  // 7. RealDuration
  snprintf(buf_lbl, sizeof(buf_lbl), "DURACION REAL: %s", bk_srad_real_duration);
  lv_label_set_text(objects.lbl_scn_last_srad_real_duration, buf_lbl);

  estadoActual = LAST_SRAD;
  debugPrintln("Cambiando a: LAST_SRAD");
}

void btn_main_info_handler(lv_event_t *e)
{
  enter_LAST_SRAD();
}

void btn_scn_last_srad_back_handler(lv_event_t *e)
{
  updateSetClockButtonState();
  lv_scr_load(objects.main);
  estadoActual = SHOW_CLOCK;
  debugPrintln("Cambiando a: SHOW_CLOCK (vuelta desde LAST_SRAD)");
}

// ============================================================

// Carga desde LittleFS las variables bk_srad_* con las estadísticas del último SRAD guardado
void loadSradStatsFromLittleFS()
{
  File f = LittleFS.open(SRAD_STATS_FILE, "r");
  if (!f)
  {
    debugPrintln("No hay estadisticas de SRAD guardadas en LittleFS (primer arranque)");
    return;
  }

  while (f.available())
  {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.length() == 0)
      continue;

    int sep = line.indexOf('=');
    if (sep < 0)
      continue;

    String key = line.substring(0, sep);
    String value = line.substring(sep + 1);

    if (key == "srad_req_date")
      snprintf(bk_srad_req_date, sizeof(bk_srad_req_date), "%s", value.c_str());
    else if (key == "srad_req_time")
      snprintf(bk_srad_req_time, sizeof(bk_srad_req_time), "%s", value.c_str());
    else if (key == "srad_break_date")
      snprintf(bk_srad_break_date, sizeof(bk_srad_break_date), "%s", value.c_str());
    else if (key == "srad_break_time")
      snprintf(bk_srad_break_time, sizeof(bk_srad_break_time), "%s", value.c_str());
    else if (key == "srad_start_date")
      snprintf(bk_srad_start_date, sizeof(bk_srad_start_date), "%s", value.c_str());
    else if (key == "srad_start_time")
      snprintf(bk_srad_start_time, sizeof(bk_srad_start_time), "%s", value.c_str());
    else if (key == "srad_stimated_end_date")
      snprintf(bk_srad_stimated_end_date, sizeof(bk_srad_stimated_end_date), "%s", value.c_str());
    else if (key == "srad_stimated_end_time")
      snprintf(bk_srad_stimated_end_time, sizeof(bk_srad_stimated_end_time), "%s", value.c_str());
    else if (key == "srad_stimated_duration")
      snprintf(bk_srad_stimated_duration, sizeof(bk_srad_stimated_duration), "%s", value.c_str());
    else if (key == "srad_real_end_date")
      snprintf(bk_srad_real_end_date, sizeof(bk_srad_real_end_date), "%s", value.c_str());
    else if (key == "srad_real_end_time")
      snprintf(bk_srad_real_end_time, sizeof(bk_srad_real_end_time), "%s", value.c_str());
    else if (key == "srad_real_duration")
      snprintf(bk_srad_real_duration, sizeof(bk_srad_real_duration), "%s", value.c_str());
  }

  f.close();
  debugPrintln("Estadisticas del ultimo SRAD cargadas desde LittleFS");
}

void setup()
{
  Serial.begin(115200);
  debugPrint("INICIADO SETUP");
  String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  debugPrintln(SW_NAME_REV);
  debugPrintln("%s", LVGL_Arduino);

  pinMode(GPIO_SRAD, INPUT_PULLUP);
  pinMode(GPIO_ENDSRAD, INPUT_PULLUP);

  if (!LittleFS.begin(true))
  {
    debugPrintln("ERROR: no se pudo montar LittleFS");
  }
  else
  {
    loadSradStatsFromLittleFS();
  }

  lv_init();
  lv_tick_set_cb((lv_tick_get_cb_t)millis);
  lv_init_esp32();
  ui_init();

  updateSetClockButtonState();

  lv_obj_add_event_cb(objects.btn_scnmain_setclock, btn_goto_set_clock_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.btn_main_info, btn_main_info_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.btn_scn_last_srad_back, btn_scn_last_srad_back_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.btn_scn_set_clock_incr, btn_incr_set_clock_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.btn_scn_set_clock_decr, btn_decr_set_clock_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.btn_scn_set_clock_ok, btn_ok_set_clock_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.btn_scn_set_clock_left, btn_left_set_clock_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.btn_scn_set_clock_right, btn_right_set_clock_handler, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.spnbx_scn_set_clock_day, spinbox_focus_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.spnbx_scn_set_clock_month, spinbox_focus_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.spnbx_scn_set_clock_year, spinbox_focus_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.spnbx_scn_set_clock_hour, spinbox_focus_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.spnbx_scn_set_clock_min, spinbox_focus_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.spnbx_scn_set_clock_sec, spinbox_focus_handler, LV_EVENT_CLICKED, NULL);

  add_style_spnbx_unfocused(objects.spnbx_scn_set_clock_day);
  add_style_spnbx_unfocused(objects.spnbx_scn_set_clock_month);
  add_style_spnbx_unfocused(objects.spnbx_scn_set_clock_year);
  add_style_spnbx_unfocused(objects.spnbx_scn_set_clock_hour);
  add_style_spnbx_unfocused(objects.spnbx_scn_set_clock_min);
  add_style_spnbx_unfocused(objects.spnbx_scn_set_clock_sec);

  set_focused_spinbox(objects.spnbx_scn_set_clock_day);

  initTimer1s();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  wifi_start_ms = millis();
  estadoActual = CONNECTING_WIFI;
  lv_scr_load(objects.scn_wifi_conn);
  lv_label_set_text(objects.lbl_scn_wi_fi_conn_network, WIFI_SSID);
  debugPrintln("Cambiando a: CONNECTING_WIFI");
}

void start_blink_label(lv_obj_t *label)
{
  lv_anim_del(label, NULL);
  lv_obj_set_style_opa(label, LV_OPA_COVER, LV_PART_MAIN);

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, label);
  lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
  lv_anim_set_duration(&a, 500);
  lv_anim_set_playback_duration(&a, 500);
  lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
  lv_anim_set_exec_cb(&a, [](void *obj, int32_t val)
                      { lv_obj_set_style_opa((lv_obj_t *)obj, val, LV_PART_MAIN); });
  lv_anim_start(&a);
}

void checkSRADTrigger()
{
  if (digitalRead(GPIO_SRAD) == LOW)
  {
    pre_srad_countdown = 3 * 60 + 30;
    lv_scr_load(objects.scn_pre_srad);
    lv_label_set_text(objects.lbl_scn_pre_srad_message, "SRAD SOLICITADO");
    lv_obj_set_style_bg_color(objects.scn_pre_srad, lv_color_hex(0x00FF00), LV_PART_MAIN);
    start_blink_label(objects.lbl_scn_pre_srad_message);

    struct tm t;
    if (getRTCTime(t))
    {
      snprintf(hora_solicitud_srad, sizeof(hora_solicitud_srad), "%02d:%02d:%02d",
               t.tm_hour, t.tm_min, t.tm_sec);
      char buf_hora_srad[16];
      snprintf(buf_hora_srad, sizeof(buf_hora_srad), "A LAS %s", hora_solicitud_srad);
      lv_label_set_text(objects.lbl_scn_pre_srad_hora_srad, buf_hora_srad);

      // Guardar Fecha y Hora de la solicitud
      snprintf(srad_req_date, sizeof(srad_req_date), "%02d/%02d/%04d",
               t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
      snprintf(srad_req_time, sizeof(srad_req_time), "%02d:%02d:%02d",
               t.tm_hour, t.tm_min, t.tm_sec);
      // guarda los backups
      snprintf(bk_srad_req_date, sizeof(bk_srad_req_date), "%s", srad_req_date);
      snprintf(bk_srad_req_time, sizeof(bk_srad_req_time), "%s", srad_req_time);
    }

    lv_anim_del(objects.lbl_scn_pre_srad_desconexion, NULL);
    lv_obj_add_flag(objects.lbl_scn_pre_srad_desconexion, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.lbl_scn_pre_srad_hora_desconexion, LV_OBJ_FLAG_HIDDEN);

    // Resetear indicación de desconexión
    has_srad_break = false;

    estadoActual = PRE_SRAD;
    debugPrintln("Cambiando a: PRE_SRAD");
  }
}

// Guarda en LittleFS las variables bk_srad_* con las estadísticas del último SRAD
void saveSradStatsToLittleFS()
{
  File f = LittleFS.open(SRAD_STATS_FILE, "w");
  if (!f)
  {
    debugPrintln("ERROR: no se pudo abrir " SRAD_STATS_FILE " para escritura en LittleFS");
    return;
  }

  f.printf("srad_req_date=%s\n", bk_srad_req_date);
  f.printf("srad_req_time=%s\n", bk_srad_req_time);
  f.printf("srad_break_date=%s\n", bk_srad_break_date);
  f.printf("srad_break_time=%s\n", bk_srad_break_time);
  f.printf("srad_start_date=%s\n", bk_srad_start_date);
  f.printf("srad_start_time=%s\n", bk_srad_start_time);
  f.printf("srad_stimated_end_date=%s\n", bk_srad_stimated_end_date);
  f.printf("srad_stimated_end_time=%s\n", bk_srad_stimated_end_time);
  f.printf("srad_stimated_duration=%s\n", bk_srad_stimated_duration);
  f.printf("srad_real_end_date=%s\n", bk_srad_real_end_date);
  f.printf("srad_real_end_time=%s\n", bk_srad_real_end_time);
  f.printf("srad_real_duration=%s\n", bk_srad_real_duration);

  f.close();
  debugPrintln("Estadisticas del ultimo SRAD guardadas en LittleFS");
}

void enter_POST_SRAD()
{
  // Si veníamos de "cuenta adelante", detener el parpadeo y restaurar el color normal
  if (srad_overtime)
  {
    lv_anim_del(objects.lbl_scn_srad_countdown, NULL);
    lv_obj_set_style_opa(objects.lbl_scn_srad_countdown, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_text_color(objects.lbl_scn_srad_countdown, lv_color_hex(0x212121), LV_PART_MAIN);
    srad_overtime = false;
  }

  lv_scr_load(objects.scn_post_srad);
  lv_label_set_text(objects.lbl_scn_post_srad_message, "FIN SRAD");
  start_blink_label(objects.lbl_scn_post_srad_message);

  // Fecha y Hora de finalización real y cálculo de duración real
  struct tm t;
  if (getRTCTime(t))
  {
    snprintf(srad_real_end_date, sizeof(srad_real_end_date), "%02d/%02d/%04d",
             t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
    snprintf(srad_real_end_time, sizeof(srad_real_end_time), "%02d:%02d:%02d",
             t.tm_hour, t.tm_min, t.tm_sec);
    // Guarda los backups
    snprintf(bk_srad_real_end_date, sizeof(bk_srad_real_end_date), "%s", srad_real_end_date);
    snprintf(bk_srad_real_end_time, sizeof(bk_srad_real_end_time), "%s", srad_real_end_time);

    if (time_srad_start > 0)
    {
      time_t time_srad_end = mktime(&t);
      double diff_sec = difftime(time_srad_end, time_srad_start);
      if (diff_sec < 0)
        diff_sec = 0;
      uint32_t dur_s = (uint32_t)diff_sec;
      uint32_t h = dur_s / 3600;
      uint32_t m = (dur_s % 3600) / 60;
      uint32_t s = dur_s % 60;
      snprintf(srad_real_duration, sizeof(srad_real_duration), "%02d:%02d:%02d", h, m, s);
      // Guarda el backup
      snprintf(bk_srad_real_duration, sizeof(bk_srad_real_duration), "%s", srad_real_duration);
    }
  }

  // Buffers formateados según solicitud
  char buf_lbl[64];

  // 1. RequestDateTime
  snprintf(buf_lbl, sizeof(buf_lbl), "SOLICITADO EL %s A LAS %s", srad_req_date, srad_req_time);
  lv_label_set_text(objects.lbl_scnpost_srad_request_date_time, buf_lbl);

  // 2. BreakDateTime
  if (has_srad_break)
  {
    snprintf(buf_lbl, sizeof(buf_lbl), "DESCONEXION EL %s A LAS %s", srad_break_date, srad_break_time);
  }
  else
  {
    snprintf(buf_lbl, sizeof(buf_lbl), "DESCONEXION EL --/--/---- A LAS --:--:--");
  }
  lv_label_set_text(objects.lbl_scnpost_srad_break_date_time, buf_lbl);

  // 3. StartDateTime
  snprintf(buf_lbl, sizeof(buf_lbl), "INICIADO EL %s A LAS %s", srad_start_date, srad_start_time);
  lv_label_set_text(objects.lbl_scnpost_srad_start_date_time, buf_lbl);

  // 4. StimatedEndDateTime
  snprintf(buf_lbl, sizeof(buf_lbl), "FIN PREVISTO EL %s A LAS %s", srad_stimated_end_date, srad_stimated_end_time);
  lv_label_set_text(objects.lbl_scnpost_srad_stimated_end_date_time, buf_lbl);

  // 5. StimatedDuration
  snprintf(buf_lbl, sizeof(buf_lbl), "DURACION PREVISTA: %s", srad_stimated_duration);
  lv_label_set_text(objects.lbl_scnpost_srad_stimated_duration, buf_lbl);

  // 6. RealEndDateTime
  snprintf(buf_lbl, sizeof(buf_lbl), "FIN REAL EL %s A LAS %s", srad_real_end_date, srad_real_end_time);
  lv_label_set_text(objects.lbl_scnpost_srad_real_end_date_time, buf_lbl);

  // 7. RealDuration
  snprintf(buf_lbl, sizeof(buf_lbl), "DURACION REAL: %s", srad_real_duration);
  lv_label_set_text(objects.lbl_scnpost_srad_real_duration, buf_lbl);

  // Reiniciar cuenta atrás de permanencia en POST_SRAD
  post_srad_countdown = post_srad_duration_s;

  // Guardar los valores backup en LitteFS
  saveSradStatsToLittleFS();

  estadoActual = POST_SRAD;
  debugPrintln("Cambiando a: POST_SRAD");
}

void checkENDSRADTrigger()
{
  if (digitalRead(GPIO_SRAD) == HIGH)
  {
    enter_POST_SRAD();
  }
  if (digitalRead(GPIO_ENDSRAD) == LOW)
  {
    srad_countdown = 10;
    debugPrintln("ACORTANDO SRAD...");
  }
}

void enter_SRAD()
{
  struct tm t;
  getRTCTime(t);

  // Guardar instante exacto de inicio real
  time_srad_start = mktime(&t);
  snprintf(srad_start_date, sizeof(srad_start_date), "%02d/%02d/%04d",
           t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
  snprintf(srad_start_time, sizeof(srad_start_time), "%02d:%02d:%02d",
           t.tm_hour, t.tm_min, t.tm_sec);
  // Guarda los bakups
  snprintf(bk_srad_start_date, sizeof(bk_srad_start_date), "%s", srad_start_date);
  snprintf(bk_srad_start_time, sizeof(bk_srad_start_time), "%s", srad_start_time);

  uint32_t secs_to_next_hour = (59 - t.tm_min) * 60 + (60 - t.tm_sec);

  srad_countdown = secs_to_next_hour + 3600;
  srad_end_hour = (t.tm_hour + 2) % 24;

  // Duración estimada y Fecha/Hora estimada de fin
  uint32_t h = srad_countdown / 3600;
  uint32_t m = (srad_countdown % 3600) / 60;
  uint32_t s = srad_countdown % 60;
  snprintf(srad_stimated_duration, sizeof(srad_stimated_duration), "%02d:%02d:%02d", h, m, s);
  // Guarda el backup
  snprintf(bk_srad_stimated_duration, sizeof(bk_srad_stimated_duration), "%s", srad_stimated_duration);

  time_t time_srad_end_est = time_srad_start + srad_countdown;
  struct tm tm_end_est;
  localtime_r(&time_srad_end_est, &tm_end_est);

  snprintf(srad_stimated_end_date, sizeof(srad_stimated_end_date), "%02d/%02d/%04d",
           tm_end_est.tm_mday, tm_end_est.tm_mon + 1, tm_end_est.tm_year + 1900);
  snprintf(srad_stimated_end_time, sizeof(srad_stimated_end_time), "%02d:%02d:%02d",
           tm_end_est.tm_hour, tm_end_est.tm_min, tm_end_est.tm_sec);
  // Guarda los backups
  snprintf(bk_srad_stimated_end_date, sizeof(bk_srad_stimated_end_date), "%s", srad_stimated_end_date);
  snprintf(bk_srad_stimated_end_time, sizeof(bk_srad_stimated_end_time), "%s", srad_stimated_end_time);

  char buf_end[24];
  snprintf(buf_end, sizeof(buf_end), "FIN DE SRAD: %02d:00:00", srad_end_hour);

  lv_scr_load(objects.scn_srad);
  lv_label_set_text(objects.lbl_scn_srad_end_time, buf_end);
  lv_label_set_text(objects.lbl_scn_srad_message, "SRAD ACTIVO");

  // Reiniciar modo "cuenta adelante" y aspecto normal del label de cuenta atrás
  srad_overtime = false;
  srad_overtime_s = 0;
  lv_anim_del(objects.lbl_scn_srad_countdown, NULL);
  lv_obj_set_style_opa(objects.lbl_scn_srad_countdown, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_text_color(objects.lbl_scn_srad_countdown, lv_color_hex(0x212121), LV_PART_MAIN);

  char buf_cd[10];
  snprintf(buf_cd, sizeof(buf_cd), "%02d:%02d:%02d", h, m, s);
  lv_label_set_text(objects.lbl_scn_srad_countdown, buf_cd);

  char buf_solicitud[32];
  snprintf(buf_solicitud, sizeof(buf_solicitud), "SOLICITADO A LAS %s", hora_solicitud_srad);
  lv_label_set_text(objects.lbl_scn_srad_solicitud_time, buf_solicitud);

  char buf_desconexion[32];
  snprintf(buf_desconexion, sizeof(buf_desconexion), "DESCONEXION A LAS %s", hora_desconexion_srad);
  lv_label_set_text(objects.lbl_scn_srad_desconexion_time, buf_desconexion);

  char buf_duracion[24];
  snprintf(buf_duracion, sizeof(buf_duracion), "DURACION: %s", buf_cd);
  lv_label_set_text(objects.lbl_scn_srad_duracion_time, buf_duracion);

  char buf_inicio[24];
  snprintf(buf_inicio, sizeof(buf_inicio), "INICIO A LAS %02d:%02d:%02d",
           t.tm_hour, t.tm_min, t.tm_sec);
  lv_label_set_text(objects.lbl_scn_srad_inicio_time, buf_inicio);

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

    if (millis() - wifi_synced_ms >= 2000)
    {
      wifi_synced_ms = 0;

      struct timeval tv_invalid = {0, 0};
      settimeofday(&tv_invalid, nullptr);
      configTime(0, 0, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);
      wifi_start_ms = millis();
      lv_scr_load(objects.scn_rtc_sync);
      estadoActual = SYNCING_NTP;
      debugPrintln("Cambiando a: SYNCING_NTP");
    }
  }
  else if (millis() - wifi_start_ms > 15000)
  {
    debugPrintln("WiFi: timeout");
    wifi_synced_ms = 0;
    lv_label_set_text(objects.lbl_scn_set_clock_title_1, "WiFi no disponible");
    enter_SET_CLOCK();
  }
  lv_task_handler();
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
    if (millis() - ntp_synced_ms >= 2000)
    {
      ntp_synced_ms = 0;
      relojAjustadoManualmente = false;
      updateSetClockButtonState();
      lv_scr_load(objects.main);
      estadoActual = SHOW_CLOCK;
      debugPrintln("Cambiando a: SHOW_CLOCK");
    }
  }
  else if (millis() - wifi_start_ms > NTP_SYNC_TIMEOUT_MS)
  {
    debugPrintln("NTP: timeout");
    lv_label_set_text(objects.lbl_scn_set_clock_title_1, "NTP Server no disponible");
    enter_SET_CLOCK();
  }
  lv_task_handler();
}

void do_SET_CLOCK()
{
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

    if (pre_srad_countdown > 0)
      pre_srad_countdown--;

    uint16_t mins = pre_srad_countdown / 60;
    uint16_t secs = pre_srad_countdown % 60;
    char buf[8];
    snprintf(buf, sizeof(buf), "%02d:%02d", mins, secs);
    lv_label_set_text(objects.lbl_scn_pre_srad_countdown, buf);

    if (pre_srad_countdown == SHUTDOWN_TIME)
    {
      lv_obj_set_style_bg_color(objects.scn_pre_srad, lv_color_hex(0xFFA500), LV_PART_MAIN);

      lv_label_set_text(objects.lbl_scn_pre_srad_desconexion, "DESCONEXION");
      lv_obj_clear_flag(objects.lbl_scn_pre_srad_desconexion, LV_OBJ_FLAG_HIDDEN);
      start_blink_label(objects.lbl_scn_pre_srad_desconexion);

      struct tm t_desc;
      if (getRTCTime(t_desc))
      {
        snprintf(hora_desconexion_srad, sizeof(hora_desconexion_srad), "%02d:%02d:%02d",
                 t_desc.tm_hour, t_desc.tm_min, t_desc.tm_sec);
        char buf_hora_desconexion[16];
        snprintf(buf_hora_desconexion, sizeof(buf_hora_desconexion), "A LAS %s", hora_desconexion_srad);
        lv_label_set_text(objects.lbl_scn_pre_srad_hora_desconexion, buf_hora_desconexion);

        // Guardar Fecha y Hora de la desconexión
        snprintf(srad_break_date, sizeof(srad_break_date), "%02d/%02d/%04d",
                 t_desc.tm_mday, t_desc.tm_mon + 1, t_desc.tm_year + 1900);
        snprintf(srad_break_time, sizeof(srad_break_time), "%02d:%02d:%02d",
                 t_desc.tm_hour, t_desc.tm_min, t_desc.tm_sec);
        has_srad_break = true;
        // Guarda los backups
        snprintf(bk_srad_break_date, sizeof(bk_srad_break_date), "%s", srad_break_date);
        snprintf(bk_srad_break_time, sizeof(bk_srad_break_time), "%s", srad_break_time);
      }
      lv_obj_clear_flag(objects.lbl_scn_pre_srad_hora_desconexion, LV_OBJ_FLAG_HIDDEN);
    }

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

    if (!srad_overtime)
    {
      if (srad_countdown > 0)
        srad_countdown--;

      uint32_t h = srad_countdown / 3600;
      uint32_t m = (srad_countdown % 3600) / 60;
      uint32_t s = srad_countdown % 60;
      char buf[10];
      snprintf(buf, sizeof(buf), "%02d:%02d:%02d", h, m, s);
      lv_label_set_text(objects.lbl_scn_srad_countdown, buf);

      if (srad_countdown == 0)
      {
        // La cuenta atrás ha llegado a 0 pero GPIO_SRAD sigue activo:
        // pasamos a cuenta adelante, con el texto parpadeando en blanco
        srad_overtime = true;
        srad_overtime_s = 0;
        lv_obj_set_style_text_color(objects.lbl_scn_srad_countdown, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        start_blink_label(objects.lbl_scn_srad_countdown);
        debugPrintln("SRAD: cuenta atras a 0, iniciando cuenta adelante");
      }
    }
    else
    {
      // Modo cuenta adelante
      srad_overtime_s++;

      uint32_t h = srad_overtime_s / 3600;
      uint32_t m = (srad_overtime_s % 3600) / 60;
      uint32_t s = srad_overtime_s % 60;
      char buf[10];
      snprintf(buf, sizeof(buf), "%02d:%02d:%02d", h, m, s);
      lv_label_set_text(objects.lbl_scn_srad_countdown, buf);
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

    if (post_srad_countdown > 0)
      post_srad_countdown--;

    if (post_srad_countdown == 0)
    {
      updateSetClockButtonState();
      lv_scr_load(objects.main);
      estadoActual = SHOW_CLOCK;
      debugPrintln("Cambiando a: SHOW_CLOCK");
    }
  }
}

void do_LAST_SRAD()
{
  // La pantalla se mantiene mostrando los datos del último SRAD
  // hasta que el usuario pulse btn_scn_last_srad_back
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
  case LAST_SRAD:
    do_LAST_SRAD();
    break;
  default:
    break;
  }
  lv_task_handler();
  delay(5);
}