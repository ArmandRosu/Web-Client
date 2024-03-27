#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "parson.h"
#include "buffer.h"
#include "requests.h"
#include "helpers.h"

void client_register() {
  // creez conexiunea cu server-ul
  int socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

  // aloc spatiu pentru username
  char* register_username = malloc(100 * sizeof(char));
  // aloc spatiu pentru parola
  char* register_password = malloc(100 * sizeof(char));

  // se asteapta introducerea de la tastatura a unui username
  printf("username=");
  fgets(register_username, 100, stdin);
  register_username = strtok(register_username, "\n");
  // se asteapta introducerea de la tastatura a unei parole
  printf("password=");
  fgets(register_password, 100, stdin);
  register_password = strtok(register_password, "\n");

  // formez mesajul de tip JSON - pentru acest lucru, m-am inspirat din
  // urmatorul link: https://github.com/kgabis/parson/blob/master/README.md
  JSON_Value *value_for_json = json_value_init_object();
  JSON_Object *element_for_json = json_value_get_object(value_for_json);

  json_object_set_string(element_for_json, "username", register_username);
  json_object_set_string(element_for_json, "password", register_password);

  char* string = NULL;
  string = json_serialize_to_string_pretty(value_for_json);

  // retin mesajul trimis catre server
  char* msg_to_server = compute_post_request("34.118.48.238:8080",
    "/api/v1/tema/auth/register", "application/json", &string, 1, NULL, NULL);

  // trimit mesajul catre server
  send_to_server(socket, msg_to_server);
  // primesc raspunsul de la server pentru mesajul trimis
  char* msg_from_server = receive_from_server(socket);

  // din mesaul primit de la server, extrag codul de exit folosind strtok;
  // pentru acest lucru, mam inspirat din urmatorul link:
  // https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
  msg_from_server = strtok(msg_from_server, " ");
  msg_from_server = strtok(NULL, " ");

  if (atoi(msg_from_server) == 200 || atoi(msg_from_server) == 201) {
    printf("OK\n");
  } else {
    printf("ERROR: Register failed!\n");
  }

  // eliberez memoria
  free(register_password);
  free(register_username);

  json_free_serialized_string(string);
  json_value_free(value_for_json);

  // inchid conexiunea cu server-ul
  close(socket);
}

char* client_login() {
  // creez conexiunea cu server-ul
  int socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

  // aloc spatiu pentru username
  char* login_username = malloc(100 * sizeof(char));
  // aloc spatiu pentru parola
  char* login_password = malloc(100 * sizeof(char));

  // se asteapta introducerea de latastatura a username-ului pentru login
  printf("username=");
  fgets(login_username, 100, stdin);
  // se asteapta introducerea de la tastatura a parolei pentru login
  printf("password=");
  fgets(login_password, 100, stdin);

  // din cele doua siruri de caratere, elimin caracterul '\n' de la final;
  // pentru acest lucru am folosit functia strtok
  login_username = strtok(login_username, "\n");
  login_password = strtok(login_password, "\n");

  // formez mesajul pe care il trimit catre server, folosind biblioteca parson:
  // https://github.com/kgabis/parson/blob/master/README.md
  // procedez similar ca la functia client_register()

  JSON_Value *value_for_json = json_value_init_object();
  JSON_Object *element_for_json = json_value_get_object(value_for_json);

  json_object_set_string(element_for_json, "username", login_username);
  json_object_set_string(element_for_json, "password", login_password);

  char* string = NULL;
  string = json_serialize_to_string_pretty(value_for_json);

  char* msg_to_server = compute_post_request("34.118.48.238:8080",
    "/api/v1/tema/auth/login", "application/json", &string, 1, NULL, NULL);

  // trimit mesajul catre server
  send_to_server(socket, msg_to_server);
  // primesc raspunsul de la server pentru mesajul trimis
  char* msg_from_server = receive_from_server(socket);

  // mai intai, extrag intr-o variabila exit_code codul de exit pentru a
  // verifica corectitudinea mesajului trimis
  char* exit_code = malloc(10000 * sizeof(char));
  memcpy(exit_code, msg_from_server, strlen(msg_from_server));
  exit_code[strlen(msg_from_server)] = '\0'; // adaug null terminator
  exit_code = strtok(exit_code, " ");
  exit_code = strtok(NULL, " ");

  if (atoi(exit_code) == 200) {
    // daca totul e ok, afisez cookie de sesiune
    char* cookie_session = malloc(10000 * sizeof(char));
    // pentru a extrage partea de
    cookie_session = strstr(msg_from_server, "Set-Cookie");
    cookie_session = strtok(cookie_session, ": ");
    cookie_session = strtok(NULL, ": ");
    cookie_session = strtok(cookie_session, ";");

    puts(cookie_session);

    // inchid conexiunea cu server-ul
    close(socket);
    // eliberez memoria
    free(login_password);
    free(login_username);

    return cookie_session;
  } else {
    printf("ERROR: Login failed!\n");
    // inchid conexiunea cu server-ul
    close(socket);
    // eliberez memoria
    free(login_password);
    free(login_username);
    return NULL;
  }
}

