#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;

//
// Event handlers
//

lv_obj_t *tick_value_change_obj;

//
// Screens
//

void create_screen_scn_post_srad() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.scn_post_srad = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x00f500), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // lbl_scnPostSRAD_Message
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_post_srad_message = obj;
            lv_obj_set_pos(obj, 0, 32);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "FIN SRAD");
        }
        {
            // lbl_scnpostSRAD_RequestDateTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scnpost_srad_request_date_time = obj;
            lv_obj_set_pos(obj, 0, 119);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "SOLICITADO EL XX/YY/ZZ A LAS HH:MM:SS");
        }
        {
            // lbl_scnpostSRAD_BreakDateTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scnpost_srad_break_date_time = obj;
            lv_obj_set_pos(obj, 0, 171);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "DESCONEXION EL XX/YY/ZZ A LAS HH:MM:SS");
        }
        {
            // lbl_scnpostSRAD_StartDateTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scnpost_srad_start_date_time = obj;
            lv_obj_set_pos(obj, 0, 218);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "INICIADO EL XX/YY/ZZ A LAS HH:MM:SS");
        }
        {
            // lbl_scnpostSRAD_StimatedEndDateTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scnpost_srad_stimated_end_date_time = obj;
            lv_obj_set_pos(obj, 0, 264);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "FIN PREVISTO EL XX/YY/ZZ A LAS HH:MM:SS");
        }
        {
            // lbl_scnpostSRAD_StimatedDuration
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scnpost_srad_stimated_duration = obj;
            lv_obj_set_pos(obj, 0, 310);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "DURACION PREVISTA: HH:MM:SS");
        }
        {
            // lbl_scnpostSRAD_RealDuration
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scnpost_srad_real_duration = obj;
            lv_obj_set_pos(obj, 0, 402);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "DURACION REAL: HH:MM:SS");
        }
        {
            // lbl_scnpostSRAD_RealEndDateTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scnpost_srad_real_end_date_time = obj;
            lv_obj_set_pos(obj, 0, 356);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "FIN REAL EL XX/YY/ZZ A LAS HH:MM:SS");
        }
    }
    
    tick_screen_scn_post_srad();
}

void tick_screen_scn_post_srad() {
}

void create_screen_scn_pre_srad() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.scn_pre_srad = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x00ff00), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // lbl_scnPreSRAD_Countdown
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_pre_srad_countdown = obj;
            lv_obj_set_pos(obj, 0, 217);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &ui_font_roboto_mono_bold_250, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "12:30");
        }
        {
            // lbl_scnPreSRAD_Message
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_pre_srad_message = obj;
            lv_obj_set_pos(obj, 0, 8);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "SRAD SOLICITADO");
        }
        {
            // lbl_scnPreSRAD_HoraSRAD
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_pre_srad_hora_srad = obj;
            lv_obj_set_pos(obj, 0, 49);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "A LAS: hh:mm:ss");
        }
        {
            // lbl_scnPreSRAD_Desconexion
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_pre_srad_desconexion = obj;
            lv_obj_set_pos(obj, 0, 114);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "DESCONEXION");
        }
        {
            // lbl_scnPreSRAD_HoraDesconexion
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_pre_srad_hora_desconexion = obj;
            lv_obj_set_pos(obj, 0, 163);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "A LAS: hh:mm:ss");
        }
    }
    
    tick_screen_scn_pre_srad();
}

void tick_screen_scn_pre_srad() {
}

