#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

void send_response(int incoming_fd, char *path)
{
  if (strcmp(path, "/") == 0)
  {
    path = "/index.html";
  }
  // +7 is for ./pages, +1 for null terminator
  size_t path_length = strlen(path) + 1;
  char file_name[path_length + 7];
  snprintf(file_name, strlen(path) + 1 + 7, "./pages%s", path);

  // Use binary read so we can serve other files like images
  FILE *file = fopen(file_name, "rb");
  if (file == NULL)
  {
    printf("File not found\n");
    char *response = "HTTP/1.0 404 Not Found";
    send(incoming_fd, response, strlen(response), 0);
    return;
  }

  fseek(file, 0, SEEK_END);
  long int file_size = ftell(file);
  // move back to start to start reading
  fseek(file, 0, SEEK_SET);
  char file_data[file_size + 1];                  // space for \0
  fread(file_data, (size_t)(file_size), 1, file); // Throw data into file_data
  fclose(file);
  file_data[file_size] = '\0'; // Add null terminator to avoid overflows when using file data as a string

  char *status_line = "HTTP/1.0 200 OK\r\n";

  // Start crafting response headers
  int content_length_header_size = snprintf(NULL, 0, "Content-Length: %li\r\n\r\n", file_size);
  char content_length_header[content_length_header_size];
  snprintf(content_length_header, (size_t)content_length_header_size, "Content-Length: %li\r\n", file_size);

  char *content_type_header = "Content-Type: text/html\r\n";
  int content_type_header_size = (int)strlen(content_type_header);

  long int response_headers_size = content_length_header_size + content_type_header_size + 4;
  char response_headers[response_headers_size];
  snprintf(response_headers, (size_t)response_headers_size, "%s%s\r\n", content_length_header, content_type_header);

  // Send response
  send(incoming_fd, status_line, strlen(status_line), 0);
  send(incoming_fd, response_headers, strlen(response_headers), 0);
  send(incoming_fd, file_data, (size_t)file_size, 0);
  send(incoming_fd, "\r\n", 2, 0);
  return;
}