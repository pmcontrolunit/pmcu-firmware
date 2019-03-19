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
int start_measurement();

/*
 This should be used as the second command.
 Receive the confirm of switching the state.
 */
unsigned char* read_start_ack();

/*
 This command turns the state of SPS30 to IDLE-MODE, which uses less power
 */
int stop_measurement();

/*
 This should be used to receive confirm of switching the state to IDLE-MODE, after stopping the measurement
 */
unsigned char* read_stop_ack();