void create_screen_scn_set_clock() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.scn_set_clock = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    {
        lv_obj_t *parent_obj = obj;
        {
            // lbl_scnSetClock_Title_2
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_set_clock_title_2 = obj;
            lv_obj_set_pos(obj, 0, 72);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Configurar Fecha y Hora");
        }
        {
            // btn_scnSetClock_OK
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_scn_set_clock_ok = obj;
            lv_obj_set_pos(obj, 570, 388);
            lv_obj_set_size(obj, 200, 80);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "OK");
                }
            }
        }
        {
            // lbl_scnSetClock_Title_1
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_set_clock_title_1 = obj;
            lv_obj_set_pos(obj, 0, 9);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "WiFi no disponible");
        }
        {
            // spnbx_scnSetClock_Day
            lv_obj_t *obj = lv_spinbox_create(parent_obj);
            objects.spnbx_scn_set_clock_day = obj;
            lv_obj_set_pos(obj, 157, 139);
            lv_obj_set_size(obj, 140, 76);
            lv_spinbox_set_digit_format(obj, 2, 0);
            lv_spinbox_set_range(obj, 1, 31);
            lv_spinbox_set_rollover(obj, false);
            lv_spinbox_set_step(obj, 1);
            lv_spinbox_set_value(obj, 1);
            add_style_spnbx_unfocused(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // spnbx_scnSetClock_Month
            lv_obj_t *obj = lv_spinbox_create(parent_obj);
            objects.spnbx_scn_set_clock_month = obj;
            lv_obj_set_pos(obj, 338, 143);
            lv_obj_set_size(obj, 140, 76);
            lv_spinbox_set_digit_format(obj, 2, 0);
            lv_spinbox_set_range(obj, 1, 12);
            lv_spinbox_set_rollover(obj, false);
            lv_spinbox_set_step(obj, 1);
            lv_spinbox_set_value(obj, 1);
            add_style_spnbx_unfocused(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // spnbx_scnSetClock_Year
            lv_obj_t *obj = lv_spinbox_create(parent_obj);
            objects.spnbx_scn_set_clock_year = obj;
            lv_obj_set_pos(obj, 517, 144);
            lv_obj_set_size(obj, 140, 76);
            lv_spinbox_set_digit_format(obj, 4, 0);
            lv_spinbox_set_range(obj, 2026, 2050);
            lv_spinbox_set_rollover(obj, false);
            lv_spinbox_set_step(obj, 1);
            lv_spinbox_set_value(obj, 1);
            add_style_spnbx_unfocused(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // spnbx_scnSetClock_Hour
            lv_obj_t *obj = lv_spinbox_create(parent_obj);
            objects.spnbx_scn_set_clock_hour = obj;
            lv_obj_set_pos(obj, 157, 244);
            lv_obj_set_size(obj, 140, 76);
            lv_spinbox_set_digit_format(obj, 2, 0);
            lv_spinbox_set_range(obj, 0, 23);
            lv_spinbox_set_rollover(obj, false);
            lv_spinbox_set_step(obj, 1);
            lv_spinbox_set_value(obj, 0);
            add_style_spnbx_unfocused(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // spnbx_scnSetClock_Min
            lv_obj_t *obj = lv_spinbox_create(parent_obj);
            objects.spnbx_scn_set_clock_min = obj;
            lv_obj_set_pos(obj, 338, 244);
            lv_obj_set_size(obj, 140, 76);
            lv_spinbox_set_digit_format(obj, 2, 0);
            lv_spinbox_set_range(obj, 0, 59);
            lv_spinbox_set_rollover(obj, false);
            lv_spinbox_set_step(obj, 1);
            lv_spinbox_set_value(obj, 0);
            add_style_spnbx_unfocused(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // spnbx_scnSetClock_Sec
            lv_obj_t *obj = lv_spinbox_create(parent_obj);
            objects.spnbx_scn_set_clock_sec = obj;
            lv_obj_set_pos(obj, 517, 244);
            lv_obj_set_size(obj, 140, 76);
            lv_spinbox_set_digit_format(obj, 2, 0);
            lv_spinbox_set_range(obj, 0, 29);
            lv_spinbox_set_rollover(obj, false);
            lv_spinbox_set_step(obj, 1);
            lv_spinbox_set_value(obj, 0);
            add_style_spnbx_unfocused(obj);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // lbl_scnSetClock_/_1
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_set_clock___1 = obj;
            lv_obj_set_pos(obj, 309, 155);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "/");
        }
        {
            // lbl_scnSetClock_/_2
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_set_clock___2 = obj;
            lv_obj_set_pos(obj, 489, 155);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "/");
        }
        {
            // lbl_scnSetClock_Dot_1
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_set_clock_dot_1 = obj;
            lv_obj_set_pos(obj, 312, 254);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, ":");
        }
        {
            // lbl_scnSetClock_Dot_2
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_set_clock_dot_2 = obj;
            lv_obj_set_pos(obj, 492, 254);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, ":");
        }
        {
            // btn_scnSetClock_Incr
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_scn_set_clock_incr = obj;
            lv_obj_set_pos(obj, 137, 388);
            lv_obj_set_size(obj, 80, 80);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x21f342), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "+");
                }
            }
        }
        {
            // btn_scnSetClock_Decr
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_scn_set_clock_decr = obj;
            lv_obj_set_pos(obj, 257, 388);
            lv_obj_set_size(obj, 80, 80);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x21f342), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "-");
                }
            }
        }
        {
            // btn_scnSetClock_Left
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_scn_set_clock_left = obj;
            lv_obj_set_pos(obj, 17, 388);
            lv_obj_set_size(obj, 80, 80);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x21f342), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "<");
                }
            }
        }
        {
            // btn_scnSetClock_Right
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_scn_set_clock_right = obj;
            lv_obj_set_pos(obj, 377, 388);
            lv_obj_set_size(obj, 80, 80);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x21f342), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, ">");
                }
            }
        }
    }
    
    tick_screen_scn_set_clock();
}

