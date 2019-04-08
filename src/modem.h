#ifndef MODEM_H_
#define MODEM_H_

#define MODEM_OK    0
#define MODEM_ERROR 1

const char *modem_get_latest_log();

int modem_init();

int modem_connect(const char *host, const char *port);

int modem_send(void *fd, const void *buffer, unsigned int buffer_length, int flags);

int modem_disconnect();

#endif
