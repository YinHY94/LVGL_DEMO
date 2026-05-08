#include "driver/lvgl/src/core/lv_obj.h"
#include "driver/lv_port_disp.h"
#include "driver/lv_port_indev.h"
#include "gui_thread.h"

static lv_style_t style_dot;

void touch_test_init(void);


// 在文件头部定义一个颜色计数器
static uint16_t hue_counter = 0;

void touch_test_init(void)
{
    /* 1. 初始化基础样式 */
    lv_style_init(&style_dot);
    lv_style_set_radius(&style_dot, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&style_dot, LV_OPA_COVER);
    
    // 让点小一点，更精致
    lv_style_set_width(&style_dot, 10);
    lv_style_set_height(&style_dot, 10);
    
    // 边框和发光
    lv_style_set_border_width(&style_dot, 1);
    lv_style_set_border_color(&style_dot, lv_color_white());
    lv_style_set_shadow_width(&style_dot, 10);
    lv_style_set_shadow_spread(&style_dot, 2);

    /* 2. 坐标标签美化 */
    lv_obj_t * label_pos = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(label_pos, &lv_font_montserrat_14, 0); // 假设你启用了该字体
    lv_obj_set_style_text_color(label_pos, lv_color_hex(0x00FF00), 0);
    lv_label_set_text(label_pos, "System Ready: Track Touch");
    lv_obj_align(label_pos, LV_ALIGN_TOP_MID, 0, 5);
    
    /* 3. 创建全屏追踪区域 */
    lv_obj_t * canvas = lv_obj_create(lv_scr_act());
    lv_obj_set_size(canvas, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(canvas, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(canvas, 0, 0);
    
    /* 4. 事件回调 */
    lv_obj_add_event_cb(canvas, [](lv_event_t * e) {
        lv_obj_t * label = (lv_obj_t *)lv_event_get_user_data(e);
        lv_indev_t * indev = lv_indev_get_act();
        if(!indev) return;

        lv_point_t vect;
        lv_indev_get_point(indev, &vect);

        // --- 核心变化：自动变色逻辑 ---
        hue_counter = (hue_counter + 5) % 360; // 每次点击颜色偏移 5 度
        lv_color_t dynamic_color = lv_color_hsv_to_rgb(hue_counter, 80, 100);

        /* 创建圆点 */
        lv_obj_t * dot = lv_obj_create(lv_scr_act());
        lv_obj_add_style(dot, &style_dot, 0);
        
        // 设置动态颜色
        lv_obj_set_style_bg_color(dot, dynamic_color, 0);
        lv_obj_set_style_shadow_color(dot, dynamic_color, 0);
        
        /* 居中放置 */
        lv_obj_set_pos(dot, vect.x - 5, vect.y - 5);
        
        /* 5. 涟漪动画：从小变大 + 渐隐 */
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, dot);
        lv_anim_set_time(&a, 500); // 持续 0.5 秒
        
        // 透明度动画
        lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
        lv_anim_set_exec_cb(&a, [](void * var, int32_t v) {
            lv_obj_set_style_opa((lv_obj_t *)var, v, 0);
        });
        lv_anim_start(&a);

        /* 更新文字 */
        lv_label_set_text_fmt(label, "#%06X | X:%d Y:%d", dynamic_color.full, vect.x, vect.y);
        lv_obj_set_style_text_color(label, dynamic_color, 0);
        
        /* 延迟删除 */
        lv_obj_del_delayed(dot, 500);

    }, LV_EVENT_PRESSING, label_pos);
}

                /* GUI Thread entry function */
void gui_thread_entry(void)
{
    /* 1. LVGL 核心初始化 */
    lv_init();
    // /* 2. 显示驱动初始化 */
    lv_port_disp_init();
    lv_port_indev_init();


    touch_test_init();
    /* 4. 任务循环 */
    while (1)
    {

        lv_timer_handler();

        tx_thread_sleep(1); 
    }
}