void tick_screen_scn_set_clock() {
}

void create_screen_scn_wifi_conn() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.scn_wifi_conn = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    {
        lv_obj_t *parent_obj = obj;
        {
            // lbl_scnWiFiConn_Conn
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_wi_fi_conn_conn = obj;
            lv_obj_set_pos(obj, 0, 49);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Conectando WiFi:");
        }
        {
            // lbl_scnWiFiConn_Network
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_wi_fi_conn_network = obj;
            lv_obj_set_pos(obj, 0, 109);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "...");
        }
        {
            // spn_scnWifiConn_Spn
            lv_obj_t *obj = lv_spinner_create(parent_obj);
            objects.spn_scn_wifi_conn_spn = obj;
            lv_obj_set_pos(obj, 335, 200);
            lv_obj_set_size(obj, 130, 130);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
    }
    
    tick_screen_scn_wifi_conn();
}

void tick_screen_scn_wifi_conn() {
}

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xf5f5f5), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // lbl_scnmain_date
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scnmain_date = obj;
            lv_obj_set_pos(obj, 0, 94);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &ui_font_roboto_mono_bold_100, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "01/02/2026");
        }
        {
            // lbl_scnmain_time
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scnmain_time = obj;
            lv_obj_set_pos(obj, 0, 240);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &ui_font_roboto_mono_bold_125, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "01:02:03");
        }
        {
            // btn_scnmain_setclock
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_scnmain_setclock = obj;
            lv_obj_set_pos(obj, 569, 369);
            lv_obj_set_size(obj, 100, 100);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // img_setClock
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    objects.img_set_clock = obj;
                    lv_obj_set_pos(obj, 5, 14);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_image_set_src(obj, &img_set_clock2_50x50);
                }
            }
        }
        {
            // btn_main_info
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_main_info = obj;
            lv_obj_set_pos(obj, 683, 369);
            lv_obj_set_size(obj, 100, 100);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xf5f5f5), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_shadow_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // img_info
                    lv_obj_t *obj = lv_image_create(parent_obj);
                    objects.img_info = obj;
                    lv_obj_set_pos(obj, -20, -11);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_image_set_src(obj, &img_info);
                    lv_obj_set_style_bg_color(obj, lv_color_hex(0xf5f5f5), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
    
    tick_screen_main();
}

void tick_screen_main() {
}

void create_screen_scn_rtc_sync() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.scn_rtc_sync = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    {
        lv_obj_t *parent_obj = obj;
        {
            // lbl_scnRtcSync_Sync
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_rtc_sync_sync = obj;
            lv_obj_set_pos(obj, 0, 49);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "Sincronizando Reloj");
        }
        {
            // spn_scnRtcSync_Spn
            lv_obj_t *obj = lv_spinner_create(parent_obj);
            objects.spn_scn_rtc_sync_spn = obj;
            lv_obj_set_pos(obj, 335, 200);
            lv_obj_set_size(obj, 130, 130);
            lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE);
        }
    }
    
    tick_screen_scn_rtc_sync();
}

void tick_screen_scn_rtc_sync() {
}

