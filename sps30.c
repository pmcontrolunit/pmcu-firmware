#include <msp430.h> 

/*
 P4.2: Serial Clock Pin (SCL)
 P4.1: Serial Data Pin (SDA)

 They both need pullup resistors
 */

char sps30_address = 0x69;      //TODO: understand why 69h, probably arbitrary

unsigned char received_data;    //used for storing received data
char sent_data = 0x01;          //used for data to be transmitted

int TXByteCtr;                  //check number of bytes sent
int Rx = 0;                     //boolean, RX: 1 TX: 0


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    init_i2c();

    while(1) {
          //Transmit process
          Rx = 0;
          TXByteCtr = 1;
          Transmit();

          //Receive process
          Rx = 1;
          Receive();
      }
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void) //interrupt used both for TX and RX
{
  if(Rx == 1){                                  //if receiving
      received_data = UCB0RXBUF;                //get RX data
      __bic_SR_register_on_exit(CPUOFF);        //exit interrupt
  }

  else {                                        //if transmitting
      if (TXByteCtr)                            //check TX byte counter
        {
          UCB0TXBUF = sent_data;                //load TX buffer
          TXByteCtr--;                          //decrement TX byte counter
        }
        else
        {
          UCB0CTL1 |= UCTXSTP;                  //trasmit STOP (end of TX)
          IFG2 &= ~UCB0TXIFG;                   //clear usci interrupt flag
          __bic_SR_register_on_exit(CPUOFF);    //exit interrupt
        }
 }

}

int init_i2c(void) {
    P4SEL |= BIT1;        //enable special function for pins (i2c)
    P4SEL |= BIT2;

    P4REN |= BIT1;        //enable special function for pins (i2c)
    P4REN |= BIT2;

    IE2 |= UCB0RXIE;      //Enable RX interrupt TODO: search for IE2 register
    IE2 |= UCB0TXIE;      //Enable TX interrupt

    UCB0CTL1 |= UCSWRST;  //reset USCI
    UCB0CTL1 |= UCSSEL_2; // use SMCLK (1 MHz)

    UCB0BR0 = 12;         //frequency divider --> final clock = SMCLK/12 = ~100kHz
    UCB0BR1 = 0;          //second divider, unused

    UCB0CTL0 |= UCMST     //set MSP as master
    UCB0CTL0 |= UCMODE_3; //set I2C mode
    UCB0CTL0 |= UCSYNC;   //set synchronous mode

    UCB0I2CSA = sps30_address;     //assign address to slave (sensor)

    UCB0CTL1 &=~ UCSWRST; //end of reset, resume operations

    __enable_interrupt(); //enter interrupt
}

int trasmit(void) {
    while (UCB0CTL1 & UCTXSTP); //check if STOP bit is 1
                                    //if it is not, then go on

    UCB0CTL1 |= UCTR;       //set as transmitter
    UCB0CTL1 |= UCTXSTT;    //transmit START

    __bis_SR_register(CPUOFF + GIE); //enter interrupt
}

int receive(void) {
    while (UCB0CTL1 & UCTXSTP); //check if STOP bit is 1
                                    //if it is not, then go on

    UCB0CTL1 &=~ UCTR;          //set as receiver
    UCB0CTL1 |= UCTXSTT;        //transmit START

    while (UCB0CTL1 & UCTXSTT); //Start condition sent? Check if START bit is 1
    UCB0CTL1 |= UCTXSTP;        //when it is not anymore, it stops receiving

    __bis_SR_register(CPUOFF + GIE); //enter interrupt
}
