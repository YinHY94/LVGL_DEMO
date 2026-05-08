/* generated thread header file - do not edit */
#ifndef GUI_THREAD_H_
#define GUI_THREAD_H_
#include "bsp_api.h"
                #include "tx_api.h"
                #include "hal_data.h"
                #ifdef __cplusplus
                extern "C" void gui_thread_entry(void);
                #else
                extern void gui_thread_entry(void);
                #endif
#include "r_iic_master.h"
#include "r_i2c_master_api.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#include "r_dmac.h"
#include "r_transfer_api.h"
#include "r_spi.h"
FSP_HEADER
/* I2C Master on IIC Instance. */
extern const i2c_master_instance_t touchpad_iic;

/** Access the I2C Master instance using these structures when calling API functions directly (::p_api is not used). */
extern iic_master_instance_ctrl_t touchpad_iic_ctrl;
extern const i2c_master_cfg_t touchpad_iic_cfg;

#ifndef touchpad_iic_callback
void touchpad_iic_callback(i2c_master_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t lvgl_tick_timer;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t lvgl_tick_timer_ctrl;
extern const timer_cfg_t lvgl_tick_timer_cfg;

#ifndef lvgl_tick_callback
void lvgl_tick_callback(timer_callback_args_t * p_args);
#endif
/* Transfer on DMAC Instance. */
extern const transfer_instance_t g_transfer1;

/** Access the DMAC instance using these structures when calling API functions directly (::p_api is not used). */
extern dmac_instance_ctrl_t g_transfer1_ctrl;
extern const transfer_cfg_t g_transfer1_cfg;

#ifndef oled_spi_tx_transfer_callback
void oled_spi_tx_transfer_callback(transfer_callback_args_t * p_args);
#endif
/** SPI on SPI Instance. */
extern const spi_instance_t oled_spi;

/** Access the SPI instance using these structures when calling API functions directly (::p_api is not used). */
extern spi_instance_ctrl_t oled_spi_ctrl;
extern const spi_cfg_t oled_spi_cfg;

/** Callback used by SPI Instance. */
#ifndef oled_spi_callback
void oled_spi_callback(spi_callback_args_t * p_args);
#endif


#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == g_transfer1)
    #define oled_spi_P_TRANSFER_TX (NULL)
#else
    #define oled_spi_P_TRANSFER_TX (&g_transfer1)
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
    #define oled_spi_P_TRANSFER_RX (NULL)
#else
    #define oled_spi_P_TRANSFER_RX (&RA_NOT_DEFINED)
#endif
#undef RA_NOT_DEFINED
FSP_FOOTER
#endif /* GUI_THREAD_H_ */
