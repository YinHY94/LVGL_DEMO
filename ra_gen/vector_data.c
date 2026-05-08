/* generated vector source file - do not edit */
        #include "bsp_api.h"
        /* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
        #if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = spi_rxi_isr, /* SPI0 RXI (Receive buffer full) */
            [1] = spi_tei_isr, /* SPI0 TEI (Transmission complete event) */
            [2] = spi_eri_isr, /* SPI0 ERI (Error) */
            [3] = dmac_int_isr, /* DMAC1 INT (DMAC1 transfer end) */
            [4] = gpt_counter_overflow_isr, /* GPT0 COUNTER OVERFLOW (Overflow) */
            [5] = iic_master_rxi_isr, /* IIC0 RXI (Receive data full) */
            [6] = iic_master_txi_isr, /* IIC0 TXI (Transmit data empty) */
            [7] = iic_master_tei_isr, /* IIC0 TEI (Transmit end) */
            [8] = iic_master_eri_isr, /* IIC0 ERI (Transfer error) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_SPI0_RXI,GROUP0), /* SPI0 RXI (Receive buffer full) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_SPI0_TEI,GROUP1), /* SPI0 TEI (Transmission complete event) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_SPI0_ERI,GROUP2), /* SPI0 ERI (Error) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_DMAC1_INT,GROUP3), /* DMAC1 INT (DMAC1 transfer end) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_GPT0_COUNTER_OVERFLOW,GROUP4), /* GPT0 COUNTER OVERFLOW (Overflow) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_IIC0_RXI,GROUP5), /* IIC0 RXI (Receive data full) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_IIC0_TXI,GROUP6), /* IIC0 TXI (Transmit data empty) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_IIC0_TEI,GROUP7), /* IIC0 TEI (Transmit end) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_IIC0_ERI,GROUP0), /* IIC0 ERI (Transfer error) */
        };
        #endif
        #endif