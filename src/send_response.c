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
  // +5 is for pages, +1 for null terminator
  size_t path_length = strlen(path) + 1;
  char file_name[path_length + 5];
  snprintf(file_name, path_length + 5, "pages%s", path);
  printf("File name: %s\n", file_name);

  // libmagic didn't work, so here's a temp? solution...
  char *mime = calloc(1, 50);
  char *extension = strrchr(file_name, '.');
  // Open the mime file, search for the extension
  FILE *mime_file = fopen("./src/mime.types", "r");
  char line[51];
  while (fgets(line, sizeof(line), mime_file))
  {
    // Check if extension is in this line
    if (strstr(line, extension) != NULL)
    {
      char *tmp_mime = strchr(line, ',') + 1;
      // tmp_mime[strcspn(tmp_mime, "\n")] = 0;
      strncpy(mime, tmp_mime, strlen(tmp_mime));
      break;
    }
  }
  // Use binary read so we can serve other files like images
  FILE *file = fopen(file_name, "rb");
  if (file == NULL)
  {
    // TODO: How to generalize send_response so we can easily send the 404 page?
    printf("File not found\n");
    char *response = "HTTP/1.0 404 Not Found";
    send(incoming_fd, response, strlen(response), 0);
    return;
  }

  fseek(file, 0, SEEK_END);
  size_t file_size = (size_t)ftell(file);
  // move back to start to start reading
  fseek(file, 0, SEEK_SET);
  // TODO: Why is this of type char for a binary file, and why does it work?
  char file_data[file_size];                      // space for \0
  fread(file_data, (size_t)(file_size), 1, file); // Throw data into file_data
  fclose(file);
  // file_data[file_size] = '\0'; // Add null terminator to avoid overflows when using file data as a string

  char *status_line = "HTTP/1.0 200 OK\r\n";
  // Start crafting response
  int content_length_header_size = snprintf(NULL, 0, "Content-Length: %li", file_size);
  char content_length_header[content_length_header_size + 1];
  snprintf(content_length_header, (size_t)content_length_header_size, "Content-Length: %li", file_size);

  printf("mime type string: %sX\n", mime);
  int content_type_header_size = snprintf(NULL, 0, "Content-Type: %s", mime);
  char content_type_header[content_type_header_size];
  snprintf(content_type_header, (size_t)content_type_header_size, "Content-Type: %s", mime);

  // TODO: Naive function to take the file extension from path and turn that into content-type header
  // char *content_type_header = "Content-Type: text/html\r\n";
  // int content_type_header_size = (int)strlen(content_type_header);

  long int response_headers_size = content_length_header_size + content_type_header_size + 4;
  char response_headers[response_headers_size];
  snprintf(response_headers, (size_t)response_headers_size, "%s\r\n%s\r\n", content_length_header, content_type_header);

  // Send response
  send(incoming_fd, status_line, strlen(status_line), 0);
  send(incoming_fd, response_headers, strlen(response_headers), 0);
  send(incoming_fd, file_data, file_size, 0);
  free(mime);
  fclose(mime_file);

  return;
}