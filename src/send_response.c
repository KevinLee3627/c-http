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
    path = "/index";
  }
  size_t path_length = strlen(path) + 1;
  char file_name[path_length + 8]; // + 8 is for ./pages
  snprintf(file_name, strlen(path) + 1 + 8, "./pages%s", path);

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

  // Start crafting response
  char *status_line = "HTTP/1.0 200 OK\r\n";
  int status_line_size = (int)strlen(status_line);

  int content_length_header_size = snprintf(NULL, 0, "Content-Length: %li\r\n\r\n", file_size);
  char content_length_header[content_length_header_size];
  snprintf(content_length_header, (size_t)content_length_header_size, "Content-Length: %li\r\n", file_size);

  char *content_type_header = "Content-Type: text/html\r\n";
  int content_type_header_size = (int)strlen(content_type_header);

  // Create response
  long int response_size = status_line_size + content_length_header_size + content_type_header_size + file_size + 4;
  char response[response_size];
  printf("response_size: %li\nstatus_line_size: %i | content_length_size: %i | content_type_size: %i | file size: %li\n", response_size, status_line_size, content_length_header_size, content_type_header_size, file_size);
  snprintf(response, (size_t)response_size, "%s%s%s\r\n%s", status_line, content_length_header, content_type_header, file_data);

  // Send response
  send(incoming_fd, response, strlen(response), 0);
  return;
}