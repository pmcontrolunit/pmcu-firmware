#include <msp430.h>
#include "uart.h"

/*
int main () {
    unsigned char command[64];
    unsigned int i;
    unsigned char byte;

    __bis_SR_register(GIE);

    WDTCTL = WDTPW | WDTHOLD;

    // sets up pins serial and usb serial
    uart_setup(uart_a0, uart_baud_rate_115200);
    uart_setup(uart_a1, uart_baud_rate_115200);

    while (1) {
        uart_write_str(uart_a1, "Synchronizing with DCE, please wait...\n");
        uart_write_str(uart_a0, "AT\r");

        uart_read(uart_a0, &byte, 1);
        if (byte == 'O' || byte == 'o') {
            uart_read(uart_a0, &byte, 1);
            if (byte == 'K' || byte == 'k') {
                break;
            }
        }

        uart_write_str(uart_a1, "Something received but not expected:\n");
        uart_write(uart_a1, byte, 1);
        uart_write_str(uart_a1, "\n");
    }
    uart_write_str(uart_a1, "Synchronization done!");

    while (1) {
          uart_write_str(uart_a1, "Insert a command:\n");

          // reads next incoming command - a1
          i = 0;
          while (1) {
              uart_read(uart_a1, &command[i], 1);
              if (command[i] == '\r' || command[i] == '\n') {
                  break;
              }
              i++;
          }

          // Prepares the command format and sends it to the modem.
          // Commands terminates with a <CR> (\r).
          command[i++] = '\r';
          command[i] = '\0'; // \0 is used to detect the string length
          uart_write_str(uart_a0, command);

          uart_write_str(uart_a1, "Command wrote out of A0, waiting for a response...\n");

          // waits for a response from modem - a0
          i = 0;
          while (1) {
              uart_read(uart_a0, &command[i], 1);
              if (command[i] == '\r') {
                  i++;

                  uart_read(uart_a0, &command[i], 1);
                  if (command[i] == '\n') {
                      break;
                  }
              }
              i++;
          }
          command[i - 1] = '\n';
          command[i] = '\0';

          uart_write_str(uart_a1, "Response:\n");
          uart_write_str(uart_a1, command); // writes response back to uart
    }
}
*/
