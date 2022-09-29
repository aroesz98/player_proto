// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.0.5
// LVGL VERSION: 8.2
// PROJECT: audi

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
lv_obj_t * ui_Screen1;
lv_obj_t * ui_ImgButton1;
lv_obj_t * ui_Slider1;
lv_obj_t * ui_BottomPanel;
lv_obj_t * ui_ButtonOrder;
lv_obj_t * ui_LabelOrder;
lv_obj_t * ui_ButtonCheckout;
lv_obj_t * ui_LabelCheckout;
lv_obj_t * ui_Screen2;
lv_obj_t * ui_ImgButton2;
lv_obj_t * ui_Slider2;
lv_obj_t * ui_BottomPanel1;
lv_obj_t * ui_ButtonOrder1;
lv_obj_t * ui_LabelOrder1;
lv_obj_t * ui_ButtonCheckout1;
lv_obj_t * ui_LabelCheckout1;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "#error LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////
static void ui_event_ImgButton1(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Screen2, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0);
    }
}
static void ui_event_ButtonOrder(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        (e);
    }
}
static void ui_event_ButtonCheckout(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        (e);
    }
}
static void ui_event_ImgButton2(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        _ui_screen_change(ui_Screen1, LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0);
    }
}
static void ui_event_ButtonOrder1(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        (e);
    }
}
static void ui_event_ButtonCheckout1(lv_event_t * e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(event == LV_EVENT_CLICKED) {
        (e);
    }
}

