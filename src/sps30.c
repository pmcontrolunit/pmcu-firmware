#include <msp430.h>
#include "uart.h"
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


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;

    uart_init(SPS30);
    while(1);
}

//COMMANDS

int start_measurement() {
    int array[] = {START_BYTE, ADDRESS, START, 0x02, 0x01, 0x03};
    int packet[] = complete_packet(array, 6)
    int length = 8;
    uart_write(parse_packet(packet, length), length);
    return 0;
}

unsigned char* read_start_ack() {
    unsigned char array[7];
    uart_read(array, 7);
    return array;
}

int stop_measurement() {
    int array[] = {START_BYTE, ADDRESS, STOP, 0x00};
    int packet[] = complete_packet(array, 4);
    int length = 6;
    uart_write(parse_packet(packet, length), length);
    return 0;
}

unsigned char* read_stop_ack() {
    return read_start_ack();
}

int ask_measured_values() {
    int array[] = {START_BYTE, ADDRESS, READ, 0x00};
    int packet[] = complete_packet(array, 4);
    int length = 6;
    uart_write(parse_packet(packet, length), length);
    return 0;
}

unsigned char* read_measured_values() {
    unsigned char array[47];
    uart_read(array, 47);
    return array;
}

//TODO: check if length is right
int ask_cleaning_interval() {
    int array[] = {START_BYTE, ADDRESS, INFO_CLEAN, 0x01, 0x00, 0x7D};
    int packet[] = complete_packet(array, 6);
    int length = 8;
    uart_write(parse_packet(packet, length), length);
    return 0;
}

unsigned char* read_cleaning_interval() {
    unsigned char array[11];
    uart_read(array, 11);
    return array;
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
    int i;

    for(i = 0; i < length; i++)
        packet[i] = bytes[i];

    packet[++i] = checksum(bytes);
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


