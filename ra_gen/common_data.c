/* generated common source file - do not edit */
#include "common_data.h"
ioport_instance_ctrl_t g_ioport_ctrl;
const ioport_instance_t g_ioport =
        {
            .p_api = &g_ioport_on_ioport,
            .p_ctrl = &g_ioport_ctrl,
            .p_cfg = &g_bsp_pin_cfg,
        };
TX_SEMAPHORE g_oled_spi_semaphore;
                void tx_startup_err_callback(void *p_instance, void *p_data);
TX_SEMAPHORE g_touchpad_iic_semaphore;
                void tx_startup_err_callback(void *p_instance, void *p_data);
void g_common_init(void) {
UINT err_g_oled_spi_semaphore;
                err_g_oled_spi_semaphore = tx_semaphore_create(&g_oled_spi_semaphore, (CHAR *) "OLED SPI Semaphore", 1);
                if (TX_SUCCESS != err_g_oled_spi_semaphore) {
                    tx_startup_err_callback(&g_oled_spi_semaphore, 0);
                }
UINT err_g_touchpad_iic_semaphore;
                err_g_touchpad_iic_semaphore = tx_semaphore_create(&g_touchpad_iic_semaphore, (CHAR *) "Touchpad IIC Semaphore", 1);
                if (TX_SUCCESS != err_g_touchpad_iic_semaphore) {
                    tx_startup_err_callback(&g_touchpad_iic_semaphore, 0);
                }
}
