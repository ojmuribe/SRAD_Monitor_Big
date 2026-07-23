#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: spnbx_focused
lv_style_t *get_style_spnbx_focused_MAIN_DEFAULT();
void add_style_spnbx_focused(lv_obj_t *obj);
void remove_style_spnbx_focused(lv_obj_t *obj);

// Style: spnbx_unfocused
lv_style_t *get_style_spnbx_unfocused_MAIN_DEFAULT();
lv_style_t *get_style_spnbx_unfocused_CURSOR_DEFAULT();
void add_style_spnbx_unfocused(lv_obj_t *obj);
void remove_style_spnbx_unfocused(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/