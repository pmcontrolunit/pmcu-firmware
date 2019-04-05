#include "time.h"
#include <msp430.h>

unsigned long int execution_time = 0;

void start_timing(void){
    TB0CTL |= TBSSEL_2; //SMCLK = 1 MHz
    TB0CTL |= ID_3; //1MHz/8
    TB0CTL |= MC_3; //up-down mode

    TB0CCR0 = 62500; //value the timer must reach ( T=62550*10^(-6)*8=0,5sec)

    TB0CTL &= ~TBIFG; //reset TBIFG
    TB0CTL |= TBIE; //timer B0 interrupt enable
    __enable_interrupt(); //GIE

}

unsigned long int get_execution_time(void) {
    return execution_time;
}

#pragma vector = TIMER0_B1_VECTOR
__interrupt void timer_b1_isr(void){
    execution_time += 1; //+1 second
    TB0CTL &= ~TBIFG; //reset TAIFG

}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;

    start_timing();
    while(1);
}
