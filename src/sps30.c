#include <msp430.h>
#include "sps30.h"

//MSP to SPS30 packet structure
//START + ADDRESS + CMD + LENGTH + ...bytes... + CHECKSUM + STOP

//SPS30 to MSP packet structure
//START + ADDRESS + CMD + STATE + LENGTH + ...bytes... + CHECKSUM + STOP

//NOTE: for sure the second has got at least 1 more byte than the first

//ARRAY LENGTHS
#define MEASURED_DATA_LENGTH 47
#define START_ACK_LENGTH 7
#define STOP_ACK_LENGTH 7

//COMMAND CODES
#define START 0x00
#define STOP 0x01
#define READ 0x03
#define INFO_CLEAN 0x80
#define CLEAN 0x56
#define INFO 0xD0
#define RESET 0xD3

//STATE CODES
#define NO_ERROR 0x00
#define WRONG_DATA_LENGTH 0x01
#define UNKNOWN_CMD 0x02
#define FORBIDDEN_CMD 0x03
#define ILLEGAL_PARAMETER 0x04
#define ARG_OUT_OF_RANGE 0x28
#define CMD_NOT_IN_STATE 0x43

//UART SPS30 ADDRESS
#define ADDRESS 0x00

//START AND STOP BYTES
#define START_BYTE 0x7E
#define STOP_BYTE 0x7E

int array_copy(unsigned char*, unsigned char*, int);

unsigned char write[8];
unsigned char read[47];

unsigned char v_mass_PM1[4];
unsigned char v_mass_PM2dot5[4];
unsigned char v_mass_PM4[4];
unsigned char v_mass_PM10[4];
unsigned char v_num_PM0dot5[4];
unsigned char v_num_PM1[4];
unsigned char v_num_PM2dot5[4];
unsigned char v_num_PM4[4];
unsigned char v_num_PM10[4];
unsigned char v_typical_size[4];

void init_12mhz_mclk() {
    UCSCTL3 |= SELREF_2;

    __bis_SR_register(SCG0);
    UCSCTL0 = 0x0000;
    UCSCTL1 = DCORSEL_5;
    UCSCTL2 = FLLD_1 + 374;
    __bic_SR_register(SCG0);

    __delay_cycles(375000);
}


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;

    __bis_SR_register(GIE);

    init_12mhz_mclk();

    uart_setup(uart_a0, uart_baud_rate_115200_12mhz);

    sps30_start_measurement();
    sps30_read_start_ack(read);

    sps30_ask_measured_values();
    sps30_read_measured_values(read);

    sps30_get_measure(v_mass_PM1, read, mass_PM1);
    sps30_get_measure(v_mass_PM2dot5, read, mass_PM2dot5);
    sps30_get_measure(v_mass_PM4, read, mass_PM4);
    sps30_get_measure(v_mass_PM10, read, mass_PM10);
    sps30_get_measure(v_num_PM0dot5, read, num_PM0dot5);
    sps30_get_measure(v_num_PM1, read, num_PM1);
    sps30_get_measure(v_num_PM2dot5, read, num_PM2dot5);
    sps30_get_measure(v_num_PM4, read, num_PM4);
    sps30_get_measure(v_num_PM10, read, num_PM10);
    sps30_get_measure(v_typical_size, read, typical_size);

    sps30_start_fan_cleaning();
    sps30_read_fan_ack(read);

    sps30_stop_measurement();
    sps30_read_stop_ack(read);

    while(1);
}
//COMMANDS

int sps30_start_measurement() {
    unsigned char array[] = {0x7E, 0x00, 0x00, 0x02, 0x01, 0x03, 0xF9, 0x7E};
    uart_write(uart_a0, array, 8, 0);
    array_copy(write, array, 8);
    return 0;
}

int sps30_read_start_ack(unsigned char* buff) {
    uart_read(uart_a0, read, 7);
    return 0;
}

int sps30_stop_measurement() {
    unsigned char array[] = {0x7E, 0x00, 0x01, 0x00, 0xFE, 0x7E};
    uart_write(uart_a0, array, 6, 0);
    array_copy(write, array, 6);
    return 0;
}

int sps30_read_stop_ack(unsigned char* buff) {
    return sps30_read_start_ack(buff);
}


int sps30_ask_measured_values() {
    unsigned char array[] = {0x7E, 0x00, 0x03, 0x00, 0xFC, 0x7E};
    uart_write(uart_a0, array, 6, 0);
    array_copy(write, array, 6);
    return 0;
}

int sps30_read_measured_values(unsigned char* buff) {
    uart_read(uart_a0, buff, 47);
    return 0;
}

//TODO: check if length is right
int sps30_ask_cleaning_interval() {
    unsigned char array[] = {0x7E, 0x00, 0x80, 0x01, 0x00, 0x7D, 0x5E, 0x7E};
    uart_write(uart_a0, array, 8, 0);
    array_copy(write, array, 8);
    return 0;
}

int sps30_read_cleaning_interval() {
    unsigned char array[11];
    uart_read(uart_a0, array, 11);
    return 0;
}

int sps30_start_fan_cleaning() {
    unsigned char array[] = {0x7E, 0x00, 0x56, 0x00, 0xA9, 0x7E};
    uart_write(uart_a0, array, 6, 0);
    array_copy(write, array, 6);
    return 0;
}

int sps30_read_fan_ack(unsigned char* buff) {
    uart_read(uart_a0, buff, 7);
    return 0;
}

//UTILS

int array_copy(unsigned char* dst, unsigned char* src, int length) {
    unsigned int i;
    for(i = 0; i < length; i++)
        dst[i] = src[i];
    return 0;
}

int sps30_get_measure(unsigned char buff[], unsigned char measures[], Measure measure) {
    unsigned int i, j;

    j = 5 + 4*measure;

    for(i = 0; i < 4; i++, j++)
        buff[i] = measures[j];

    return 0;
}
