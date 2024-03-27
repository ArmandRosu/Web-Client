#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "buffer.h"
#include "parson.h"
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char *cookie, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "HOST: %s ", host);
    compute_message(message, line);

    if (token != NULL) {
      memset(line, 0, LINELEN);
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
char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char *cookie, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "HOST: %s ", host);
    compute_message(message, line);
    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    
    for(int i = 0; i + 1 < body_data_fields_count; i++) {
        strcat(body_data_buffer, body_data[i]);
    	strcat(body_data_buffer, "&");
    }
    int aux_body_data_fields_count = body_data_fields_count - 1;
    strcat(body_data_buffer, body_data[aux_body_data_fields_count]);

    sprintf(line, "Content-Type: %s\r\nContent-Length: %ld", content_type, strlen(body_data_buffer));
    compute_message(message, line);

    if (token != NULL) {
      memset(line, 0, LINELEN);
      sprintf(line, "Authorization: Delimiter %s", token);
      compute_message(message, line);
    }


    // Step 4 (optional): add cookies
    if (cookie != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: %s", cookie);
        compute_message(message, line);
    }
    // Step 5: add new line at end of header
    compute_message(message, "");
    // Step 6: add the actual payload data
    memset(line, 0, LINELEN);
    strcat(message, body_data_buffer);

    free(line);
    return message;
}
char *compute_delete_request(char *host, char *url, char *cookie, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "HOST: %s ", host);
    compute_message(message, line);

    if (token != NULL) {
        memset(line, 0, LINELEN);
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