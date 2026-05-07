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
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_SPI0_RXI,GROUP0), /* SPI0 RXI (Receive buffer full) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_SPI0_TEI,GROUP1), /* SPI0 TEI (Transmission complete event) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_SPI0_ERI,GROUP2), /* SPI0 ERI (Error) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_DMAC1_INT,GROUP3), /* DMAC1 INT (DMAC1 transfer end) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_GPT0_COUNTER_OVERFLOW,GROUP4), /* GPT0 COUNTER OVERFLOW (Overflow) */
        };
        #endif
        #endif