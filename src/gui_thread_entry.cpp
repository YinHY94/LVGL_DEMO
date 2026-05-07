#include "driver/lvgl/src/core/lv_obj.h"
#include "driver/lv_port_disp.h"
#include "gui_thread.h"



                /* GUI Thread entry function */
void gui_thread_entry(void)
{
    /* 1. LVGL 核心初始化 */
    lv_init();
    // /* 2. 显示驱动初始化 */
    lv_port_disp_init();

// lv_obj_t * scr = lv_scr_act();

// /* 2. 设置屏幕的背景颜色为绿色 */
// // lv_palette_main(LV_PALETTE_GREEN) 是 LVGL 内置的标准绿色
// lv_obj_set_style_bg_color(scr, lv_palette_main(LV_PALETTE_GREEN), 0);

// /* 3. 设置背景的不透明度 */
// // 必须设置为 LV_OPA_COVER（100%不透明），否则颜色可能无法显示
// lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

lv_obj_t * label = lv_label_create(lv_scr_act());
    
    /* 设置文字内容 */
    lv_label_set_text(label, "Hello world");
    
    /* 将标签居中显示 */
    lv_obj_center(label);

    /* 4. 任务循环 */
    while (1)
    {
        // lv_tick_inc(5);
        

        lv_timer_handler();

        tx_thread_sleep(1); 
    }
}
