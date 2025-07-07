#ifndef LV_DEMO_ESTADO_DETALLE_H
#define LV_DEMO_ESTADO_DETALLE_H

#include "lvgl/lvgl.h"

// Reutiliza la misma estructura que ya tienes
typedef struct {
    const char* nombre;
    int pacientes_activos;
    int criticos;
    int alertas;
    int estables;
    lv_color_t color_estado;
} estado_info_t;

void lv_demo_estado_detalle_show(estado_info_t * estado);

#endif // LV_DEMO_ESTADO_DETALLE_H
