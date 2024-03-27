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
void register_function (int socket) {

    printf("username=");
    char* username = calloc(100, sizeof(char));
    fgets(username, 100, stdin);
    username[strlen(username) - 1] = '\0';
            
    printf("password=");
    char* password = calloc(100, sizeof(char));
    fgets(password, 100, stdin);
    password[strlen(password) - 1] = '\0';

    if (strchr(username, ' ') != NULL || strchr(password, ' ') != NULL) {
        printf("Username and password must not contain spaces\n");
        free(username);
        free(password);
        return;
    }

    JSON_Value *json_value;
    JSON_Object *json_object;

    json_value = json_value_init_object();
    json_object = json_value_get_object(json_value);

    json_object_set_string(json_object, "username", username);
    free(username);
    json_object_set_string(json_object, "password", password);
    free(password);

    char* json_string = json_serialize_to_string(json_value);
    char* post_request = compute_post_request("34.254.242.81:8080",
            "/api/v1/tema/auth/register", "application/json", &json_string, 1, NULL, NULL);
    send_to_server(socket, post_request);
    char* receive_message = receive_from_server(socket);

    char* exit_code = calloc(10000, sizeof(char));
    exit_code = strchr(receive_message, ' ') + 1;
    exit_code[3] = '\0';

    int exit_code_value = atoi(exit_code);
    if (exit_code_value == 200) {
        printf("200 - OK\n");
    } else {
        if (exit_code_value == 201) {
            printf("201 - Created\n");
        } else {
            printf("ERROR - Username already used\n");
        }
    }

    json_free_serialized_string(json_string);
    json_value_free(json_value);
}
void login_function (int socket, char** cookie) {

    printf("username=");
    char* username = calloc(100, sizeof(char));
    fgets(username, 100, stdin);
    username[strlen(username) - 1] = '\0';

    printf("password=");
    char* password = calloc(100, sizeof(char));
    fgets(password, 100, stdin);
    password[strlen(password) - 1] = '\0';

    if (strchr(username, ' ') != NULL || strchr(password, ' ') != NULL) {
        printf("Username and password must not contain spaces\n");
        free(username);
        free(password);
        return;
    }

    JSON_Value *json_value;
    JSON_Object *json_object;

    json_value = json_value_init_object();
    json_object = json_value_get_object(json_value);
    
    json_object_set_string(json_object, "username", username);
    free(username);
    
    json_object_set_string(json_object, "password", password);
    free(password);

    char* json_string = json_serialize_to_string(json_value);
    char* post_request = compute_post_request("34.254.242.81:8080",
            "/api/v1/tema/auth/login", "application/json", &json_string, 1, NULL, NULL);
            
    send_to_server(socket, post_request);
    char* receive_message = receive_from_server(socket);

    char* exit_code = calloc(10000, sizeof(char));
    char* exit_code_aux = calloc(10000, sizeof(char));
    exit_code_aux = strchr(receive_message, ' ') + 1;
    strcpy(exit_code, exit_code_aux);
    exit_code[3] = '\0';
    int exit_code_value = atoi(exit_code);
    if (exit_code_value == 200) {
        (*cookie) = strstr(receive_message, "Set-Cookie: ") + 12;
        char* end = strchr((*cookie), ';');
        int length = end - (*cookie);
        (*cookie)[length] = '\0';
        printf("Cookie: %s\n200 - OK\n", (*cookie));
    } else {
        printf("ERROR - Credentials don't match\n");
    }

    json_free_serialized_string(json_string);
    json_value_free(json_value);
}
void enter_library_function (int socket, char** cookie, char** token) {
    char* get_request = compute_get_request("34.254.242.81:8080",
            "/api/v1/tema/library/access", NULL, (*cookie), NULL);
    send_to_server(socket, get_request);
    char* receive_message = receive_from_server(socket);
    char* exit_code = calloc(10000, sizeof(char));
    char* exit_code_aux = calloc(10000, sizeof(char));
    exit_code_aux = strchr(receive_message, ' ') + 1;
    strcpy(exit_code, exit_code_aux);
    exit_code[3] = '\0';
    int exit_code_value = atoi(exit_code);
    if (exit_code_value == 200) {
        (*token) = strstr(receive_message, "{\"token\":\"") + 10;
        char* end = strstr((*token), "\"}");
        int length = end - (*token);
        (*token)[length] = '\0';
        printf("Token: %s\n200 - OK\n", (*token));
    } else {
        printf("ERROR - You shall not pass! (Access forbiden)\n");
    }
}
void get_books_function (int socket, char** cookie, char** token, char** books) {
    char* get_request = compute_get_request("34.254.242.81:8080",
            "/api/v1/tema/library/books", NULL, (*cookie), (*token));
            
    send_to_server(socket, get_request);
    char* receive_message = receive_from_server(socket);
    char* exit_code = calloc(10000, sizeof(char));
    char* exit_code_aux = calloc(10000, sizeof(char));
    exit_code_aux = strchr(receive_message, ' ') + 1;
    strcpy(exit_code, exit_code_aux);
    exit_code[3] = '\0';
    int exit_code_value = atoi(exit_code);
    if (exit_code_value == 200) {
        (*books) = strstr(receive_message, "[");
        char* end = strstr((*books), "]");
        int length = end - (*books);
        (*books)[length + 1] = '\0';
        printf("%s\n200 - OK\n", (*books));
    } else {
        printf("ERROR - You shall not pass! (Access forbiden)\n");
    }
}
void get_book_function (int socket, char** cookie, char** token) {
    char* url = calloc(100, sizeof(char));
    strcpy(url, "/api/v1/tema/library/books/");

    printf("id=");
    char* id = calloc(100, sizeof(char));
    fgets(id, 100, stdin);
    id[strlen(id) - 1] = '\0';
    strcat(url, id);
    
    char* get_request = compute_get_request("34.254.242.81:8080",
            url, NULL, (*cookie), (*token));
    
    free(id);
    free(url);

    send_to_server(socket, get_request);
    char* receive_message = receive_from_server(socket);
    char* exit_code = calloc(10000, sizeof(char));
    char* exit_code_aux = calloc(10000, sizeof(char));
    exit_code_aux = strchr(receive_message, ' ') + 1;
    strcpy(exit_code, exit_code_aux);
    exit_code[3] = '\0';

    int exit_code_value = atoi(exit_code);
    
    if (exit_code_value == 200) {
        char* book = calloc(10000, sizeof(char));
        book = strstr(receive_message, "{");
        char* end = strstr(book, "}");
        int length = end - book;
        book[length + 1] = '\0';
        printf("%s\n200 - OK\n", book);
    } else {
        printf("ERROR - Wrong ID\n");
    }
}
void add_book_function (int socket, char** cookie, char** token) {
    JSON_Value *json_value;
    JSON_Object *json_object;

    json_value = json_value_init_object();
    json_object = json_value_get_object(json_value);

    printf("title=");
    char* title = calloc(100, sizeof(char));
    fgets(title, 100, stdin);
    title[strlen(title) - 1] = '\0';
    json_object_set_string(json_object, "title", title);
    free(title);
    printf("author=");
    char* author = calloc(100, sizeof(char));
    fgets(author, 100, stdin);
    author[strlen(author) - 1] = '\0';
    json_object_set_string(json_object, "author", author);
    free(author);
    printf("genre=");
    char* genre = calloc(100, sizeof(char));
    fgets(genre, 100, stdin);
    genre[strlen(genre) - 1] = '\0';
    json_object_set_string(json_object, "genre", genre);
    free(genre);
    printf("publisher=");
    char* publisher = calloc(100, sizeof(char));
    fgets(publisher, 100, stdin);
    publisher[strlen(publisher) - 1] = '\0';
    json_object_set_string(json_object, "publisher", publisher);
    free(publisher);
    printf("page_count=");
    char* page_count = calloc(100, sizeof(char));
    fgets(page_count, 100, stdin);
    page_count[strlen(page_count) - 1] = '\0';
    json_object_set_string(json_object, "page_count", page_count);
    free(page_count);

    char* json_string = json_serialize_to_string(json_value);
    char* post_request = compute_post_request("34.254.242.81:8080",
            "/api/v1/tema/library/books", "application/json", &json_string, 1, (*cookie), (*token));
            
    send_to_server(socket, post_request);
    char* receive_message = receive_from_server(socket);

    char* exit_code = calloc(10000, sizeof(char));
    exit_code = strchr(receive_message, ' ') + 1;
    exit_code[3] = '\0';

    int exit_code_value = atoi(exit_code);
    if (exit_code_value == 200) {
        printf("200 - OK\n");
    } else {
        printf("ERROR - You shall not pass! (Access forbiden)\n");
    }

    json_free_serialized_string(json_string);
    json_value_free(json_value);
}
void delete_book_function (int socket, char** cookie, char** token) {
    char* url = calloc(100, sizeof(char));
    strcpy(url, "/api/v1/tema/library/books/");

    printf("id=");
    char* id = calloc(100, sizeof(char));
    fgets(id, 100, stdin);
    id[strlen(id) - 1] = '\0';
    strcat(url, id);
    
    char* delete_request = compute_delete_request("34.254.242.81:8080",
            url, (*cookie), (*token));
    free(id);
    free(url);
    send_to_server(socket, delete_request);
    char* exit_code = calloc(10000, sizeof(char));
    char* receive_message = receive_from_server(socket);
    exit_code = strchr(receive_message, ' ') + 1;
    exit_code[3] = '\0';

    int exit_code_value = atoi(exit_code);
    
    if (exit_code_value == 200) {
        printf("200 - OK\n");
    } else {
        printf("ERROR - You shall not pass! (Access forbiden)\n");
    }
}
void logout_function (int socket, char** cookie, char** token) {
    char* request = compute_get_request("34.254.242.81:8080",
            "/api/v1/tema/auth/logout", NULL, (*cookie), (*token));
    send_to_server(socket, request);
    char* receive_message = receive_from_server(socket);

    char* exit_code = calloc(10000, sizeof(char));
    exit_code = strchr(receive_message, ' ') + 1;
    exit_code[3] = '\0';

    int exit_code_value = atoi(exit_code);
    if (exit_code_value == 200) {
        printf("200 - OK\n");
    } else {
        printf("ERROR - Wrong command!\n");
    }
}
int main(int argc, char *argv[])
{
    char* command = calloc(20, sizeof(char));
    fgets(command, 20, stdin);
    char* cookie = NULL;
    char* token = NULL;
    char* books = NULL;

    int socket;    

    while (strcmp(command, "exit\n") != 0) {
        socket = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
        if (strcmp(command, "register\n") == 0) {
            register_function(socket);
            goto while_end;
        }
        if (strcmp(command, "login\n") == 0) {
            if (cookie == NULL) {
                login_function(socket, &cookie);
            } else {
                printf("You are already logged in\n");
            }
            goto while_end;
        }
        if (strcmp(command, "enter_library\n") == 0) {
            if (cookie == NULL) {
                printf("You are not logged in\n");
            } else {
                if (token == NULL) {
                    enter_library_function(socket, &cookie, &token);
                } else {
                    printf("You are already in the library\n");
                }
                
            }
            goto while_end;
        }
        if (strcmp(command, "get_books\n") == 0) {
            if (token == NULL) {
                printf("You didn't enter the library\n");
            } else {
                get_books_function(socket, &cookie, &token, &books);
            }
            goto while_end;
        }
        if (strcmp(command, "get_book\n") == 0) {
            if (token == NULL) {
                printf("You didn't enter the library\n");
            } else {
                get_book_function(socket, &cookie, &token);
            }
            goto while_end;
        }
        if (strcmp(command, "add_book\n") == 0) {
            if (token == NULL) {
                printf("You didn't enter the library\n");
            } else {
                add_book_function(socket, &cookie, &token);
            }
            goto while_end;
        }
        if (strcmp(command, "delete_book\n") == 0) {
            if (token == NULL) {
                printf("You didn't enter the library\n");
            } else {
                delete_book_function(socket, &cookie, &token);
            }
            goto while_end;
        }
        if (strcmp(command, "logout\n") == 0) {
            if (cookie == NULL) {
                printf("You didn't login\n");
            } else {
                logout_function(socket, &cookie, &token);
                token = cookie = NULL;
            }
            goto while_end;
        }
        printf("WRONG COMMAND\n");
while_end:
        memset(command, 0, 20);
        fgets(command, 20, stdin);
        close(socket);
    }
    free(command);
    
    return 0;
}