///////////////////// SCREENS ////////////////////
void ui_Screen1_screen_init(void)
{

    // ui_Screen1

    ui_Screen1 = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_Screen1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Screen1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_ImgButton1

    ui_ImgButton1 = lv_imgbtn_create(ui_Screen1);
    lv_imgbtn_set_src(ui_ImgButton1, LV_IMGBTN_STATE_RELEASED, NULL, &ui_img_audi4_png, NULL);

    lv_obj_set_width(ui_ImgButton1, 600);
    lv_obj_set_height(ui_ImgButton1, 291);

    lv_obj_set_x(ui_ImgButton1, 0);
    lv_obj_set_y(ui_ImgButton1, 0);

    lv_obj_set_align(ui_ImgButton1, LV_ALIGN_CENTER);

    lv_obj_add_event_cb(ui_ImgButton1, ui_event_ImgButton1, LV_EVENT_ALL, NULL);

    // ui_Slider1

    ui_Slider1 = lv_slider_create(ui_Screen1);
    lv_slider_set_range(ui_Slider1, 0, 100);

    lv_obj_set_width(ui_Slider1, 400);
    lv_obj_set_height(ui_Slider1, 20);

    lv_obj_set_x(ui_Slider1, 0);
    lv_obj_set_y(ui_Slider1, 40);

    lv_obj_set_align(ui_Slider1, LV_ALIGN_TOP_MID);

    lv_obj_set_style_bg_color(ui_Slider1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Slider1, 100, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_Slider1, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Slider1, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_Slider1, lv_color_hex(0xFF0000), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Slider1, 100, LV_PART_KNOB | LV_STATE_DEFAULT);

    // ui_BottomPanel

    ui_BottomPanel = lv_obj_create(ui_Screen1);

    lv_obj_set_width(ui_BottomPanel, 600);
    lv_obj_set_height(ui_BottomPanel, 80);

    lv_obj_set_x(ui_BottomPanel, 0);
    lv_obj_set_y(ui_BottomPanel, -50);

    lv_obj_set_align(ui_BottomPanel, LV_ALIGN_BOTTOM_MID);

    lv_obj_clear_flag(ui_BottomPanel, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_BottomPanel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BottomPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_BottomPanel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_BottomPanel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_ButtonOrder

    ui_ButtonOrder = lv_btn_create(ui_BottomPanel);

    lv_obj_set_width(ui_ButtonOrder, 200);
    lv_obj_set_height(ui_ButtonOrder, 50);

    lv_obj_set_x(ui_ButtonOrder, 0);
    lv_obj_set_y(ui_ButtonOrder, 0);

    lv_obj_set_align(ui_ButtonOrder, LV_ALIGN_LEFT_MID);

    lv_obj_add_flag(ui_ButtonOrder, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_ButtonOrder, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_ButtonOrder, ui_event_ButtonOrder, LV_EVENT_ALL, NULL);
    lv_obj_set_style_bg_color(ui_ButtonOrder, lv_color_hex(0x505050), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ButtonOrder, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_LabelOrder

    ui_LabelOrder = lv_label_create(ui_ButtonOrder);

    lv_obj_set_width(ui_LabelOrder, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LabelOrder, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LabelOrder, 0);
    lv_obj_set_y(ui_LabelOrder, 0);

    lv_obj_set_align(ui_LabelOrder, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LabelOrder, "Order now!");

    lv_obj_set_style_text_font(ui_LabelOrder, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_ButtonCheckout

    ui_ButtonCheckout = lv_btn_create(ui_BottomPanel);

    lv_obj_set_width(ui_ButtonCheckout, 200);
    lv_obj_set_height(ui_ButtonCheckout, 50);

    lv_obj_set_x(ui_ButtonCheckout, 0);
    lv_obj_set_y(ui_ButtonCheckout, 0);

    lv_obj_set_align(ui_ButtonCheckout, LV_ALIGN_RIGHT_MID);

    lv_obj_add_flag(ui_ButtonCheckout, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_ButtonCheckout, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_ButtonCheckout, ui_event_ButtonCheckout, LV_EVENT_ALL, NULL);
    lv_obj_set_style_bg_color(ui_ButtonCheckout, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ButtonCheckout, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_LabelCheckout

    ui_LabelCheckout = lv_label_create(ui_ButtonCheckout);

    lv_obj_set_width(ui_LabelCheckout, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LabelCheckout, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LabelCheckout, 0);
    lv_obj_set_y(ui_LabelCheckout, 0);

    lv_obj_set_align(ui_LabelCheckout, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LabelCheckout, "Inventory");

    lv_obj_set_style_text_color(ui_LabelCheckout, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LabelCheckout, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LabelCheckout, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

}
void ui_Screen2_screen_init(void)
{

    // ui_Screen2

    ui_Screen2 = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_Screen2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Screen2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_ImgButton2

    ui_ImgButton2 = lv_imgbtn_create(ui_Screen2);
    lv_imgbtn_set_src(ui_ImgButton2, LV_IMGBTN_STATE_RELEASED, NULL, &ui_img_audi3_png, NULL);

    lv_obj_set_width(ui_ImgButton2, 800);
    lv_obj_set_height(ui_ImgButton2, 284);

    lv_obj_set_x(ui_ImgButton2, 0);
    lv_obj_set_y(ui_ImgButton2, 0);

    lv_obj_set_align(ui_ImgButton2, LV_ALIGN_CENTER);

    lv_obj_add_event_cb(ui_ImgButton2, ui_event_ImgButton2, LV_EVENT_ALL, NULL);

    // ui_Slider2

    ui_Slider2 = lv_slider_create(ui_Screen2);
    lv_slider_set_range(ui_Slider2, 0, 100);

    lv_obj_set_width(ui_Slider2, 400);
    lv_obj_set_height(ui_Slider2, 20);

    lv_obj_set_x(ui_Slider2, 0);
    lv_obj_set_y(ui_Slider2, 40);

    lv_obj_set_align(ui_Slider2, LV_ALIGN_TOP_MID);

    lv_obj_set_style_bg_color(ui_Slider2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Slider2, 100, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_Slider2, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Slider2, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_Slider2, lv_color_hex(0xFF0000), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Slider2, 100, LV_PART_KNOB | LV_STATE_DEFAULT);

    // ui_BottomPanel1

    ui_BottomPanel1 = lv_obj_create(ui_Screen2);

    lv_obj_set_width(ui_BottomPanel1, 600);
    lv_obj_set_height(ui_BottomPanel1, 80);

    lv_obj_set_x(ui_BottomPanel1, 0);
    lv_obj_set_y(ui_BottomPanel1, -50);

    lv_obj_set_align(ui_BottomPanel1, LV_ALIGN_BOTTOM_MID);

    lv_obj_clear_flag(ui_BottomPanel1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_BottomPanel1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_BottomPanel1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_BottomPanel1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_BottomPanel1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_ButtonOrder1

    ui_ButtonOrder1 = lv_btn_create(ui_BottomPanel1);

    lv_obj_set_width(ui_ButtonOrder1, 200);
    lv_obj_set_height(ui_ButtonOrder1, 50);

    lv_obj_set_x(ui_ButtonOrder1, 0);
    lv_obj_set_y(ui_ButtonOrder1, 0);

    lv_obj_set_align(ui_ButtonOrder1, LV_ALIGN_LEFT_MID);

    lv_obj_add_flag(ui_ButtonOrder1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_ButtonOrder1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_ButtonOrder1, ui_event_ButtonOrder1, LV_EVENT_ALL, NULL);
    lv_obj_set_style_bg_color(ui_ButtonOrder1, lv_color_hex(0x505050), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ButtonOrder1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_LabelOrder1

    ui_LabelOrder1 = lv_label_create(ui_ButtonOrder1);

    lv_obj_set_width(ui_LabelOrder1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LabelOrder1, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LabelOrder1, 0);
    lv_obj_set_y(ui_LabelOrder1, 0);

    lv_obj_set_align(ui_LabelOrder1, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LabelOrder1, "Order now!");

    lv_obj_set_style_text_font(ui_LabelOrder1, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_ButtonCheckout1

    ui_ButtonCheckout1 = lv_btn_create(ui_BottomPanel1);

    lv_obj_set_width(ui_ButtonCheckout1, 200);
    lv_obj_set_height(ui_ButtonCheckout1, 50);

    lv_obj_set_x(ui_ButtonCheckout1, 0);
    lv_obj_set_y(ui_ButtonCheckout1, 0);

    lv_obj_set_align(ui_ButtonCheckout1, LV_ALIGN_RIGHT_MID);

    lv_obj_add_flag(ui_ButtonCheckout1, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(ui_ButtonCheckout1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(ui_ButtonCheckout1, ui_event_ButtonCheckout1, LV_EVENT_ALL, NULL);
    lv_obj_set_style_bg_color(ui_ButtonCheckout1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ButtonCheckout1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_LabelCheckout1

    ui_LabelCheckout1 = lv_label_create(ui_ButtonCheckout1);

    lv_obj_set_width(ui_LabelCheckout1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_LabelCheckout1, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_LabelCheckout1, 0);
    lv_obj_set_y(ui_LabelCheckout1, 0);

    lv_obj_set_align(ui_LabelCheckout1, LV_ALIGN_CENTER);

    lv_label_set_text(ui_LabelCheckout1, "Inventory");

    lv_obj_set_style_text_color(ui_LabelCheckout1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LabelCheckout1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LabelCheckout1, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

}

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_Screen1_screen_init();
    ui_Screen2_screen_init();
    lv_disp_load_scr(ui_Screen1);
}

