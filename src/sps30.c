#include <msp430.h>
#include "uart.h"
#include "sps30.h"

unsigned char write[] = {0x7E, 0x00, 0x00, 0x02, 0x01, 0x03, 0xF9, 0x7E};
unsigned char read[7];

int main(void) {

    WDTCTL = WDTPW | WDTHOLD;

    uart_init(SPS30);
    uart_write(write, 8);
    uart_read(read, 7);
    while(1);
}

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

//COMMANDS

//to be used as the first command
int start_measurement() {
    int array[] = {START_BYTE, ADDRESS, START, 0x02, 0x01, 0x03};
    int packet[] = complete_packet(array, 6)
    int length = 8;
    uart_write(parse_packet(packet, length), length);
    return 0;
}

