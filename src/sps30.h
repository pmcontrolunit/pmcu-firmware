#include "uart.h"

/*
 * sps30.h
 *
 *  Created on: 18 mar 2019
 *      Author: Alessandro
 */


/*
 This must be used as the very first command.
 It switches the state of SPS30 from IDLE-MODE to MEASURING-MODE.
 */
int sps30_start_measurement();

/*
 This should be used as the second command.
 Receive the confirm of switching the state.
 It is a debug version of the command because stores the response in an array (buff).
 Raccomended length: 8
 */
int sps30_read_start_ack(unsigned char* buff);



/*
 This command turns the state of SPS30 to IDLE-MODE, which uses less power
 */
int sps30_stop_measurement();

/*
 This should be used to receive confirm of switching the state to IDLE-MODE, after stopping the measurement.
 It is a debug version, indeed involves an array (buff) as a parameter, in it will be stored the response.
 Raccomended length: 8
 */
int sps30_read_stop_ack(unsigned char* buff);

/*
 This should be used to ask the SPS30 to send the measures. NOTE: do not ask more than once per second.
 */
int sps30_ask_measured_values();

/*
 Receive the measured values in an array (buff) passed as a parameter. Raccomended length: 47
 */
int sps30_read_measured_values(unsigned char* buff);


/*
 Excecute manually the fan cleaning (to be done periodically in order to guarantee the quality of the measures).
 Default fan-cleaning interval: 160 hours (too much!)
 */
int sps30_start_fan_cleaning();

/*
 Receive the confirm of the start of fan-cleaning.
 */
int sps30_read_fan_ack(unsigned char* buff);
