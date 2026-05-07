#include "driver/lvgl/src/core/lv_obj.h"
#include "driver/lv_port_disp.h"
#include "gui_thread.h"


extern volatile bool g_flush_done;

                /* GUI Thread entry function */
void gui_thread_entry(void)
{
    /* 1. LVGL 核心初始化 */
    lv_init();
    // /* 2. 显示驱动初始化 */
    lv_port_disp_init();

    lv_obj_t * screen = lv_scr_act();

/* 2. (可选) 手动设置屏幕背景颜色，防止黑屏 */
lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), 0); // 设置为黑色背景
lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

/* 3. 创建 Label 对象 */
lv_obj_t * label = lv_label_create(screen);

/* 4. 设置显示的文本 */
lv_label_set_text(label, "Hello World");

/* 5. 手动配置文字样式 (因为关闭了主题，默认可能看不见) */
// 设置文字颜色为白色
lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
// 设置使用的字体 (确保 lv_conf.h 中开启了对应的字体，例如 MONTSERRAT_14)
lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);

/* 6. 关键步骤：将 Label 置于父对象（即屏幕）的中心 */
lv_obj_center(label);

    /* 4. 任务循环 */
    while (1)
    {
        // lv_tick_inc(5);
        

        lv_timer_handler();

        if (g_flush_done)
        {
            g_flush_done = false;

            lv_disp_t * disp = lv_disp_get_default();
            if (disp)
            {
                lv_disp_flush_ready(disp->driver);
            }
        }
        tx_thread_sleep(1); 
    }
}
