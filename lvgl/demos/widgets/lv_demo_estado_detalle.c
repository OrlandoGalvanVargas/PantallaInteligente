/**
 * @file lv_demo_estado_detalle.c
 * Vista detallada de un estado
 */

#include "lv_demo_estado_detalle.h"
static void btn_back_click_cb(lv_event_t * e);

void lv_demo_estado_detalle_show(estado_info_t * estado)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(cont, lv_color_white(), 0);

    lv_obj_t * title = lv_label_create(cont);
    lv_label_set_text_fmt(title, "Información del estado %s", estado->nombre);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x2196f3), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    lv_obj_t * info = lv_label_create(cont);
    lv_label_set_text_fmt(info,
        "🔴 Críticos: %d\n"
        "🟡 Alerta: %d\n"
        "🟢 Estables: %d\n"
        "📊 Total: %d",
        estado->criticos,
        estado->alertas,
        estado->estables,
        estado->pacientes_activos);
    lv_obj_set_style_text_font(info, &lv_font_montserrat_24, 0);
    lv_obj_align(info, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * btn_back = lv_button_create(cont);
    lv_obj_set_size(btn_back, 120, 40);
    lv_obj_align(btn_back, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_bg_color(btn_back, lv_color_hex(0x2196f3), 0);

    lv_obj_t * btn_label = lv_label_create(btn_back);
    lv_label_set_text(btn_label, "Regresar");
    lv_obj_set_style_text_color(btn_label, lv_color_white(), 0);
    lv_obj_center(btn_label);

  lv_obj_add_event_cb(btn_back, btn_back_click_cb, LV_EVENT_CLICKED, NULL);

}
static void btn_back_click_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);
    lv_obj_delete(lv_obj_get_parent(cont));
}
