/**
 * @file lv_demo_widgets.c
 *
 */
#include "lv_demo_widgets.h"
#include <stdio.h>
#if LV_USE_DEMO_WIDGETS

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN && LV_MEM_SIZE < (38ul * 1024ul)
    #error Insufficient memory for lv_demo_widgets. Please set LV_MEM_SIZE to at least 38KB (38ul * 1024ul).  48KB is recommended.
#endif

typedef enum {
    DISP_SMALL,
    DISP_MEDIUM,
    DISP_LARGE,
} disp_size_t;

// Estructura para datos de estados
typedef struct {
    const char* nombre;
    int pacientes_activos;
    int criticos;
    int alertas;
    int estables;
    lv_color_t color_estado;
} estado_info_t;

static void scroll_anim_y_cb(void * var, int32_t v);
static void slideshow_anim_completed_cb(lv_anim_t * a_old);
static void tabview_delete_event_cb(lv_event_t * e);
static void crear_tablero_general(lv_obj_t * tab);
static void crear_header_titulo(lv_obj_t * parent);
static void crear_filtros_busqueda(lv_obj_t * parent);
static void crear_tarjetas_resumen(lv_obj_t * parent);
static void crear_mapa_estados(lv_obj_t * parent);
static void crear_estados_prioritarios(lv_obj_t * parent);
static void estado_click_event_cb(lv_event_t * e);
static void popup_close_event_cb(lv_event_t * e);
static void popup_timer_cb(lv_timer_t * timer);

/**********************
 *  STATIC VARIABLES
 **********************/
static disp_size_t disp_size;

static lv_obj_t * tv;
static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_card;
static lv_style_t style_estado_critico;
static lv_style_t style_estado_alerta;
static lv_style_t style_estado_estable;

static const lv_font_t * font_large;
static const lv_font_t * font_normal;

// Datos estáticos de los estados (32 estados de México)
static estado_info_t estados_mexico[] = {
    {"Aguascalientes", 125, 3, 15, 107, {.red = 0, .green = 255, .blue = 0}},
    {"Baja California", 287, 8, 32, 247, {.red = 255, .green = 255, .blue = 0}},
    {"Baja California Sur", 95, 2, 8, 85, {.red = 0, .green = 255, .blue = 0}},
    {"Campeche", 78, 1, 5, 72, {.red = 0, .green = 255, .blue = 0}},
    {"Chiapas", 425, 15, 48, 362, {.red = 255, .green = 0, .blue = 0}},
    {"Chihuahua", 312, 9, 38, 265, {.red = 255, .green = 255, .blue = 0}},
    {"Ciudad de México", 658, 25, 85, 548, {.red = 255, .green = 0, .blue = 0}},
    {"Coahuila", 234, 7, 28, 199, {.red = 255, .green = 255, .blue = 0}},
    {"Colima", 67, 2, 6, 59, {.red = 0, .green = 255, .blue = 0}},
    {"Durango", 156, 4, 18, 134, {.red = 255, .green = 255, .blue = 0}},
    {"Guanajuato", 398, 12, 45, 341, {.red = 255, .green = 0, .blue = 0}},
    {"Guerrero", 298, 11, 35, 252, {.red = 255, .green = 255, .blue = 0}},
    {"Hidalgo", 187, 5, 22, 160, {.red = 255, .green = 255, .blue = 0}},
    {"Jalisco", 523, 18, 62, 443, {.red = 255, .green = 0, .blue = 0}},
    {"México", 612, 22, 78, 512, {.red = 255, .green = 0, .blue = 0}},
    {"Michoacán", 345, 13, 41, 291, {.red = 255, .green = 255, .blue = 0}},
    {"Morelos", 143, 4, 16, 123, {.red = 255, .green = 255, .blue = 0}},
    {"Nayarit", 89, 2, 9, 78, {.red = 0, .green = 255, .blue = 0}},
    {"Nuevo León", 421, 14, 58, 349, {.red = 255, .green = 0, .blue = 0}},
    {"Oaxaca", 267, 9, 31, 227, {.red = 255, .green = 255, .blue = 0}},
    {"Puebla", 456, 16, 52, 388, {.red = 255, .green = 0, .blue = 0}},
    {"Querétaro", 198, 6, 24, 168, {.red = 255, .green = 255, .blue = 0}},
    {"Quintana Roo", 132, 3, 14, 115, {.red = 0, .green = 255, .blue = 0}},
    {"San Luis Potosí", 213, 7, 26, 180, {.red = 255, .green = 255, .blue = 0}},
    {"Sinaloa", 245, 8, 29, 208, {.red = 255, .green = 255, .blue = 0}},
    {"Sonora", 198, 6, 23, 169, {.red = 255, .green = 255, .blue = 0}},
    {"Tabasco", 167, 5, 19, 143, {.red = 255, .green = 255, .blue = 0}},
    {"Tamaulipas", 278, 9, 33, 236, {.red = 255, .green = 255, .blue = 0}},
    {"Tlaxcala", 98, 2, 10, 86, {.red = 0, .green = 255, .blue = 0}},
    {"Veracruz", 543, 19, 65, 459, {.red = 255, .green = 0, .blue = 0}},
    {"Yucatán", 176, 5, 20, 151, {.red = 255, .green = 255, .blue = 0}},
    {"Zacatecas", 134, 3, 15, 116, {.red = 0, .green = 255, .blue = 0}}
};

