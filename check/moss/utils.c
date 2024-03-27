#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "buffer.h"
#include "parson.h"
#include "helpers.h"
#include "requests.h"

// am luat aceasta functie din cadrul laboratorului 10, cu o mica
// modificare: am un singur cookie pentru tema si un token
char *compute_get_request(char *host, char *url, char *query_params,
                            char *cookie, char* token) {
   int i;
   char *message = calloc(BUFLEN, sizeof(char));
   char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) { // verific daca am parametrii de cerere
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    // adaug linia lin in mesajul msg
    compute_message(message, line); // afisez prima oara formatul cererii

    // Step 2: add the host
    memset(line, 0, LINELEN);
    // retin host-ul
    sprintf(line, "HOST: %s ", host); // pe urmatoarea linie afisez host-ul
    // adaug pe linia urmatoare a mesajului linia aferenta host-ului
    compute_message(message, line);

    // adaug token
    if (token != NULL) {
      memset(line, 0, LINELEN);
      // adaug un delimitator intre authorization si token
      sprintf(line, "Authorization: Delimiter %s", token);
      compute_message(message, line);
    }

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookie != NULL) {
      memset(line, 0, LINELEN);
      sprintf(line, "Cookie: %s", cookie);
      compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");

    return message;
}

// am luat aceasta functie din cadrul laboratorului 10, cu o mica modificare:
// am un singur cookie si un token
char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char *cookie, char* token) {
    int i, size;
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url); // retin prima linie pentru mesaj
    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN); // resetez linia
    sprintf(line, "HOST: %s ", host); // retin hostul
    compute_message(message, line); // adaug o linie cu host-ul

    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    size = 0;
    i = 0;
    // retin in body_data_buffer valorile din body_data, cu & intre ele
    while (i < body_data_fields_count - 1) {
      strcat(body_data_buffer, body_data[i]);
    	strcat(body_data_buffer, "&");
      // adaug la size dimensiunea pentru body_data si 1 pentru caracterul '&'
    	//size = size + strlen()
      i++;
    }
    // retin si ultima valoare din body_data
    strcat(body_data_buffer, body_data[body_data_fields_count - 1]);
    size = size + strlen(body_data_buffer);

    // retin tipul continutului si dimensiunea calculata anterior
    sprintf(line, "Content-Type: %s\r\nContent-Length: %d", content_type, size);
    // adaug linia la mesaj
    compute_message(message, line);

    // adaug token
    if (token != NULL) {
      memset(line, 0, LINELEN);
      // adaug un delimitator intre authorization si token
      sprintf(line, "Authorization: Delimiter %s", token);
      compute_message(message, line);
    }

    // Step 4 (optional): add cookies
  	if (cookie != NULL) {
      memset(line, 0, LINELEN);
      //strcat(line, cookie);
      sprintf(line, "Cookie: %s", cookie);
      compute_message(message, line);
    }
    // Step 5: add new line at end of header
    compute_message(message, "");
    // Step 6: add the actual payload data
    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);

    return message;
}

char* compute_delete_request(char* host, char* url, char* cookie, char* token) {
  int i;
  char *message = calloc(BUFLEN, sizeof(char));
  char *line = calloc(LINELEN, sizeof(char));

  sprintf(line, "DELETE %s HTTP/1.1", url); // retin prima linie pentru mesaj
  compute_message(message, line);

  memset(line, 0, LINELEN); // resetez linia
  sprintf(line, "HOST: %s ", host); // retin hostul
  compute_message(message, line); // adaug o linie cu host-ul

  // adaug token
  if (token != NULL) {
    memset(line, 0, LINELEN);
    // adaug un delimitator intre authorization si token
    sprintf(line, "Authorization: Delimiter %s", token);
    compute_message(message, line);
  }

  // Step 3 (optional): add headers and/or cookies, according to the protocol format
  if (cookie != NULL) {
    memset(line, 0, LINELEN);
    sprintf(line, "Cookie: %s", cookie);
    compute_message(message, line);
  }
  // Step 4: add final new line
  compute_message(message, "");

  return message;

}