// Pentru verificarea autentificarii, de foloseste parametrul cookie_session
char* client_enter_library(char* cookie_session) {
  // creez conexiunea cu server-ul
  int socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
  // formez mesjaul pe care vreau sa il tirmit catre server
  char* msg_to_server = compute_get_request("34.118.48.238:8080",
    "/api/v1/tema/library/access", NULL, cookie_session, NULL);
  // trimit mesajul catre server
  send_to_server(socket, msg_to_server);

  // primesc raspunsul de la server pentru mesajul trimis
  char* msg_from_server = receive_from_server(socket);

  // extrag exit code-ul din mesajul primit de la server
  char* exit_code = malloc(10000 * sizeof(char));
  memcpy(exit_code, msg_from_server, strlen(msg_from_server));
  exit_code[strlen(msg_from_server)] = '\0';
  exit_code = strtok(exit_code, " ");
  exit_code = strtok(NULL, " ");

  if (atoi(exit_code) == 200) {
    // daca este OK, atunci determin token-ul, il afisez si il returnez
    // extrag token-ul din mesaj
    char* token = malloc(1000 * sizeof(char));
    token = strstr(msg_from_server, "token");
    token = strtok(token, ":");
    token = strtok(NULL, ":");
    token = strtok(token, "\"");
    // afisez token-ul
    puts(token);
    // inchid conexiunea cu server-ul
    close(socket);
    return token;
  } else if (atoi(exit_code) == 400) {
    printf("ERROR: Bad request!\n");
    // inchid conexiunea cu server-ul
    close(socket);
    return NULL;
  } else {
    printf("ERROR: You do not have access to the library!\n");
    close(socket);
    return NULL;
  }
}

void client_get_books(char* cookie, char* token) {
  // creez conexiunea cu server-ul
  int socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

  // formez mesajul pe care il trimit catre server
  char* msg_to_server = compute_get_request("34.118.48.238:8080",
    "/api/v1/tema/library/books", NULL, cookie, token);

  // trimit mesajul catre server
  send_to_server(socket, msg_to_server);

  // primesc raspunsul de la server pentru mesajul trimis
  char* msg_from_server = receive_from_server(socket);

  // extrag exit_code
  char* exit_code = malloc(1000 * sizeof(char));
  memcpy(exit_code, msg_from_server, strlen(msg_from_server));
  exit_code[strlen(msg_from_server)] = '\0';
  exit_code = strtok(exit_code, " ");
  exit_code = strtok(NULL, " ");

  if (atoi(exit_code) == 200) {
    char* books = malloc(1000 * sizeof(char));
    books = strstr(msg_from_server, "[");
    puts(books);

  } else if (atoi(exit_code) == 403) {
    printf("ERROR: You do not have access to the library!\n");
  } else {
    printf("ERROR: Bad Request!\n");
  }

  close(socket);
}

