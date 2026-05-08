/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/


#include "gui_thread.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#include "tx_api.h"
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include "r_spi.h"
#include "r_spi_api.h"
#include "common_data.h"
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
            lv_disp_flush_ready(&g_disp_drv);
        }
        tx_semaphore_put(&g_oled_spi_semaphore);
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
    tx_semaphore_get(&g_oled_spi_semaphore, TX_WAIT_FOREVER);
    R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_CS_PIN, BSP_IO_LEVEL_LOW);
    R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_DC_PIN, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
    g_spi_status= spi_cmd;
    R_SPI_Write(&oled_spi_ctrl, &cmd, 1, SPI_BIT_WIDTH_8_BITS);

    

    if (len > 0)
    {
        tx_semaphore_get(&g_oled_spi_semaphore, TX_WAIT_FOREVER);
        g_spi_status= spi_cmd;
        R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_CS_PIN, BSP_IO_LEVEL_LOW);
        R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_DC_PIN, BSP_IO_LEVEL_HIGH);
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
        R_SPI_Write(&oled_spi_ctrl, data, len, SPI_BIT_WIDTH_8_BITS);
        while(g_spi_status == spi_cmd);
    }

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
    lcd_send_cmd(0x11, NULL, 0); // Sleep out
    R_BSP_SoftwareDelay(120, BSP_DELAY_UNITS_MILLISECONDS);

    uint8_t colmod = 0x05; // 厂家使用的 16-bit 模式值
    lcd_send_cmd(0x3A, &colmod, 1);

    uint8_t b2[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
    lcd_send_cmd(0xB2, b2, 5); // Porch Setting

    uint8_t b7 = 0x35; 
    lcd_send_cmd(0xB7, &b7, 1); // Gate Control

    uint8_t bb = 0x32; 
    lcd_send_cmd(0xBB, &bb, 1); // VCOM Setting

    uint8_t c0 = 0x2C;
    lcd_send_cmd(0xC0, &c0, 1); // LCM Control

    uint8_t c2 = 0x01;
    lcd_send_cmd(0xC2, &c2, 1); // VDV and VRH Command Enable

    uint8_t c3 = 0x15;
    lcd_send_cmd(0xC3, &c3, 1); // VRH Set

    uint8_t c4 = 0x20;
    lcd_send_cmd(0xC4, &c4, 1); // VDV Set

    uint8_t c6 = 0x0F;
    lcd_send_cmd(0xC6, &c6, 1); // Frame Rate Control

    uint8_t d0[] = {0xA4, 0xA1};
    lcd_send_cmd(0xD0, d0, 2); // Power Control

    uint8_t e0[] = {0xD0, 0x08, 0x0E, 0x09, 0x09, 0x05, 0x31, 0x33, 0x48, 0x17, 0x14, 0x15, 0x31, 0x34};
    lcd_send_cmd(0xE0, e0, 14); // Positive Voltage Gamma

    uint8_t e1[] = {0xD0, 0x08, 0x0E, 0x09, 0x09, 0x15, 0x31, 0x33, 0x48, 0x17, 0x14, 0x15, 0x31, 0x34};
    lcd_send_cmd(0xE1, e1, 14); // Negative Voltage Gamma

    uint8_t madctl = 0x60; // 0x00为竖屏，0x70或0x60为横屏
    lcd_send_cmd(0x36, &madctl, 1);

    lcd_send_cmd(0x21, NULL, 0); // 开启颜色反相（ST7789常见需求）
    lcd_send_cmd(0x29, NULL, 0); // Display ON
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

    tx_semaphore_get(&g_oled_spi_semaphore, TX_WAIT_FOREVER);

    R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_CS_PIN, BSP_IO_LEVEL_LOW);
    R_IOPORT_PinWrite(&g_ioport_ctrl, OLED_DC_PIN, BSP_IO_LEVEL_HIGH); 
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);

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
