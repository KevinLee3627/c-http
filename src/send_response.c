#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>

#define ASSET_DIR "./pages"

// send() has a change to not send all the bytes - this ensures it does
int send_data(SSL *ssl, void *data, int data_size)
{
  unsigned char *pdata = (unsigned char *)data;
  int numSent;

  while (data_size > 0)
  {
    numSent = SSL_write(ssl, pdata, data_size);
    if (numSent <= 0)
    {
      ERR_print_errors_fp(stderr);
      return -1;
    }
    pdata += numSent;
    data_size -= numSent;
  }
  return 0;
}

void send_404(SSL *ssl)
{
  // TODO: ALso please find better names and don't hardcode these values
  // TODO: How to generalize send_response so we can easily send the 404 page?

  printf("File not found\n");
  char *response = "HTTP/1.0 404 Not Found\r\nContent-Length: 826\r\nContent-Type: text/html\r\n";
  send_data(ssl, response, (int)strlen(response));
  int data_404_size = 826;
  FILE *file_404 = fopen("./pages/404.html", "rb");
  unsigned char file_data_404[data_404_size];
  fread(file_data_404, (size_t)(data_404_size), 1, file_404); // Throw data into file_data
  send_data(ssl, file_data_404, data_404_size);
  fclose(file_404);
}

void send_301(SSL *ssl, char *path)
{
  printf("Attempted access to http version\n");
  char buffer[256];
  char *response = "HTTP/1.0 301 Moved Permanently\r\nLocation: https://kevinjlee.xyz";
  strcpy(buffer, response);
  strcat(buffer, path);

  int final_length = (int)strlen(buffer);
  send_data(ssl, buffer, final_length);
}

void send_response(SSL *ssl, char *path)
{
  if (strcmp(path, "/") == 0)
  {
    path = "/index.html";
  }

  int file_path_size = snprintf(NULL, 0, "%s%s", ASSET_DIR, path);
  char *file_path = malloc((size_t)(file_path_size + 1)); // yay null terminator
  snprintf(file_path, (size_t)(file_path_size + 1), "%s%s", ASSET_DIR, path);

  // don't let people get cheeky and start poking around the filesystem
  if (strstr(file_path, "..") != NULL || strchr(file_path, '~') != NULL)
  {
    printf("Stay out!\n");
    send_404(ssl);
    exit(0);
  }
  // libmagic didn't work, so here's a temp? solution...
  // Open the mime file, search for the extension
  char mime[50];
  // Move file_path pointer over by one because it starts with a period
  char *extension = strrchr(file_path + 1, '.');
  // If there is no extension, set it to .html
  if (extension == NULL)
  {
    file_path_size += 5;
    file_path = realloc(file_path, (size_t)(file_path_size + 1)); // yay null terminator
    snprintf(file_path, (size_t)(file_path_size + 1), "%s%s.html", ASSET_DIR, path);
    extension = ".html";
  }
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
      strncpy(mime, tmp_mime, strlen(tmp_mime) + 1); // dpn't forget +1 to copy null terminator...
      break;
    }
  }
  fclose(mime_file);

  // Use binary read so we can serve other files like images
  FILE *file = fopen(file_path, "rb");
  free(file_path);
  if (file == NULL)
  {
    send_404(ssl);
    return;
  }
  struct stat file_stats;
  // fileno gets the file descriptor; fopen is the stream
  fstat(fileno(file), &file_stats);
  off_t file_size = file_stats.st_size;

  char *status_line = "HTTP/1.0 200 OK\r\n";
  // DON'T SEND strlen(...) + 1 bytes!!! the +1 sends a null terminator, which makes things weird
  send_data(ssl, status_line, (int)strlen(status_line));

  int headers_size = snprintf(NULL, 0, "Content-Length: %li\r\nContent-Type: %s\r\n", file_size, mime);
  char headers[headers_size + 1];
  snprintf(headers, (size_t)(headers_size + 1), "Content-Length: %li\r\nContent-Type: %s\r\n", file_size, mime);
  send_data(ssl, headers, (int)strlen(headers));

  send_data(ssl, "\r\n", 2);
  unsigned char file_data[file_size];
  fread(file_data, (size_t)(file_size), 1, file); // Throw data into file_data
  send_data(ssl, file_data, (int)file_size);
  fclose(file);

  // printf("RESPONSE:\n%s%s", status_line, headers);
  return;
}