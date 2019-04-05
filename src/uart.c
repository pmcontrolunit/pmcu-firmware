#include "uart.h"

#include "circular_buffer.h"
#include "string.h"

circular_buffer read_buffer_a0;
circular_buffer read_buffer_a1;

uart_error latest_error = UART_OK;

// UART_BAUD_RATE_9600_ACLK_32KHZ
void uart_set_baud_rate_9600_aclk_32khz(uart_module module) {
    UART_REGISTER(module, UART_CTL1) |= UCSSEL_1;
    UART_REGISTER(module, UART_BR0) = 3;
    UART_REGISTER(module, UART_BR1) = 0;
    UART_REGISTER(module, UART_MCTL) = UCBRS_3 + UCBRF_0;
}

// UART_BAUD_RATE_9600_SMCLK_1MHZ
void uart_set_baud_rate_9600_smclk_1mhz(uart_module module) {
    UART_REGISTER(module, UART_CTL1) |= UCSSEL_2;
    UART_REGISTER(module, UART_BR0) = 104;
    UART_REGISTER(module, UART_BR1) = 0;
    UART_REGISTER(module, UART_MCTL) = UCBRS_1 + UCBRF_0;
}

// UART_BAUD_RATE_9600_SMCLK_12MHZ
void uart_set_baud_rate_9600_smclk_12mhz(uart_module module) {
    UART_REGISTER(module, UART_CTL1) |= UCSSEL_2;
    UART_REGISTER(module, UART_BR0) = 226;
    UART_REGISTER(module, UART_BR1) = 4;
    UART_REGISTER(module, UART_MCTL) = UCBRS_0 + UCBRF_0;
}

// UART_BAUD_RATE_115200_SMCLK_1MHZ
void uart_set_baud_rate_115200_1mhz(uart_module module) {
    UART_REGISTER(module, UART_CTL1) |= UCSSEL_2;
    UART_REGISTER(module, UART_BR0) = 8;
    UART_REGISTER(module, UART_BR1) = 0;
    UART_REGISTER(module, UART_MCTL) = UCBRS_6 + UCBRF_0;
}

// UART_BAUD_RATE_115200_SMCLK_12MHZ
void uart_set_baud_rate_115200_12mhz(uart_module module) {
    UART_REGISTER(module, UART_CTL1) |= UCSSEL_2;
    UART_REGISTER(module, UART_BR0) = 104;
    UART_REGISTER(module, UART_BR1) = 0;
    UART_REGISTER(module, UART_MCTL) = UCBRS_1 + UCBRF_0;
}

void uart_setup(uart_module module, uart_settings settings) {
    // Halts state-machine operation
    UART_REGISTER(module, UART_CTL1) |= UCSWRST;

    // Applies settings
    UART_REGISTER(module, UART_CTL0) = settings & 0xF8;

    // Sets baud-rate
    switch (settings & 0x07) {
    default:
    case UART_BAUD_RATE_9600_ACLK_32KHZ:
        uart_set_baud_rate_9600_aclk_32khz(module);
        break;

    case UART_BAUD_RATE_9600_SMCLK_1MHZ:
        uart_set_baud_rate_9600_smclk_1mhz(module);
        break;

    case UART_BAUD_RATE_9600_SMCLK_12MHZ:
        uart_set_baud_rate_9600_smclk_12mhz(module);
        break;

    case UART_BAUD_RATE_115200_SMCLK_1MHZ:
        uart_set_baud_rate_115200_smclk_1mhz(module);
        break;

    case UART_BAUD_RATE_115200_SMCLK_12MHZ:
        uart_set_baud_rate_115200_smclk_12mhz(module);
        break;
    }

    // Enables communication pins
    switch (module) {
    default:
    case UART_A0:
        P3SEL |= BIT3 + BIT4;
        break;
    case UART_A1:
        P4SEL |= BIT4 + BIT5;
        break;
    }

    // Starts state-machine
    UART_REGISTER(module, UART_CTL1) &= ~UCSWRST;

    // Enables receiving interrupts
    UART_REGISTER(module, UART_IE) |= UCRXIE;
}

uart_error uart_get_latest_error() {
    return latest_error;
}

inline int uart_write(uart_module module, uint8_t byte) {
    return uart_write_buffer(module, &byte, 1);
}