void client_add_book(char* cookie, char* token) {
  // creez conexiunea cu server-ul
  int socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

  char* title = malloc(100 * sizeof(char));
  char* author = malloc(100 * sizeof(char));
  char* genre = malloc(100 * sizeof(char));
  char* publisher = malloc(100 * sizeof(char));
  int page_count;

  // fac citirea de la tastatura pentru fiecare camp
  printf("title=");
  fgets(title, 100, stdin);
  title = strtok(title, "\n");

  printf("author=");
  fgets(author, 100, stdin);
  author = strtok(author, "\n");

  printf("genre=");
  fgets(genre, 100, stdin);
  genre = strtok(genre, "\n");

  printf("publisher=");
  fgets(publisher, 100, stdin);
  publisher = strtok(publisher, "\n");

  printf("page_count=");
  scanf("%d", &page_count);

  // construiesc mesajul in format JSON

  JSON_Value *value_for_json = json_value_init_object();
  JSON_Object *element_for_json = json_value_get_object(value_for_json);

  json_object_set_string(element_for_json, "title", title);
  json_object_set_string(element_for_json, "author", author);
  json_object_set_string(element_for_json, "genre", genre);
  json_object_set_string(element_for_json, "publisher", publisher);
  json_object_set_number(element_for_json, "page_count", page_count);

  char* string = NULL;
  string = json_serialize_to_string_pretty(value_for_json);

  // retin mesajul trimis catre server
  char* msg_to_server = compute_post_request("34.118.48.238:8080",
    "/api/v1/tema/library/books", "application/json", &string, 1, cookie, token);

  // trimit mesajul catre server
  send_to_server(socket, msg_to_server);
  // primesc raspunsul de la server pentru mesajul trimis
  char* msg_from_server = receive_from_server(socket);

  // extrag exit_code
  char* exit_code = malloc(1000 * sizeof(char));
  memcpy(exit_code, msg_from_server, strlen(msg_from_server));
  exit_code[strlen(msg_from_server)] = '\0';
  exit_code = strtok(exit_code, " ");
  exit_code = strtok(NULL, " ");

  if (atoi(exit_code) == 200) {
    printf("OK\n");
  } else if (atoi(exit_code) == 403) {
    printf("ERROR: You do not have access to the library!\n");
  } else {
    printf("ERROR: The fields are not correct!\n");
  }

  free(title);
  free(author);
  free(genre);
  free(publisher);

  json_free_serialized_string(string);
  json_value_free(value_for_json);

  // inchid conexiunea cu server-ul
  close(socket);
}

void client_get_book(char* cookie, char* token) {
  // creez conexiunea cu server-ul
  int socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

  char* id = malloc(100 * sizeof(char));
  printf("id=");
  fgets(id, 100, stdin);
  id = strtok(id, "\n");

  // formez url-ul
  char* url = malloc(100 * sizeof(char));
  memcpy(url, "/api/v1/tema/library/books/", 27);
  url[27] = '\0';
  url = strcat(url, id);

  // formez mesajul pe care il trimit catre server
  char* msg_to_server = compute_get_request("34.118.48.238:8080",
    url, NULL, cookie, token);
  // trimit mesajul catre server
  send_to_server(socket, msg_to_server);

  // retin mesajul romiti de la server
  char* msg_from_server = receive_from_server(socket);

  // extrag exit code-ul
  char* exit_code = malloc(10000 * sizeof(char));
  memcpy(exit_code, msg_from_server, strlen(msg_from_server));
  exit_code[strlen(msg_from_server)] = '\0';
  exit_code = strtok(exit_code, " ");
  exit_code = strtok(NULL, " ");

  if (atoi(exit_code) == 200) {
    char* book = malloc(1000 * sizeof(char));
    book = strstr(msg_from_server, "[");
    puts(book);

  } else if (atoi(exit_code) == 404 || atoi(exit_code) == 400) {
    printf("ERROR: The fields are not correct - bad request!\n");
  } else {
    printf("ERROR: You do not have access to the library!\n");
  }

  // eliberez memoria
  free(url);
  free(id);
  // inchid conexiunea
  close(socket);
}

