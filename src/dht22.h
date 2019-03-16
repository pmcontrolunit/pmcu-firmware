#ifndef DHT22
#define DHT22

#include <msp430.h>

#define DHT22_TIMEOUT 1 // Timeout delay expressed in seconds (can't be higher than 15).

/*
 * DHT22_ERROR values:
 */
#define DHT22_NO_ERROR              0x00 // No error encountred.
#define DHT22_FLOATING_ERROR        0x01 // Default vaule when no read has been done. Is also returned if a reading is on-going.
#define DHT22_GENERIC_ERROR         0x02 // Generic communication error.
#define DHT22_TIMEOUT_ERROR         0x03 // The communication took more time than DHT22_TIMEOUT value.
#define DHT22_WRONG_CHECKSUM_ERROR  0x04 // Received checksum doesn't match computated.

/*
 * Returns the latest RH value (for humidity measures) read, mutiplied by 10 (for performance reasons).
 *
 * To get the real value, issue:
 * rh = dht22_rh / 10;
 */
int dht22_get_rh();

/*
 * Returns the latest temperature value read, multiplied by 10 (for performance reasons).
 *
 * To get the real value, issue:
 * temperature = dht22_temperature / 10;
 */
int dht22_get_temperature();

/*
 * Returns the latest DHT22_ERROR encountred during DHT22 read request.
 * If 0, means that the last communication gone fine and values can be read.
 */
unsigned int dht22_get_latest_error();

/*
 * Synchronously sends a read request to the DHT22 sensor, receives data and saves the result.
 * If all gone well, results are stored on dht22_rh and dht22_temperature variables.
 *
 * NOTE: before reading, check the value returned from dht22_get_error().
 *
 * Returns 0 if any error was encountred, otherwise returns a DHT22_ERROR.
 */
unsigned int dht22_read();

#endif
