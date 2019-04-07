#ifndef MODEM_H_
#define MODEM_H_

int modem_init();

int modem_connect(const char *host, const char *port);

int modem_sendall(void *fd, const void *buffer, unsigned int buffer_length, int flags);

int modem_recvall(void *fd, void *buffer, unsigned int buffer_length, int flags);

int modem_disconnect();

int modem_halt();

#endif