void lv_demo_widgets(void)
{
    if(LV_HOR_RES <= 320) disp_size = DISP_SMALL;
    else if(LV_HOR_RES < 720) disp_size = DISP_MEDIUM;
    else disp_size = DISP_LARGE;

    font_large = LV_FONT_DEFAULT;
    font_normal = LV_FONT_DEFAULT;

    int32_t tab_h;
    if(disp_size == DISP_LARGE) {
        tab_h = 70;
#if LV_FONT_MONTSERRAT_24
        font_large     = &lv_font_montserrat_24;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_24 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
#if LV_FONT_MONTSERRAT_16
        font_normal    = &lv_font_montserrat_16;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_16 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
    }
    else if(disp_size == DISP_MEDIUM) {
        tab_h = 45;

#if LV_FONT_MONTSERRAT_14
        font_normal    = &lv_font_montserrat_14;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_14 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
    }
    else {   /* disp_size == DISP_SMALL */
        tab_h = 45;
#if LV_FONT_MONTSERRAT_18
        font_large     = &lv_font_montserrat_18;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_18 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
#if LV_FONT_MONTSERRAT_12
        font_normal    = &lv_font_montserrat_12;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_12 is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
    }

#if LV_USE_THEME_DEFAULT
    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK,
                          font_normal);
