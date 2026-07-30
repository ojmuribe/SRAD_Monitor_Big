#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_SCN_POST_SRAD = 1,
    SCREEN_ID_SCN_PRE_SRAD = 2,
    SCREEN_ID_SCN_SET_CLOCK = 3,
    SCREEN_ID_SCN_WIFI_CONN = 4,
    SCREEN_ID_MAIN = 5,
    SCREEN_ID_SCN_RTC_SYNC = 6,
    SCREEN_ID_SCN_SRAD = 7,
    SCREEN_ID_SCN_LAST_SRAD = 8,
    _SCREEN_ID_LAST = 8
};

typedef struct _objects_t {
    lv_obj_t *scn_post_srad;
    lv_obj_t *scn_pre_srad;
    lv_obj_t *scn_set_clock;
    lv_obj_t *scn_wifi_conn;
    lv_obj_t *main;
    lv_obj_t *scn_rtc_sync;
    lv_obj_t *scn_srad;
    lv_obj_t *scn_last_srad;
    lv_obj_t *lbl_scn_post_srad_message;
    lv_obj_t *lbl_scnpost_srad_request_date_time;
    lv_obj_t *lbl_scnpost_srad_break_date_time;
    lv_obj_t *lbl_scnpost_srad_start_date_time;
    lv_obj_t *lbl_scnpost_srad_stimated_end_date_time;
    lv_obj_t *lbl_scnpost_srad_stimated_duration;
    lv_obj_t *lbl_scnpost_srad_real_duration;
    lv_obj_t *lbl_scnpost_srad_real_end_date_time;
    lv_obj_t *lbl_scn_pre_srad_countdown;
    lv_obj_t *lbl_scn_pre_srad_message;
    lv_obj_t *lbl_scn_pre_srad_hora_srad;
    lv_obj_t *lbl_scn_pre_srad_desconexion;
    lv_obj_t *lbl_scn_pre_srad_hora_desconexion;
    lv_obj_t *lbl_scn_set_clock_title_2;
    lv_obj_t *btn_scn_set_clock_ok;
    lv_obj_t *lbl_scn_set_clock_title_1;
    lv_obj_t *spnbx_scn_set_clock_day;
    lv_obj_t *spnbx_scn_set_clock_month;
    lv_obj_t *spnbx_scn_set_clock_year;
    lv_obj_t *spnbx_scn_set_clock_hour;
    lv_obj_t *spnbx_scn_set_clock_min;
    lv_obj_t *spnbx_scn_set_clock_sec;
    lv_obj_t *lbl_scn_set_clock___1;
    lv_obj_t *lbl_scn_set_clock___2;
    lv_obj_t *lbl_scn_set_clock_dot_1;
    lv_obj_t *lbl_scn_set_clock_dot_2;
    lv_obj_t *btn_scn_set_clock_incr;
    lv_obj_t *btn_scn_set_clock_decr;
    lv_obj_t *btn_scn_set_clock_left;
    lv_obj_t *btn_scn_set_clock_right;
    lv_obj_t *lbl_scn_wi_fi_conn_conn;
    lv_obj_t *lbl_scn_wi_fi_conn_network;
    lv_obj_t *spn_scn_wifi_conn_spn;
    lv_obj_t *lbl_scnmain_date;
    lv_obj_t *lbl_scnmain_time;
    lv_obj_t *btn_scnmain_setclock;
    lv_obj_t *btn_main_info;
    lv_obj_t *lbl_scn_rtc_sync_sync;
    lv_obj_t *spn_scn_rtc_sync_spn;
    lv_obj_t *lbl_scn_srad_countdown;
    lv_obj_t *lbl_scn_srad_message;
    lv_obj_t *lbl_scn_srad_end_time;
    lv_obj_t *lbl_scn_srad_solicitud_time;
    lv_obj_t *lbl_scn_srad_desconexion_time;
    lv_obj_t *lbl_scn_srad_duracion_time;
    lv_obj_t *lbl_scn_srad_inicio_time;
    lv_obj_t *lbl_scn_last_srad_message;
    lv_obj_t *lbl_scn_last_srad_request_date_time;
    lv_obj_t *lbl_scn_last_srad_break_date_time;
    lv_obj_t *lbl_scn_last_srad_start_date_time;
    lv_obj_t *lbl_scn_last_srad_stimated_end_date_time;
    lv_obj_t *lbl_scn_last_srad_stimated_duration;
    lv_obj_t *lbl_scn_last_srad_real_duration;
    lv_obj_t *lbl_scn_last_srad_real_end_date_time;
    lv_obj_t *btn_scn_last_srad_back;
} objects_t;

extern objects_t objects;

void create_screen_scn_post_srad();
void tick_screen_scn_post_srad();

void create_screen_scn_pre_srad();
void tick_screen_scn_pre_srad();

void create_screen_scn_set_clock();
void tick_screen_scn_set_clock();

void create_screen_scn_wifi_conn();
void tick_screen_scn_wifi_conn();

void create_screen_main();
void tick_screen_main();

void create_screen_scn_rtc_sync();
void tick_screen_scn_rtc_sync();

void create_screen_scn_srad();
void tick_screen_scn_srad();

void create_screen_scn_last_srad();
void tick_screen_scn_last_srad();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/