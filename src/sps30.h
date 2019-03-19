/*
 * sps30.h
 *
 *  Created on: 18 mar 2019
 *      Author: Alessandro
 */

#define START 0x00
#define STOP 0x01
#define READ 0x03
#define CLEAN 0x56
#define INFO 0xD0
#define RESET 0xD3

#define ADDRESS 0x00

#define START_BYTE 0x7E
#define STOP_BYTE 0x7E

int checksum(int*, int);
int* complete_packet(int*, int);
unsigned char* parse_packet(int*, int);
