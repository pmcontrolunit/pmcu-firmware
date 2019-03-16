#ifndef GPS_H_
#define GPS_H_

typedef struct {

} gps_data;

int gps_read_sentence(unsigned char *sentence_type, unsigned char *buffer, unsigned int buffer_length);

int gps_read();

#endif
