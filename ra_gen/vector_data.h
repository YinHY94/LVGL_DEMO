/* generated vector header file - do not edit */
        #ifndef VECTOR_DATA_H
        #define VECTOR_DATA_H
        #ifdef __cplusplus
        extern "C" {
        #endif
                /* Number of interrupts allocated */
        #ifndef VECTOR_DATA_IRQ_COUNT
        #define VECTOR_DATA_IRQ_COUNT    (4)
        #endif
        /* ISR prototypes */
        void spi_rxi_isr(void);
        void spi_tei_isr(void);
        void spi_eri_isr(void);
        void dmac_int_isr(void);

        /* Vector table allocations */
        #define VECTOR_NUMBER_SPI0_RXI ((IRQn_Type) 0) /* SPI0 RXI (Receive buffer full) */
        #define SPI0_RXI_IRQn          ((IRQn_Type) 0) /* SPI0 RXI (Receive buffer full) */
        #define VECTOR_NUMBER_SPI0_TEI ((IRQn_Type) 1) /* SPI0 TEI (Transmission complete event) */
        #define SPI0_TEI_IRQn          ((IRQn_Type) 1) /* SPI0 TEI (Transmission complete event) */
        #define VECTOR_NUMBER_SPI0_ERI ((IRQn_Type) 2) /* SPI0 ERI (Error) */
        #define SPI0_ERI_IRQn          ((IRQn_Type) 2) /* SPI0 ERI (Error) */
        #define VECTOR_NUMBER_DMAC0_INT ((IRQn_Type) 3) /* DMAC0 INT (DMAC0 transfer end) */
        #define DMAC0_INT_IRQn          ((IRQn_Type) 3) /* DMAC0 INT (DMAC0 transfer end) */
        /* The number of entries required for the ICU vector table. */
        #define BSP_ICU_VECTOR_NUM_ENTRIES (4)

        #ifdef __cplusplus
        }
        #endif
        #endif /* VECTOR_DATA_H */