void client_delete_book(char* cookie, char* token) {
  // creez conexiunea cu server-ul
  int socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
  // formez url-ul - am luat copy paste bucata de cod de la functia get_book
  char* id = malloc(100 * sizeof(char));
  printf("id=");
  fgets(id, 100, stdin);
  id = strtok(id, "\n");

  char* url = malloc(100 * sizeof(char));
  memcpy(url, "/api/v1/tema/library/books/", 27);
  url[27] = '\0';
  url = strcat(url, id);

  // formez mesajul pe care il trimit catre server
  char* msg_to_server = compute_delete_request("34.118.48.238:8080",
    url, cookie, token);

  // trimit mesajul catre server
  send_to_server(socket, msg_to_server);

  // primesc raspunsul de la server
  char* msg_from_server = receive_from_server(socket);

  // extrag exit code-ul
  char* exit_code = malloc(10000 * sizeof(char));
  memcpy(exit_code, msg_from_server, strlen(msg_from_server));
  exit_code[strlen(msg_from_server)] = '\0';
  exit_code = strtok(exit_code, " ");
  exit_code = strtok(NULL, " ");

  if (atoi(exit_code) == 200) {
    printf("OK\n");
  } else if (atoi(exit_code) == 403) {
    printf("ERROR: You are not logged in!\n");
  } else {
    printf("ERROR: Bad request!\n");
  }

  // inchid conexiunea
  close(socket);
  // eliberez memoria
  free(url);
  free(id);

}

void client_logout(char* cookie, char* token) {
  // creez conexiunea cu server-ul
  int socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);

  // formez mesajul pe care il trimit catre server
  char* msg_to_server = compute_get_request("34.118.48.238:8080",
    "/api/v1/tema/auth/logout", 0, cookie, token);

  // trimit mesajul catre server
  send_to_server(socket, msg_to_server);

  // primesc rasunsul de la server
  char* msg_from_server = receive_from_server(socket);

  // extrag exit code-ul
  char* exit_code = malloc(1000 * sizeof(char));
  memcpy(exit_code, msg_from_server, strlen(msg_from_server));
  exit_code[strlen(msg_from_server)] = '\0';
  exit_code = strtok(exit_code, " ");
  exit_code = strtok(NULL, " ");
  puts(msg_from_server);
  // afisez un mesaj la consola in functie de valoarea exit code-ului
  if (atoi(exit_code) == 200) {
    printf("OK\n");
  } else if (atoi(exit_code) == 404 || atoi(exit_code) == 400) {
    printf("ERROR: You are not logged in!\n");
  } else {
    printf("ERROR: Bad request!\n");
  }

  // inchid conexiunea
  close(socket);
}


int main(int argc, char const *argv[]) {
  // in aceasta variabila voi retine comanda primita de la tastatura
  char* command = malloc(20 * sizeof(char));
  // in aceasta variabila retin cookie-ul pentru comanda login
  char* cookie = NULL; // o initializez cu NULL
  // in aceasta variabila retin token-ul primit de la enter_library
  char* token = NULL; // o initializez cu NULL

  while (1) {
    memset(command, 0, 20);
    // retin comanda primita de la tastatura
    fgets(command, 20, stdin);
    // daca primesc mesajul exit
    if (strcmp(command, "exit\n") == 0) {
      // ies din bucla
      break;
    } else if (strcmp(command, "register\n") == 0) {
      // apelez functia client_register()
      client_register();
    } else if (strcmp(command, "login\n") == 0) {
      // apelez functia client_login()
      cookie = client_login();
    } else if (strcmp(command, "enter_library\n") == 0) {
      // apelez functia client_enter_library()
      // primesc in variabila token accesul la biblioteca
      token = client_enter_library(cookie);
    } else if (strcmp(command, "get_books\n") == 0) {
      // apelez functia client_get_books()
      client_get_books(cookie, token);
    } else if (strcmp(command, "add_book\n") == 0) {
      // apelez functia client_add_book()
      client_add_book(cookie, token);
    } else if (strcmp(command, "get_book\n") == 0) {
      // apelez functia client_get_book()
      client_get_book(cookie, token);
    } else if (strcmp(command, "logout\n") == 0) {
      // apelez functia client_logout()
      client_logout(cookie, token);
      // daca ma deloghez, campurile token si cookie vor fi setate la NULL
      cookie = NULL;
      token = NULL;
    } else if (strcmp(command, "delete_book\n") == 0) {
      // apelez functia client_delete_book
      client_delete_book(cookie, token);
    }
  }

  free(command);

  return 0;
}
