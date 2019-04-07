#include "modem.h"

#include "uart.h"

#include <string.h>

void sim800l_write_command(const uint8_t *command) {
    uart_write_string(UART_A0, command);
    uart_write_string(UART_A0, "\r");
}

inline void sim800l_read_response(uint8_t *buffer, unsigned int buffer_length) {
    uart_read_line(UART_A0, NULL, NULL, 0);
    uart_read_line(UART_A0, buffer, buffer_length, 0);
}

inline void sim800l_read_echoed_response(uint8_t *buffer, unsigned int buffer_length) {
    uart_read_input(UART_A0, NULL, NULL);
    sim800l_read_response(buffer, buffer_length);
}

int modem_init() {
    uint8_t response[256];

    uart_setup(UART_A0, UART_BAUD_RATE_9600_SMCLK_1MHZ);

    // Resets to default configuration
    sim800l_write_command("ATZ");
    uart_read_until_string(UART_A0, "OK\r\n", NULL, NULL, 0);

    // No echoes
    sim800l_write_command("ATE0");
    sim800l_read_echoed_response(response, 256);
    if (strcmp((const char *) response, "OK") != 0) {
        return 1;
    }

    // Verbose mode for errors
    sim800l_write_command("AT+CMEE=2");
    sim800l_read_response(response, 256);
    if (strcmp((const char *) response, "OK") != 0) {
        return 2;
    }

    // Deactivates GPRS PDP context
    sim800l_write_command("AT+CIPSHUT");
    sim800l_read_response(response, 256);
    if (strcmp((const char *) response, "SHUT OK") != 0) {
        return 3;
    }

    // Single IP connection
    sim800l_write_command("AT+CIPMUX=0");
    sim800l_read_response(response, 256);
    if (strcmp((const char *) response, "OK") != 0) {
        return 4;
    }

    // Attaches GPRS service
    sim800l_write_command("AT+CGATT=1");
    sim800l_read_response(response, 256);
    if (strcmp((const char *) response, "OK") != 0) {
        return 5;
    }

    // Starts task and set APN, username and password
    sim800l_write_command("AT+CSTT=\"TM\",\"\",\"\"");
    sim800l_read_response(response, 256);
    if (strcmp((const char *) response, "OK") != 0) {
        return 6;
    }

    // Brings up wireless connection with GPRS
    sim800l_write_command("AT+CIICR");
    sim800l_read_response(response, 256);
    if (strcmp((const char *) response, "OK") != 0) {
        return 7;
    }

    // Gets the local IP address (needed to connect)
    sim800l_write_command("AT+CIFSR");
    sim800l_read_response(response, 256);
    if (strcmp((const char *) response, "OK") != 0) {
        return 7;
    }

    // Removes send data prompt
    sim800l_write_command("AT+CIPSPRT=2");
    sim800l_read_response(response,256);
    if (strcmp((const char *) response, "OK") != 0) {
        return 8;
    }

    return 0;
}

int modem_connect(const char *host, const char *port) {
    uint8_t buffer[256];
    strcpy((const char *) buffer, "AT+CIPSTART=\"TCP\",\"");
    strcat((const char *) buffer, host);
    strcat((const char *) buffer, "\",");
    strcat((const char *) buffer, port);

    sim800l_write_command((const uint8_t *) buffer);
    sim800l_read_response(buffer, 256);
    if (strcmp((const char *) buffer, "OK") != 0) {
        return 1;
    }
    sim800l_read_response(buffer, 256);
    if (strcmp((const char *) buffer, "CONNECT OK") != 0) {
        return 2;
    }
    return 0;
}

int modem_sendall(void *fd, const void *buffer, unsigned int buffer_length, int flags) {
    static const uint8_t exit_sequence[2] = {0x1A, 0x1A};
    uint8_t response[256];

    sim800l_write_command("AT+CIPSEND");
    uart_write_buffer(UART_A0, buffer, buffer_length);
    uart_write_buffer(UART_A0, exit_sequence, 2);

    uart_read_buffer(UART_A0, response, 256, 0);

    return 0;
}

int modem_recvall(void *fd, void *buffer, unsigned int buffer_length, int flags) {
}
