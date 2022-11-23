#ifndef SEND_RESPONSE_H
#define SEND_RESPONSE_H
int send_data(SSL *ssl, void *data, long data_size);
void send_404(SSL *ssl);
void send_301(SSL *ssl, char *path);
void send_response(SSL *ssl, char *path);
#endif