void create_screen_scn_srad() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.scn_srad = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // lbl_scnSRAD_Countdown
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_srad_countdown = obj;
            lv_obj_set_pos(obj, 0, 274);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &ui_font_roboto_mono_bold_100, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "00:00:00");
        }
        {
            // lbl_scnSRAD_Message
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_srad_message = obj;
            lv_obj_set_pos(obj, 0, 3);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "SRAD ACTIVO");
        }
        {
            // lbl_scnSRAD_EndTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_srad_end_time = obj;
            lv_obj_set_pos(obj, 0, 382);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "HORA FINAL: 00:00:00");
        }
        {
            // lbl_scnSRAD_SolicitudTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_srad_solicitud_time = obj;
            lv_obj_set_pos(obj, 0, 78);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "SOLICITADO A LAS HH:MM:SS");
        }
        {
            // lbl_scnSRAD_DesconexionTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_srad_desconexion_time = obj;
            lv_obj_set_pos(obj, 0, 122);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "DESCONEXION A LAS HH:MM:SS");
        }
        {
            // lbl_scnSRAD_DuracionTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_srad_duracion_time = obj;
            lv_obj_set_pos(obj, 0, 208);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "DURACION:  HH:MM:SS");
        }
        {
            // lbl_scnSRAD_InicioTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_srad_inicio_time = obj;
            lv_obj_set_pos(obj, 0, 166);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "INICIO A LAS HH:MM:SS");
        }
    }
    
    tick_screen_scn_srad();
}

void tick_screen_scn_srad() {
}

void create_screen_scn_last_srad() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.scn_last_srad = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xf5f5f5), LV_PART_MAIN | LV_STATE_DEFAULT);
    {
        lv_obj_t *parent_obj = obj;
        {
            // lbl_scnLastSRAD_Message
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_last_srad_message = obj;
            lv_obj_set_pos(obj, 0, 6);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_48, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "INFORMACION ULTIMO SRAD");
        }
        {
            // lbl_scnLastSRAD_RequestDateTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_last_srad_request_date_time = obj;
            lv_obj_set_pos(obj, 0, 83);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "SOLICITADO EL XX/YY/ZZ A LAS HH:MM:SS");
        }
        {
            // lbl_scnLastSRAD_BreakDateTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_last_srad_break_date_time = obj;
            lv_obj_set_pos(obj, 0, 135);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "DESCONEXION EL XX/YY/ZZ A LAS HH:MM:SS");
        }
        {
            // lbl_scnLastSRAD_StartDateTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_last_srad_start_date_time = obj;
            lv_obj_set_pos(obj, 0, 182);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "INICIADO EL XX/YY/ZZ A LAS HH:MM:SS");
        }
        {
            // lbl_scnLastSRAD_StimatedEndDateTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_last_srad_stimated_end_date_time = obj;
            lv_obj_set_pos(obj, 0, 228);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "FIN PREVISTO EL XX/YY/ZZ A LAS HH:MM:SS");
        }
        {
            // lbl_scnLastSRAD_StimatedDuration
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_last_srad_stimated_duration = obj;
            lv_obj_set_pos(obj, 0, 274);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "DURACION PREVISTA: HH:MM:SS");
        }
        {
            // lbl_scnLastSRAD_RealDuration
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_last_srad_real_duration = obj;
            lv_obj_set_pos(obj, 0, 366);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "DURACION REAL: HH:MM:SS");
        }
        {
            // lbl_scnLastSRAD_RealEndDateTime
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.lbl_scn_last_srad_real_end_date_time = obj;
            lv_obj_set_pos(obj, 0, 320);
            lv_obj_set_size(obj, 800, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_34, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text_static(obj, "FIN REAL EL XX/YY/ZZ A LAS HH:MM:SS");
        }
        {
            // btn_scnLastSRAD_Back
            lv_obj_t *obj = lv_button_create(parent_obj);
            objects.btn_scn_last_srad_back = obj;
            lv_obj_set_pos(obj, 304, 417);
            lv_obj_set_size(obj, 192, 50);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text_static(obj, "VOLVER");
                }
            }
        }
    }
    
    tick_screen_scn_last_srad();
}