int uart_write_buffer(uart_module module, uint8_t *buffer, unsigned int buffer_length) {
    // Sets DMA0 trigger on A0/A1 UART_TXBUF ready
    DMACTL0 = module == uart_a0 ? DMA0TSEL_17 : DMA0TSEL_21;

    // From buffer to UART_TXBUF
    __data16_write_addr((unsigned short) &DMA0SA,(unsigned long) buffer);
    __data16_write_addr((unsigned short) &DMA0DA,(unsigned long) &UART_REGISTER(module, UART_TXBUF));

    // How much data to transfer
    DMA0SZ = buffer_length;

    // Src address is incremented, dst address not, byte to byte and trigger on level
    DMA0CTL = DMASRCINCR_3 + DMADSTINCR_0 + DMASBDB + DMALEVEL + DMADT_0;

    // Starts the timeout timer to run every second
    TA0CTL = TASSEL_1 + MC_1 + ID_3;
    TA0CCR0 = 4000 * UART_WRITE_TIMEOUT;
    TA0CCTL0 |= CCIE;

    // Halts the CPU to wait for completation
    __bis_SR_register(LPM0_bits);
    __no_operation();

    // Stops the timer
    TA0CTL = MC0;

    if (DMA0SZ == 0) {
        // All data have been transferred
        latest_error = UART_OK;
    } else {
        // Transfer went timeout
        latest_error = UART_TIMEOUT;
    }

    return buffer_length - DMA0SZ;
}

inline int uart_write_string(uart_module module, const uint8_t *string) {
    return uart_write_buffer(module, string, strlen(string));
}

int uart_read(uart_module module, uint8_t *byte) {
    // Selects the right buffer based on the module A0/A1
    circular_buffer *read_buffer = module == uart_a0 ? &read_buffer_a0 : &read_buffer_a1;

    // If it's currently empty
    if (circular_buffer_is_empty(read_buffer)) {
        // Starts the timeout timer to run every second
        TA0CTL = TASSEL_1 + MC_1 + ID_3;
        TA0CCR0 = 4000 * UART_READ_TIMEOUT;
        TA0CCTL0 |= CCIE;

        // Halts the CPU to wait for a new byte
        __bis_SR_register(LPM0_bits);
        __no_operation();
    }

    // Clears the timer
    TA0CTL = MC0;

    // Buffer is still empty, it went timeout
    if (circular_buffer_is_empty(&read_buffer)) {
        latest_error = UART_TIMEOUT;
        return 0;
    } else {
        // Reads a new byte
        circular_buffer_read(&read_buffer, byte);
        latest_error = UART_OK;
        return 1;
    }
}

int uart_read_buffer(uart_module module, uint8_t *buffer, unsigned int buffer_length) {
    for (unsigned int i = 0; i < buffer_length; i++) {
        uart_read(module, &buffer[i]);
        if (latest_error != UART_OK) {
            break;
        }
    }
    return i;
}

int uart_read_line(uart_module module, uint8_t *buffer, unsigned int buffer_length) {
    for (unsigned int i = 0; i < buffer_length;) {
        uart_read(module, &buffer[i]);
        if (latest_error != UART_OK) {
            break;
        }

        switch (buffer[i]) {
        // If it's a \n replaces it with a \0 and returns
        case '\n':
            buffer[i] = '\0';
            break;
        // If it's a \r skips without saving it
        case '\r':
            continue;
        // For any character increments
        default:
            i++;
        }
    }
    // Returns the number of the bytes read
    return i + 1;
}

int uart_read_until(uart_module module, const uint8_t *sample, unsigned int sample_length, uint8_t *buffer, unsigned int buffer_length) {
    unsigned int sample_i = 0;
    unsigned int buffer_i = 0;
    uint8_t fallback = 0;

    // If buffer isn't defined points to 1 byte
    if (!buffer) {
       buffer = &fallback;
       buffer_length = 1;
    }

    while (1) {
       uart_read(module, &buffer[buffer_i]);
       if (latest_error != UART_OK) {
           break;
       }

       if (buffer[buffer_i] == sample[sample_i]) {
           sample_i++;
       } else if (sample_i > 0) {
           sample_i = 0;
       }

       if (sample_i == sample_length) {
           break;
       }

       // Goes back to 0 if reaches the limit
       buffer_i = ++buffer_i >= buffer_length ? buffer_i : 0;
    }
    return buffer_i + 1;
}

inline int uart_read_until_string(uart_module module, const char *sample_string, uint8_t *buffer, unsigned int buffer_length) {
    return uart_read_until(module, sample_string, strlen(sample_string), buffer, buffer_length);
}

#pragma vector=DMA_VECTOR
__interrupt void on_uart_tx_complete() {
    __bic_SR_register_on_exit(LPM0_bits);
    __no_operation();
}

#pragma vector=USCI_A0_VECTOR
__interrupt void on_uart_a0_rx() {
    if (UCA0IFG & UCRXIFG) {
        // If buffer is empty, a read could be waiting, so we remove LPM0
        if (circular_buffer_is_empty(&read_buffer_a0)) {
            __bic_SR_register_on_exit(LPM0_bits);
        }
        circular_buffer_write(&read_buffer_a0, UCA0RXBUF);
    }
}

#pragma vector=USCI_A1_VECTOR
__interrupt void on_uart_a1_rx() {
    if (UCA1IFG & UCRXIFG) {
        circular_buffer_write(&read_buffer_a1, UCA1RXBUF);
    }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void on_rx_timeout() {
    __bic_SR_register_on_exit(LPM0_bits);
}

