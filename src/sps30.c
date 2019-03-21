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

int checksum(int*, int);
int* complete_packet(int*, int);
unsigned char* parse_packet(int*, int);
int array_copy(unsigned char*, unsigned char*, int);

int overclock();

unsigned char write[8];
unsigned char read[50];

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;

    uart_init(SPS30);

    start_measurement();
    read_start_ack();

    ask_measured_values();
    read_measured_values();

    stop_measurement();
    read_stop_ack();

    while(1);
}
//COMMANDS


int start_measurement() {
    unsigned char array[] = {0x7E, 0x00, 0x00, 0x02, 0x01, 0x03, 0xF9, 0x7E};
    uart_write(array, 8);
    array_copy(write, array, 8);
    return 0;
}

int read_start_ack() {
    uart_read(read, 7);
    return 0;
}

int stop_measurement() {
    unsigned char array[] = {0x7E, 0x00, 0x01, 0x00, 0xFE, 0x7E};
    uart_write(array, 6);
    array_copy(write, array, 6);
    return 0;
}

int read_stop_ack() {
    return read_start_ack();
}

int ask_measured_values() {
    unsigned char array[] = {0x7E, 0x00, 0x03, 0x00, 0xFC, 0x7E};
    uart_write(array, 6);
    array_copy(write, array, 6);
    return 0;
}

int read_measured_values() {
    uart_read(read, 47);
    return 0;
}

//TODO: check if length is right
int ask_cleaning_interval() {
    unsigned char array[] = {0x7E, 0x00, 0x80, 0x01, 0x00, 0x7D, 0x5E, 0x7E};
    uart_write(array, 8);
    array_copy(write, array, 8);
    return 0;
}

int read_cleaning_interval() {
    unsigned char array[11];
    uart_read(array, 11);
    return 0;
}


//UTILS

//insert array with start but without stop nor checksum
int checksum(int* bytes, int length) {
    int checksum = 0;
    int i;

    for(i = 1; i < length; i++)
        checksum += bytes[i];

    return checksum ^= 0xFF;
}

//return a packet with length+2 with checksum and stop byte
int* complete_packet(int* bytes, int length) {
    int packet[length + 2];
    unsigned int i;

    for(i = 0; i < length; i++)
        packet[i] = bytes[i];

    packet[++i] = checksum(bytes, length);
    packet[++i] = STOP_BYTE;

    return packet;
}

//parse packet from int to char array
unsigned char* parse_packet(int* bytes, int length) {
    unsigned char* packet;
    int i;

    for(i = 0; i < length; i++)
        packet[i] = bytes[i] + '0';

    return packet;
}

int array_copy(unsigned char* dst, unsigned char* src, int length) {
    unsigned int i;
    for(i = 0; i < length; i++)
        dst[i] = src[i];
    return 0;
}
