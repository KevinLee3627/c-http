#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>

int send_data(int incoming_fd, void *data, ssize_t data_size)
{
  unsigned char *pdata = (unsigned char *)data;
  ssize_t numSent;

  while (data_size > 0)
  {
    numSent = send(incoming_fd, pdata, (size_t)data_size, 0);
    if (numSent == -1)
    {
      return -1;
    }
    pdata += numSent;
    data_size -= numSent;
  }
  return 0;
}

void send_response(int incoming_fd, char *path)
{
  if (strcmp(path, "/") == 0)
  {
    path = "/index.html";
  }
  // +5 is for pages, +1 for null terminator
  size_t path_length = strlen(path) + 1;
  char file_path[path_length + 5];
  snprintf(file_path, path_length + 5, "pages%s", path);
  // printf("File name: %s\n", file_path);

  // libmagic didn't work, so here's a temp? solution...
  // Open the mime file, search for the extension
  char mime[50];
  char *extension = strrchr(file_path, '.');

  FILE *mime_file = fopen("./src/mime.types", "r");
  char line[51];
  while (fgets(line, sizeof(line), mime_file))
  {
    if (line == NULL)
    {
      printf("Unknown file mime type.\n");
      break;
    }
    // Check if extension is in this line
    if (strstr(line, extension) != NULL)
    {
      char *tmp_mime = strchr(line, ',') + 1;
      tmp_mime[strcspn(tmp_mime, "\n")] = '\0';
      strncpy(mime, tmp_mime, strlen(tmp_mime));
      break;
    }
  }
  fclose(mime_file);

  // Use binary read so we can serve other files like images
  FILE *file = fopen(file_path, "rb");
  if (file == NULL)
  {
    // TODO: How to generalize send_response so we can easily send the 404 page?
    printf("File not found\n");
    char *response = "HTTP/1.0 404 Not Found";
    send(incoming_fd, response, strlen(response), 0);
    return;
  }

  // Get size of file in bytes
  struct stat file_stats;
  // fileno gets the file descriptor; fopen is the stream
  fstat(fileno(file), &file_stats);
  off_t file_size = file_stats.st_size;

  char *status_line = "HTTP/1.0 200 OK\r\n";
  // DON'T SEND strlen(...) + 1 bytes!!! the +1 sends a null terminator, which makes things weird
  send(incoming_fd, status_line, strlen(status_line), 0);

  int headers_size = snprintf(NULL, 0, "Content-Length: %li\r\nContent-Type: %s\r\n", file_size, mime);
  char headers[headers_size + 1];
  snprintf(headers, (size_t)(headers_size + 1), "Content-Length: %li\r\nContent-Type: %s\r\n", file_size, mime);
  send(incoming_fd, headers, strlen(headers), 0);

  send(incoming_fd, "\r\n", 2, 0);
  // TODO: Why is this of type char for a binary file, and why does it work?
  char file_data[file_size];
  fread(file_data, (size_t)(file_size), 1, file); // Throw data into file_data
  send_data(incoming_fd, file_data, file_size);
  fclose(file);

  printf("RESPONSE:\n%s%s", status_line, headers);
  // send(incoming_fd, file_data, (size_t)file_size, 0);
  return;
}