#endif

    // Inicializar estilos
    lv_style_init(&style_text_muted);
    lv_style_set_text_opa(&style_text_muted, LV_OPA_50);

    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);

    lv_style_init(&style_card);
    lv_style_set_radius(&style_card, 10);
    lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
    lv_style_set_bg_color(&style_card, lv_color_white());
    lv_style_set_border_width(&style_card, 2);
    lv_style_set_border_color(&style_card, lv_color_hex(0xe0e0e0));
    lv_style_set_shadow_width(&style_card, 8);
    lv_style_set_shadow_color(&style_card, lv_color_hex(0x999999));
    lv_style_set_shadow_ofs_x(&style_card, 3);
    lv_style_set_shadow_ofs_y(&style_card, 3);
    lv_style_set_pad_all(&style_card, 10);

    lv_style_init(&style_estado_critico);
    lv_style_set_bg_color(&style_estado_critico, lv_color_hex(0xff4444));
    lv_style_set_bg_opa(&style_estado_critico, LV_OPA_COVER);
    lv_style_set_border_width(&style_estado_critico, 2);
    lv_style_set_border_color(&style_estado_critico, lv_color_hex(0xcc0000));
    lv_style_set_radius(&style_estado_critico, 5);
    lv_style_set_pad_all(&style_estado_critico, 5);

    lv_style_init(&style_estado_alerta);
    lv_style_set_bg_color(&style_estado_alerta, lv_color_hex(0xffcc00));
    lv_style_set_bg_opa(&style_estado_alerta, LV_OPA_COVER);
    lv_style_set_border_width(&style_estado_alerta, 2);
    lv_style_set_border_color(&style_estado_alerta, lv_color_hex(0xcc9900));
    lv_style_set_radius(&style_estado_alerta, 5);
    lv_style_set_pad_all(&style_estado_alerta, 5);

    lv_style_init(&style_estado_estable);
    lv_style_set_bg_color(&style_estado_estable, lv_color_hex(0x44ff44));
    lv_style_set_bg_opa(&style_estado_estable, LV_OPA_COVER);
    lv_style_set_border_width(&style_estado_estable, 2);
    lv_style_set_border_color(&style_estado_estable, lv_color_hex(0x00cc00));
    lv_style_set_radius(&style_estado_estable, 5);
    lv_style_set_pad_all(&style_estado_estable, 5);

    tv = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_size(tv, tab_h);
    lv_obj_add_event_cb(tv, tabview_delete_event_cb, LV_EVENT_DELETE, NULL);

    lv_obj_set_style_text_font(lv_screen_active(), font_normal, 0);

    // Crear las dos pestañas
    lv_obj_t * t1 = lv_tabview_add_tab(tv, "Tablero general");
    lv_obj_t * t2 = lv_tabview_add_tab(tv, "Historico");

    // Crear contenido del tablero general
    crear_tablero_general(t1);
}