void tick_screen_scn_last_srad() {
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_scn_post_srad,
    tick_screen_scn_pre_srad,
    tick_screen_scn_set_clock,
    tick_screen_scn_wifi_conn,
    tick_screen_main,
    tick_screen_scn_rtc_sync,
    tick_screen_scn_srad,
    tick_screen_scn_last_srad,
};
void tick_screen(int screen_index) {
    if (screen_index >= 0 && screen_index < 8) {
        tick_screen_funcs[screen_index]();
    }
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen(screenId - 1);
}

//
// Fonts
//

ext_font_desc_t fonts[] = {
    { "Roboto_Mono_Bold_100", &ui_font_roboto_mono_bold_100 },
    { "Roboto_Mono_Bold_125", &ui_font_roboto_mono_bold_125 },
    { "Roboto_Mono_Bold_250", &ui_font_roboto_mono_bold_250 },
#if LV_FONT_MONTSERRAT_8
    { "MONTSERRAT_8", &lv_font_montserrat_8 },
#endif
#if LV_FONT_MONTSERRAT_10
    { "MONTSERRAT_10", &lv_font_montserrat_10 },
#endif
#if LV_FONT_MONTSERRAT_12
    { "MONTSERRAT_12", &lv_font_montserrat_12 },
#endif
#if LV_FONT_MONTSERRAT_14
    { "MONTSERRAT_14", &lv_font_montserrat_14 },
#endif
#if LV_FONT_MONTSERRAT_16
    { "MONTSERRAT_16", &lv_font_montserrat_16 },
#endif
#if LV_FONT_MONTSERRAT_18
    { "MONTSERRAT_18", &lv_font_montserrat_18 },
#endif
#if LV_FONT_MONTSERRAT_20
    { "MONTSERRAT_20", &lv_font_montserrat_20 },
#endif
#if LV_FONT_MONTSERRAT_22
    { "MONTSERRAT_22", &lv_font_montserrat_22 },
#endif
#if LV_FONT_MONTSERRAT_24
    { "MONTSERRAT_24", &lv_font_montserrat_24 },
#endif
#if LV_FONT_MONTSERRAT_26
    { "MONTSERRAT_26", &lv_font_montserrat_26 },
#endif
#if LV_FONT_MONTSERRAT_28
    { "MONTSERRAT_28", &lv_font_montserrat_28 },
#endif
#if LV_FONT_MONTSERRAT_30
    { "MONTSERRAT_30", &lv_font_montserrat_30 },
#endif
#if LV_FONT_MONTSERRAT_32
    { "MONTSERRAT_32", &lv_font_montserrat_32 },
#endif
#if LV_FONT_MONTSERRAT_34
    { "MONTSERRAT_34", &lv_font_montserrat_34 },
#endif
#if LV_FONT_MONTSERRAT_36
    { "MONTSERRAT_36", &lv_font_montserrat_36 },
#endif
#if LV_FONT_MONTSERRAT_38
    { "MONTSERRAT_38", &lv_font_montserrat_38 },
#endif
#if LV_FONT_MONTSERRAT_40
    { "MONTSERRAT_40", &lv_font_montserrat_40 },
#endif
#if LV_FONT_MONTSERRAT_42
    { "MONTSERRAT_42", &lv_font_montserrat_42 },
#endif
#if LV_FONT_MONTSERRAT_44
    { "MONTSERRAT_44", &lv_font_montserrat_44 },
#endif
#if LV_FONT_MONTSERRAT_46
    { "MONTSERRAT_46", &lv_font_montserrat_46 },
#endif
#if LV_FONT_MONTSERRAT_48
    { "MONTSERRAT_48", &lv_font_montserrat_48 },
#endif
};

//
// Color themes
//

uint32_t active_theme_index = 0;

//
//
//

void create_screens() {

// Set default LVGL theme
    lv_display_t *dispp = lv_display_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_display_set_theme(dispp, theme);
    
    // Initialize screens
    // Create screens
    create_screen_scn_post_srad();
    create_screen_scn_pre_srad();
    create_screen_scn_set_clock();
    create_screen_scn_wifi_conn();
    create_screen_main();
    create_screen_scn_rtc_sync();
    create_screen_scn_srad();
    create_screen_scn_last_srad();
}