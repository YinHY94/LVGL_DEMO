/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/


#include "r_gpt.h"
#include "r_timer_api.h"
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include "r_spi.h"
#include "r_spi_api.h"
#include "tx_api.h"
#include "common_data.h"
#include "hal_data.h"
/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
    //#warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 320 is used for now.
    #define MY_DISP_HOR_RES    320
#endif

#ifndef MY_DISP_VER_RES
    //#warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen height, default value 240 is used for now.
    #define MY_DISP_VER_RES    240
#endif

/* ST7789 Commands */
#define ST7789_SLPOUT       0x11
#define ST7789_COLMOD       0x3A
#define ST7789_MADCTL       0x36
#define ST7789_CASET        0x2A
#define ST7789_RASET        0x2B
#define ST7789_RAMWR        0x2C
#define ST7789_DISPON       0x29

/* Pin Mapping */
#define OLED_CS_PIN         oled_spi_cs
#define OLED_DC_PIN         oled_spi_dc    
#define OLED_RESET_PIN      oled_spi_rst
/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
    spi_idle=0,
    spi_cmd,
    spi_data
} spi_send_status;
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);
static void lcd_send_cmd(uint8_t cmd, const uint8_t * data, size_t len);
/**********************
 *  STATIC VARIABLES
 **********************/
static lv_disp_drv_t g_disp_drv;
static volatile spi_send_status g_spi_status = spi_idle;
/**********************
 *      MACROS
 **********************/

 /**********************
 *  GLOBAL VARIABLES
 **********************/

volatile bool g_flush_done = false;
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    // /* Example for 1) */
    // static lv_disp_draw_buf_t draw_buf_dsc_1;
    // static lv_color_t buf_1[MY_DISP_HOR_RES * 10];                          /*A buffer for 10 rows*/
    // lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

    /* Example for 2) */
    static lv_disp_draw_buf_t draw_buf_dsc_2;
    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 24];                        /*A buffer for 10 rows*/
    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 24];                        /*An other buffer for 10 rows*/
    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 24);   /*Initialize the display buffer*/

    // /* Example for 3) also set disp_drv.full_refresh = 1 below*/
    // static lv_disp_draw_buf_t draw_buf_dsc_3;
    // static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*A screen sized buffer*/
    // static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*Another screen sized buffer*/
    // lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,
    //                       MY_DISP_VER_RES * LV_VER_RES_MAX);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    //static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&g_disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    g_disp_drv.hor_res = MY_DISP_HOR_RES;
    g_disp_drv.ver_res = MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    g_disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    g_disp_drv.draw_buf = &draw_buf_dsc_2;

    /*Required for Example 3)*/
    //disp_drv.full_refresh = 1;

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&g_disp_drv);
}

void oled_spi_callback(spi_callback_args_t * p_args)
{
    if (SPI_EVENT_TRANSFER_COMPLETE == p_args->event)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_CS_PIN, BSP_IO_LEVEL_HIGH);
        if (g_spi_status == spi_data)
        {
            g_flush_done = true;
        }

        g_spi_status = spi_idle;
        
    }
}

void lvgl_tick_callback(timer_callback_args_t *p_args)
{
    if (p_args->event == TIMER_EVENT_CYCLE_END)
    {
        lv_tick_inc(1);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lcd_send_cmd(uint8_t cmd, const uint8_t * data, size_t len)
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_CS_PIN, BSP_IO_LEVEL_LOW);
    R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_DC_PIN, BSP_IO_LEVEL_LOW);

    g_spi_status= spi_cmd;
    R_SPI_Write(&oled_spi_ctrl, &cmd, 1, SPI_BIT_WIDTH_8_BITS);
    while(g_spi_status == spi_cmd);
    

    if (len > 0)
    {
        g_spi_status= spi_cmd;
        R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_DC_PIN, BSP_IO_LEVEL_HIGH);
        R_SPI_Write(&oled_spi_ctrl, data, len, SPI_BIT_WIDTH_8_BITS);
        while(g_spi_status == spi_cmd);
    }

    R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_CS_PIN, BSP_IO_LEVEL_HIGH);
}

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    R_SPI_Open(&oled_spi_ctrl, &oled_spi_cfg);

    R_GPT_Open(&lvgl_tick_timer_ctrl, &lvgl_tick_timer_cfg);
    R_GPT_Start(&lvgl_tick_timer_ctrl);
    R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_RESET_PIN, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_RESET_PIN, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);

    /* ST7789 初始化 */
    lcd_send_cmd(ST7789_SLPOUT, NULL, 0); 
    
    uint8_t colmod = 0x55; /* 16-bit RGB565 */
    lcd_send_cmd(ST7789_COLMOD, &colmod, 1);
    
    uint8_t madctl = 0x00; /* 根据实际屏幕调整方向 */
    lcd_send_cmd(ST7789_MADCTL, &madctl, 1);
    
    lcd_send_cmd(ST7789_DISPON, NULL, 0);
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    if (g_spi_status != spi_idle)
    {
        return;
    }

    if((!disp_flush_enabled)) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    uint8_t addr[4];
    
    /* 设置列范围 */
    addr[0] = (uint8_t)(area->x1 >> 8);
    addr[1] = (uint8_t)(area->x1 & 0xFF);
    addr[2] = (uint8_t)(area->x2 >> 8);
    addr[3] = (uint8_t)(area->x2 & 0xFF);
    lcd_send_cmd(ST7789_CASET, addr, 4);

    /* 设置行范围 */
    addr[0] = (uint8_t)(area->y1 >> 8);
    addr[1] = (uint8_t)(area->y1 & 0xFF);
    addr[2] = (uint8_t)(area->y2 >> 8);
    addr[3] = (uint8_t)(area->y2 & 0xFF);
    lcd_send_cmd(ST7789_RASET, addr, 4);

    /* 准备写入 RAM */
    lcd_send_cmd(ST7789_RAMWR, NULL, 0);


    R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_CS_PIN, BSP_IO_LEVEL_LOW);
    R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_DC_PIN, BSP_IO_LEVEL_HIGH); 

    /* 计算像素字节数 (RGB565 = 2 bytes per pixel) */
    uint32_t size = (uint32_t)((lv_area_get_width(area) * lv_area_get_height(area)) * sizeof(lv_color_t));

    /* 开启非阻塞传输 */
    g_spi_status = spi_data;
    fsp_err_t err = R_SPI_Write(&oled_spi_ctrl, (uint8_t *)color_p, size, SPI_BIT_WIDTH_8_BITS);

    if (FSP_SUCCESS != err)
    {
        /* 启动失败，必须立即释放 LVGL */
        R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_CS_PIN, BSP_IO_LEVEL_HIGH);
        g_spi_status = spi_idle;
        lv_disp_flush_ready(disp_drv);
    }

}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
