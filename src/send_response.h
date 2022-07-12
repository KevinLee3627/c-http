#ifndef SEND_RESPONSE_H
#define SEND_RESPONSE_H
int send_data(SSL *ssl, void *data, long data_size);
void send_404(void);
void send_response(SSL *ssl, char *path);
#endif