/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_dmac.h"
#include "r_transfer_api.h"
#include "r_spi.h"
FSP_HEADER
/* Transfer on DMAC Instance. */
extern const transfer_instance_t g_transfer0;

/** Access the DMAC instance using these structures when calling API functions directly (::p_api is not used). */
extern dmac_instance_ctrl_t g_transfer0_ctrl;
extern const transfer_cfg_t g_transfer0_cfg;

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
#if (RA_NOT_DEFINED == g_transfer0)
    #define oled_spi_P_TRANSFER_TX (NULL)
#else
    #define oled_spi_P_TRANSFER_TX (&g_transfer0)
#endif
#if (RA_NOT_DEFINED == RA_NOT_DEFINED)
    #define oled_spi_P_TRANSFER_RX (NULL)
#else
    #define oled_spi_P_TRANSFER_RX (&RA_NOT_DEFINED)
#endif
#undef RA_NOT_DEFINED
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