static void crear_tablero_general(lv_obj_t * tab)
{
    // Crear scroll container principal
    lv_obj_t * scroll = lv_obj_create(tab);
    lv_obj_set_size(scroll, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(scroll, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(scroll, 5, 0);

    // 1. Header con título
    crear_header_titulo(scroll);

    // 2. Filtros y búsqueda
    crear_filtros_busqueda(scroll);

    // Crear contenedor principal horizontal
    lv_obj_t * main_container = lv_obj_create(scroll);
    lv_obj_set_size(main_container, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(main_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(main_container, 0, 0);
    lv_obj_set_style_border_width(main_container, 0, 0);
    lv_obj_set_style_bg_opa(main_container, LV_OPA_TRANSP, 0);

    // Columna izquierda (tarjetas resumen + estados prioritarios)
    lv_obj_t * left_column = lv_obj_create(main_container);
    lv_obj_set_size(left_column, LV_PCT(30), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(left_column, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(left_column, 5, 0);
    lv_obj_set_style_border_width(left_column, 0, 0);
    lv_obj_set_style_bg_opa(left_column, LV_OPA_TRANSP, 0);

    // 3. Tarjetas de resumen
    crear_tarjetas_resumen(left_column);

    // 4. Estados prioritarios
    crear_estados_prioritarios(left_column);

    // Columna derecha (mapa de estados)
    lv_obj_t * right_column = lv_obj_create(main_container);
    lv_obj_set_size(right_column, LV_PCT(70), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(right_column, 5, 0);
    lv_obj_set_style_border_width(right_column, 0, 0);
    lv_obj_set_style_bg_opa(right_column, LV_OPA_TRANSP, 0);

    // 5. Mapa de estados
    crear_mapa_estados(right_column);
}

static void crear_header_titulo(lv_obj_t * parent)
{
    lv_obj_t * header = lv_obj_create(parent);
    lv_obj_set_size(header, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_add_style(header, &style_card, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x2196f3), 0);

    lv_obj_t * title = lv_label_create(header);
    lv_label_set_text(title, "Monitoreo Nacional de Pacientes");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, 0);

    lv_obj_t * subtitle = lv_label_create(header);
    lv_label_set_text(subtitle, "10,240 pacientes activos en 32 estados");
    lv_obj_set_style_text_color(subtitle, lv_color_white(), 0);
    lv_obj_align(subtitle, LV_ALIGN_TOP_LEFT, 0, 30);
}

static void crear_filtros_busqueda(lv_obj_t * parent)
{
    lv_obj_t * filter_container = lv_obj_create(parent);
    lv_obj_set_size(filter_container, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(filter_container, LV_FLEX_FLOW_ROW);
    lv_obj_add_style(filter_container, &style_card, 0);

    // Barra de búsqueda
    lv_obj_t * search_bar = lv_textarea_create(filter_container);
    lv_obj_set_size(search_bar, LV_PCT(40), 40);
    lv_textarea_set_placeholder_text(search_bar, "Buscar estado o paciente...");
    lv_textarea_set_one_line(search_bar, true);

    // Dropdown tipo de monitoreo
    lv_obj_t * dropdown1 = lv_dropdown_create(filter_container);
    lv_obj_set_size(dropdown1, LV_PCT(25), 40);
    lv_dropdown_set_options(dropdown1, "Todos\nPost-operatorio\nCrónicos\nEmergencia");
    lv_dropdown_set_selected(dropdown1, 0);

    // Dropdown rango de fechas
    lv_obj_t * dropdown2 = lv_dropdown_create(filter_container);
    lv_obj_set_size(dropdown2, LV_PCT(25), 40);
    lv_dropdown_set_options(dropdown2, "Hoy\nÚltimos 7 días\nÚltimos 30 días\nTodos");
    lv_dropdown_set_selected(dropdown2, 0);
}

static void crear_tarjetas_resumen(lv_obj_t * parent)
{
    lv_obj_t * title = lv_label_create(parent);
    lv_label_set_text(title, "Resumen Nacional");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x333333), 0);

    // Tarjeta total
    lv_obj_t * card_total = lv_obj_create(parent);
    lv_obj_set_size(card_total, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_add_style(card_total, &style_card, 0);
    lv_obj_set_style_bg_color(card_total, lv_color_hex(0x4caf50), 0);

    lv_obj_t * total_label = lv_label_create(card_total);
    lv_label_set_text(total_label, "Total Pacientes\n10,240");
    lv_obj_set_style_text_color(total_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(total_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(total_label);

    // Tarjeta críticos
    lv_obj_t * card_criticos = lv_obj_create(parent);
    lv_obj_set_size(card_criticos, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_add_style(card_criticos, &style_card, 0);
    lv_obj_set_style_bg_color(card_criticos, lv_color_hex(0xf44336), 0);

    lv_obj_t * criticos_label = lv_label_create(card_criticos);
    lv_label_set_text(criticos_label, "🔴 Críticos\n342");
    lv_obj_set_style_text_color(criticos_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(criticos_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(criticos_label);

    // Tarjeta alertas
    lv_obj_t * card_alertas = lv_obj_create(parent);
    lv_obj_set_size(card_alertas, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_add_style(card_alertas, &style_card, 0);
    lv_obj_set_style_bg_color(card_alertas, lv_color_hex(0xff9800), 0);

    lv_obj_t * alertas_label = lv_label_create(card_alertas);
    lv_label_set_text(alertas_label, "🟡 En Alerta\n1,158");
    lv_obj_set_style_text_color(alertas_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(alertas_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(alertas_label);

    // Tarjeta estables
    lv_obj_t * card_estables = lv_obj_create(parent);
    lv_obj_set_size(card_estables, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_add_style(card_estables, &style_card, 0);
    lv_obj_set_style_bg_color(card_estables, lv_color_hex(0x4caf50), 0);

    lv_obj_t * estables_label = lv_label_create(card_estables);
    lv_label_set_text(estables_label, "🟢 Estables\n8,740");
    lv_obj_set_style_text_color(estables_label, lv_color_white(), 0);
    lv_obj_set_style_text_align(estables_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(estables_label);
}

static void crear_estados_prioritarios(lv_obj_t * parent)
{
    lv_obj_t * title = lv_label_create(parent);
    lv_label_set_text(title, "Estados Prioritarios");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x333333), 0);

    // Lista de estados prioritarios (top 5)
    const char* estados_prioritarios[] = {
        "Ciudad de México\n🔴 25 críticos | 85 alertas",
        "Nuevo León\n🔴 14 críticos | 58 alertas", 
        "Jalisco\n🔴 18 críticos | 62 alertas",
        "Veracruz\n🔴 19 críticos | 65 alertas",
        "Estado de México\n🔴 22 críticos | 78 alertas"
    };

    for(int i = 0; i < 5; i++) {
        lv_obj_t * estado_card = lv_obj_create(parent);
        lv_obj_set_size(estado_card, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_add_style(estado_card, &style_card, 0);
        lv_obj_set_style_bg_color(estado_card, lv_color_hex(0xfff3e0), 0);

        lv_obj_t * estado_label = lv_label_create(estado_card);
        lv_label_set_text(estado_label, estados_prioritarios[i]);
        lv_obj_set_style_text_color(estado_label, lv_color_hex(0x333333), 0);
        lv_obj_align(estado_label, LV_ALIGN_CENTER, 0, 0);
    }
}

static void crear_mapa_estados(lv_obj_t * parent)
{
    lv_obj_t * title = lv_label_create(parent);
    lv_label_set_text(title, "Mapa Interactivo - Estados de México");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x333333), 0);

    // Contenedor del mapa
    lv_obj_t * mapa_container = lv_obj_create(parent);
    lv_obj_set_size(mapa_container, LV_PCT(100), 350);
    lv_obj_add_style(mapa_container, &style_card, 0);
    lv_obj_set_style_bg_color(mapa_container, lv_color_hex(0xf5f5f5), 0);

    // Crear grid para los estados (8x4 = 32 estados)
    lv_obj_set_layout(mapa_container, LV_LAYOUT_GRID);
    
    // Configurar columnas del grid
    static int32_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), 
                                LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), 
                                LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), 
                                LV_GRID_TEMPLATE_LAST};
    
    lv_obj_set_grid_dsc_array(mapa_container, col_dsc, row_dsc);

    // Crear rectangulos para cada estado
    for(int i = 0; i < 32; i++) {
        lv_obj_t * estado_btn = lv_button_create(mapa_container);
        
        // Posición en el grid
        int col = i % 8;
        int row = i / 8;
        lv_obj_set_grid_cell(estado_btn, LV_GRID_ALIGN_STRETCH, col, 1, 
                             LV_GRID_ALIGN_STRETCH, row, 1);
        
        // Aplicar estilo según el estado
        if(estados_mexico[i].criticos > 10) {
            lv_obj_add_style(estado_btn, &style_estado_critico, 0);
        } else if(estados_mexico[i].alertas > 20) {
            lv_obj_add_style(estado_btn, &style_estado_alerta, 0);
        } else {
            lv_obj_add_style(estado_btn, &style_estado_estable, 0);
        }

        // Crear etiqueta del estado
        lv_obj_t * estado_label = lv_label_create(estado_btn);
        static char estado_text[64];
        snprintf(estado_text, sizeof(estado_text), "%s\n%d", 
                estados_mexico[i].nombre, estados_mexico[i].pacientes_activos);
        lv_label_set_text(estado_label, estado_text);
        lv_obj_set_style_text_color(estado_label, lv_color_black(), 0);
        lv_obj_set_style_text_align(estado_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_center(estado_label);

        // Agregar evento click
        lv_obj_add_event_cb(estado_btn, estado_click_event_cb, LV_EVENT_CLICKED, &estados_mexico[i]);
    }
}

static void estado_click_event_cb(lv_event_t * e)
{
    estado_info_t * estado = (estado_info_t *)lv_event_get_user_data(e);
    
    // Crear popup con información detallada del estado
    lv_obj_t * popup = lv_obj_create(lv_screen_active());
    lv_obj_set_size(popup, 300, 200);
    lv_obj_center(popup);
    lv_obj_add_style(popup, &style_card, 0);
    lv_obj_set_style_bg_color(popup, lv_color_white(), 0);
    
    // Título del popup
    lv_obj_t * popup_title = lv_label_create(popup);
    lv_label_set_text_fmt(popup_title, "Estado: %s", estado->nombre);
    lv_obj_add_style(popup_title, &style_title, 0);
    lv_obj_align(popup_title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Información detallada
    lv_obj_t * info_label = lv_label_create(popup);
    lv_label_set_text_fmt(info_label, 
        "Pacientes Activos: %d\n"
        "🔴 Críticos: %d\n"
        "🟡 En Alerta: %d\n"
        "🟢 Estables: %d",
        estado->pacientes_activos,
        estado->criticos,
        estado->alertas,
        estado->estables);
    lv_obj_align(info_label, LV_ALIGN_CENTER, 0, 0);
    
    // Botón cerrar
    lv_obj_t * close_btn = lv_button_create(popup);
    lv_obj_set_size(close_btn, 80, 30);
    lv_obj_align(close_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    lv_obj_t * close_label = lv_label_create(close_btn);
    lv_label_set_text(close_label, "Cerrar");
    lv_obj_center(close_label);
    
    // Evento para cerrar el popup
    lv_obj_add_event_cb(close_btn, popup_close_event_cb, LV_EVENT_CLICKED, popup);
    
    // Auto-cerrar después de 5 segundos
    lv_timer_create(popup_timer_cb, 5000, popup);
}

static void popup_close_event_cb(lv_event_t * e)
{
    lv_obj_t * popup = (lv_obj_t *)lv_event_get_user_data(e);
    lv_obj_delete(popup);
}

static void popup_timer_cb(lv_timer_t * timer)
{
    lv_obj_t * popup = (lv_obj_t *)timer->user_data;
    if(popup && lv_obj_is_valid(popup)) {
        lv_obj_delete(popup);
    }
    lv_timer_delete(timer);
}

void lv_demo_widgets_start_slideshow(void)
{
    lv_obj_update_layout(tv);

    lv_obj_t * cont = lv_tabview_get_content(tv);

    lv_obj_t * tab = lv_obj_get_child(cont, 0);

    int32_t v = lv_obj_get_scroll_bottom(tab);
    uint32_t t = lv_anim_speed(lv_display_get_dpi(NULL));
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, scroll_anim_y_cb);
    lv_anim_set_duration(&a, t);
    lv_anim_set_playback_duration(&a, t);
    lv_anim_set_values(&a, 0, v);
    lv_anim_set_var(&a, tab);
    lv_anim_set_completed_cb(&a, slideshow_anim_completed_cb);
    lv_anim_start(&a);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void scroll_anim_y_cb(void * var, int32_t v)
{
    lv_obj_scroll_to_y(var, v, LV_ANIM_OFF);
}

static void slideshow_anim_completed_cb(lv_anim_t * a_old)
{
    LV_UNUSED(a_old);

    lv_obj_t * cont = lv_tabview_get_content(tv);
    uint32_t tab_id = lv_tabview_get_tab_active(tv);
    tab_id += 1;
    if(tab_id > 1) tab_id = 0;  // Solo 2 pestañas ahora
    lv_tabview_set_active(tv, tab_id, LV_ANIM_ON);

    lv_obj_t * tab = lv_obj_get_child(cont, tab_id);
    lv_obj_scroll_to_y(tab, 0, LV_ANIM_OFF);
    lv_obj_update_layout(tv);

    int32_t v = lv_obj_get_scroll_bottom(tab);
    uint32_t t = lv_anim_speed(lv_display_get_dpi(NULL));

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, scroll_anim_y_cb);
    lv_anim_set_duration(&a, t);
    lv_anim_set_playback_duration(&a, t);
    lv_anim_set_values(&a, 0, v);
    lv_anim_set_var(&a, tab);
    lv_anim_set_completed_cb(&a, slideshow_anim_completed_cb);
    lv_anim_start(&a);
}

static void tabview_delete_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_DELETE) {
        lv_style_reset(&style_text_muted);
        lv_style_reset(&style_title);
        lv_style_reset(&style_card);
        lv_style_reset(&style_estado_critico);
        lv_style_reset(&style_estado_alerta);
        lv_style_reset(&style_estado_estable);
    }
}

#endif