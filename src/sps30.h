#include "uart.h"

/*
 Use this enum to select the desired type of data in "sps30_get_measure" function (at the end of the file)
 Note that:
 -"mass" expresses microgram per meter^3 (ug/^3) of the particle
 -"num" expresses the number of particles in a centimeter^3 (n/cm^3)
 -"typical_size" is the average dimension in micrometer (um) of the particles (probably)
 */
typedef enum {
   mass_PM1,
   mass_PM2dot5,
   mass_PM4,
   mass_PM10,
   num_PM0dot5,
   num_PM1,
   num_PM2dot5,
   num_PM4,
   num_PM10,
   typical_size
} Measure;


/*
 This must be used as the very first command.
 It switches the state of SPS30 from IDLE-MODE to MEASURING-MODE.
 */
int sps30_start_measurement();

/*
 This should be used as the second command.
 Receive the confirm of switching the state.
 It is a debug version of the command because stores the response in an array (buff).
 Recommended length: 8
 */
int sps30_read_start_ack(unsigned char* buff);



/*
 This command turns the state of SPS30 to IDLE-MODE, which uses less power
 */
int sps30_stop_measurement();

/*
 This should be used to receive confirm of switching the state to IDLE-MODE, after stopping the measurement.
 It is a debug version, indeed involves an array (buff) as a parameter, in it will be stored the response.
 Recommended length: 8
 */
int sps30_read_stop_ack(unsigned char* buff);

/*
 This should be used to ask the SPS30 to send the measures. NOTE: do not ask more than once per second.
 */
int sps30_ask_measured_values();

/*
 Receive the measured values in an array (buff) passed as a parameter. Recommended length: 47
 */
int sps30_read_measured_values(unsigned char* buff);


/*
 Execute manually the fan cleaning (to be done periodically in order to guarantee the quality of the measures).
 Default fan-cleaning interval: 160 hours (too much!)
 */
int sps30_start_fan_cleaning();

/*
 Receive the confirm of the start of fan-cleaning.
 */
int sps30_read_fan_ack(unsigned char* buff);


/*
 Use this function to extract the desired information from the measurement.
 Insert argument this way:
 -"buff" is the array in which data is going to be saved (recommended dimensions: 4)
 -"measures" is the array in which data from "sps30_read_measured_values" has been saved
 -"measure" is the type of data you need (see the enum at the top of the file)
 */
int sps30_get_measure(unsigned char buff[], unsigned char measures[], Measure measure);
