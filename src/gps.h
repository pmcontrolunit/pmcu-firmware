#ifndef GPS_H_
#define GPS_H_

/*
 * Stores a GPS sentence on a "buffer", according to NMEA protocol.
 * The "sentence_type" must be formatted like the following: $GPGGA, $GPGSA...
 *
 * Returns 0 if all goes well.
 */
int gps_read_sentence(unsigned char *sentence_type, unsigned char *buffer, unsigned int buffer_offset, unsigned int buffer_length);

#endif
