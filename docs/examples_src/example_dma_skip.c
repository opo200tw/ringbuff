/* Buffer */
ringbuff_t buff;
uint8_t buff_data[8];

/* Working data length */
size_t len;

/* Send data function */
void send_data(void);

int
main(void) {
    /* Initialize buffer */
    ringbuff_init(&buff, buff_data, sizeof(buff_data));

    /* Write 4 bytes of data */
    ringbuff_write(&buff, "0123", 4);

    /* Send data over DMA */
    send_data();

    while (1);
}

/* Send data over DMA */
void
send_data(void) {
    /* If len > 0, DMA transfer is on-going */
    if (len) {
        return;
    }

    /* Get maximal length of buffer to read data as linear memory */
    len = ringbuff_get_linear_block_read_length(&buff);
    if (len) {
        /* Get pointer to read memory */
        uint8_t* data = ringbuff_get_linear_block_read_address(&buff);

        /* Start DMA transfer */
        start_dma_transfer(data, len);
    }

    /* Function does not wait for transfer to finish */
}

/* Interrupt handler */
/* Called on DMA transfer finish */
void
DMA_Interrupt_handler(void) {
    /* Transfer finished */
    if (len) {
        /* Now skip the data (move read pointer) as they were successfully transferred over DMA */
        ringbuff_skip(&buff, len);

        /* Reset length = DMA is not active */
        len = 0;

        /* Try to send more */
        send_data();
    }
}