// Read file
// Get size of file
// Format response
// send()!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

void send_response()
{
  FILE *html_file = fopen("./pages/index.html", "r");
  int file_size = fseek(html_file, 0, SEEK_END);
  // move back to start to start reading
  fseek(html_file, 0, SEEK_SET);
  char file_data[file_size + 1];
  fread(file_data, sizeof(char) * (size_t)(file_size + 1), 1, html_file); // Throw data into file_data

  char *status_line = "HTTP/1.0 200 OK\r\n";
  size_t status_line_size = sizeof(status_line);

  int header_size = snprintf(NULL, 0, "Content-Length: %i\r\n", file_size) + 1;
  char header_content_length[header_size];
  snprintf(header_content_length, (size_t)header_size, "Content-Length: %i\r\n", file_size);

  char *response = NULL;
  snprintf(response, status_line_size + (size_t)header_size, "%s%s\r\n%s", status_line, header_content_length, file_data);
  printf("%s", response);

  // send(incoming_socket, response, strlen(response), 0);
}