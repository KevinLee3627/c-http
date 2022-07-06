#ifndef SEND_RESPONSE_H
#define SEND_RESPONSE_H
int send_data(int incoming_fd, void *data, long data_size);
void send_404();
void send_response(int incoming_fd, char *path);
#endif