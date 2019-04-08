#include "modem.h"

#include "uart.h"

#include <string.h>

#define SIM800L_LOG_MAX_SIZE 256

uint8_t sim800l_latest_log[SIM800L_LOG_MAX_SIZE];

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

int sim800l_init() {
    sim800l_write_command("ATZ");
    uart_read_until_string(UART_A0, "OK\r\n", NULL, NULL, 0);

    sim800l_write_command("ATE0");
    sim800l_read_echoed_response(sim800l_latest_log, SIM800L_LOG_MAX_SIZE);
    if (strcmp((const char *) sim800l_latest_log, "OK") != 0) {
        return MODEM_ERROR;
    }

    sim800l_write_command("AT+CMEE=2");
    sim800l_read_response(sim800l_latest_log, SIM800L_LOG_MAX_SIZE);
    if (strcmp((const char *) sim800l_latest_log, "OK") != 0) {
        return MODEM_ERROR;
    }

    sim800l_write_command("AT+CIPSPRT=1");
    sim800l_read_response(sim800l_latest_log, SIM800L_LOG_MAX_SIZE);
    if (strcmp((const char *) sim800l_latest_log, "OK") != 0) {
        return MODEM_ERROR;
    }

    return MODEM_OK;
}

int sim800l_gprs_shut() {
    sim800l_write_command("AT+CIPSHUT");
    sim800l_read_response(sim800l_latest_log, SIM800L_LOG_MAX_SIZE);
    if (strcmp((const char *) sim800l_latest_log, "SHUT OK") != 0) {
        return MODEM_ERROR;
    }
    return MODEM_OK;
}

int sim800l_gprs_init() {
    if (sim800l_gprs_shut()) {
        return MODEM_ERROR;
    }

    sim800l_write_command("AT+CIPMUX=0");
    sim800l_read_response(sim800l_latest_log, SIM800L_LOG_MAX_SIZE);
    if (strcmp((const char *) sim800l_latest_log, "OK") != 0) {
        return MODEM_ERROR;
    }

    sim800l_write_command("AT+CGATT=1");
    sim800l_read_response(sim800l_latest_log, SIM800L_LOG_MAX_SIZE);
    if (strcmp((const char *) sim800l_latest_log, "OK") != 0) {
        return MODEM_ERROR;
    }

    sim800l_write_command("AT+CSTT=\"TM\",\"\",\"\"");
    sim800l_read_response(sim800l_latest_log, SIM800L_LOG_MAX_SIZE);
    if (strcmp((const char *) sim800l_latest_log, "OK") != 0) {
        return MODEM_ERROR;
    }

    sim800l_write_command("AT+CIICR");
    sim800l_read_response(sim800l_latest_log, SIM800L_LOG_MAX_SIZE);
    if (strcmp((const char *) sim800l_latest_log, "OK") != 0) {
        return MODEM_ERROR;
    }

    sim800l_write_command("AT+CIFSR");
    sim800l_read_response(sim800l_latest_log, SIM800L_LOG_MAX_SIZE);

    return MODEM_OK;
}

const char *modem_get_latest_log() {
    return (const char *) sim800l_latest_log;
}

int modem_init() {
    uart_setup(UART_A0, UART_BAUD_RATE_9600_SMCLK_1MHZ);

    if (sim800l_init() == MODEM_ERROR) {
        return MODEM_ERROR;
    }
    if (sim800l_gprs_init() == MODEM_ERROR) {
        return MODEM_ERROR;
    }
    return MODEM_OK;
}

int modem_connect(const char *host, const char *port) {
    strcpy((const char *) sim800l_latest_log, "AT+CIPSTART=\"TCP\",\"");
    strcat((const char *) sim800l_latest_log, host);
    strcat((const char *) sim800l_latest_log, "\",\"");
    strcat((const char *) sim800l_latest_log, port);
    strcat((const char *) sim800l_latest_log, "\"");

    sim800l_write_command((const uint8_t *) sim800l_latest_log);
    sim800l_read_response(sim800l_latest_log, SIM800L_LOG_MAX_SIZE);
    if (strcmp((const char *) sim800l_latest_log, "OK") != 0) {
        return MODEM_ERROR;
    }
    sim800l_read_response(sim800l_latest_log, SIM800L_LOG_MAX_SIZE);
    if (strcmp((const char *) sim800l_latest_log, "CONNECT OK") != 0) {
        return MODEM_ERROR;
    }

    return MODEM_OK;
}

int modem_send(void *fd, const void *buffer, unsigned int buffer_length, int flags) {
    sim800l_write_command("AT+CIPSEND");
    uart_read_until_string(UART_A0, "> ", NULL, NULL, 0);

    uart_write_buffer(UART_A0, buffer, buffer_length);
    uart_write(UART_A0, 0x1a);

    sim800l_read_response(sim800l_latest_log, SIM800L_LOG_MAX_SIZE);
    if (strcmp((const char *) sim800l_latest_log, "SEND OK") != 0) {
        return MODEM_ERROR;
    }

    return MODEM_OK;
}

int modem_disconnect() {
    sim800l_write_command("AT+CIPSHUT");
    sim800l_read_response(sim800l_latest_log, SIM800L_LOG_MAX_SIZE);
    if (strcmp((const char *) sim800l_latest_log, "SHUT OK") != 0) {
        return MODEM_ERROR;
    }
    return 0;
}
