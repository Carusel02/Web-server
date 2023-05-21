#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <stdbool.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

/* info about server*/
#define IP_SERVER "34.254.242.81"
#define PORT_SERVER 8080

/* info about state of user */
typedef enum {
    /* no cookie :( */
    NOT_LOGGED_IN = 0,
    /* with cookie, but no token :( */
    LOGGED_IN = 1,
    /* with cookie and token :) */
    ENTERED_LIBRARY = 2
} state;

/* info about notification command */
typedef enum {
    
    REGISTER = 3,
    
    LOGIN = 4,

    TOKEN = 5,

    BOOKS = 6,

    BOOK = 7,
    
    LOGOUT = 8

} notification;

/* extract cookie */
void extract_cookie(char *response, char *cookie) {
    char *line;
    line = strtok(response, "\n");
    
    while (line != NULL) {
        if(strncmp(line, "Set-Cookie", 10) == 0) {
            strcpy(cookie, line + 12);
            break;
        }
        line = strtok(NULL, "\n");
    }

    const char* semicolon = strchr(cookie, ';');
    if(semicolon == NULL) {
        perror("eroare extract cookie");
        exit(EXIT_FAILURE);
    }
    cookie[semicolon - cookie] = '\0';

}

/* extract token */
void extract_token(char *response, char *value_token) {
    char *token = strchr(response, '{');
    JSON_Value *jwt = json_parse_string(token);
    strcpy(value_token,(char *)json_object_get_string(json_object(jwt), "token"));
    json_value_free(jwt);
}

void create_user(char **json) {
    
    /* alloc username & password */
    printf("username=");
    char *username = calloc(256, sizeof(char));
    char *password = calloc(256, sizeof(char));
    /* show prompts & read from input */
    scanf("%s", username);
    printf("password=");
    scanf("%s", password);

    /* verification */
    if(username == NULL || password == NULL) {
        perror("eroare create user");
        exit(EXIT_FAILURE);
    }

    /* create json */
    JSON_Value *root_value = json_value_init_object();
    json_object_set_string(json_object(root_value), "username", username);
    json_object_set_string(json_object(root_value), "password", password);
    *json = json_serialize_to_string_pretty(root_value);

    /* free */
    free(username);
    free(password);
    json_value_free(root_value);
}

void create_book(char **json) {

    /* alloc title, author, genre, publisher, page_count */
    char *title = calloc(128, sizeof(char));
    char *author = calloc(128, sizeof(char));
    char *genre = calloc(128, sizeof(char));
    char *publisher = calloc(128, sizeof(char));
    int page_count;
    /* show prompts & read from input */
    fflush(stdin);

    getchar(); // Consume newline character
    printf("title=");
    fgets(title, 128, stdin);
    title[strlen(title) - 1] = '\0';

    printf("author=");
    fgets(author, 128, stdin);
    author[strlen(author) - 1] = '\0';

    printf("genre=");
    fgets(genre, 128, stdin);
    genre[strlen(genre) - 1] = '\0';

    printf("publisher=");
    fgets(publisher, 128, stdin);
    publisher[strlen(publisher) - 1] = '\0';
    
    /* verification */
    printf("page_count=");
    int check = scanf("%d", &page_count);
    if(check == -1) {
        printf("numar pagini invalid!\n");
        free(title);
        free(author);
        free(genre);
        free(publisher);
        *json = NULL;
        return;
    }

    if(strlen(title) == 0 || strlen(author) == 0 || strlen(genre) == 0 || strlen(publisher) == 0) {
        printf("Nu s-a putut adauga cartea!\n");
        free(title);
        free(author);
        free(genre);
        free(publisher);
        *json = NULL;
        return;
    }

    /* verification */
    // printf("\n");
    // printf("title is %s\n", title);
    // printf("author is %s\n", author);
    // printf("genre is %s\n", genre);
    // printf("publisher is %s\n", publisher);
    // printf("page_count is %d\n", page_count);

    /* create json */
    JSON_Value *root_value = json_value_init_object();
    json_object_set_string(json_object(root_value), "title", title);
    json_object_set_string(json_object(root_value), "author", author);
    json_object_set_string(json_object(root_value), "genre", genre);
    json_object_set_string(json_object(root_value), "publisher", publisher);
    json_object_set_number(json_object(root_value), "page_count", page_count);
    *json = json_serialize_to_string_pretty(root_value);

    /* free */
    free(title);
    free(author);
    free(genre);
    free(publisher);
    json_value_free(root_value);

    printf("Carte adaugata cu succes!\n");

}

int notification_client(char *response, notification not) {
    
    char *payload;
    JSON_Value *root_value;

    /* search for DATA in response */
    if(not == BOOKS) {
        payload = strchr(response, '[');
        root_value = json_parse_string(payload);
    } else {
        payload = strchr(response, '{');
        root_value = json_parse_string(payload);
    }

    switch (not) {
    /* case register notification */
    case REGISTER:
        if(json_object_get_value(json_object(root_value), "error") != NULL) {
            printf("Nu e ok: %s\n", json_object_get_string(json_object(root_value), "error"));
        } else {
            printf("200 - OK - Utilizator înregistrat cu succes!\n");
        }
        break;
    
    /* case login notification */
    case LOGIN:
        if(json_object_get_value(json_object(root_value), "error") != NULL) {
            printf("error: %s\n", json_object_get_string(json_object(root_value), "error"));
            json_value_free(root_value);
            return -1;
        } else {
            printf("200 - OK - Utilizator autentificat cu succes!\n");
        }
        break;

    /* case token notification */
    case TOKEN:
        if(json_object_get_value(json_object(root_value), "error") != NULL) {
            printf("error: %s\n", json_object_get_string(json_object(root_value), "error"));
            json_value_free(root_value);
            return -1;
        } else {
            printf("200 - OK - Acces permis!\n");
        }
        break;

    /* case books notification */
    case BOOKS:
        if(json_object_get_value(json_object(root_value), "error") != NULL) {
            printf("error: %s\n", json_object_get_string(json_object(root_value), "error"));
            json_value_free(root_value);
            return -1;
        } else {
            printf("200 - OK - Carti disponibile:\n");
            JSON_Array *books = json_value_get_array(root_value);
            int nr_books = (int)json_array_get_count(books);
            
            if(nr_books == 0) {
                printf("Nu exista carti disponibile!\n");
            } else {
                for(int i = 0; i < nr_books; i++) {
                    JSON_Object *book = json_array_get_object(books, i);
                    printf("id=%d\n", (int)json_object_get_number(book, "id"));
                    printf("title=%s\n", json_object_get_string(book, "title"));
                    printf("\n");
                }
            }
            json_array_clear(books);
        }
        break;

    /* case book notification */
    case BOOK:
        if(json_object_get_value(json_object(root_value), "error") != NULL) {
            printf("error: %s\n", json_object_get_string(json_object(root_value), "error"));
        } else {
            printf("200 - OK - Carte disponibila:\n");
            JSON_Object *book = json_object(root_value);
            printf("id=%d\n", (int)json_object_get_number(book, "id"));
            printf("title=%s\n", json_object_get_string(book, "title"));
            printf("author=%s\n", json_object_get_string(book, "author"));
            printf("genre=%s\n", json_object_get_string(book, "genre"));
            printf("publisher=%s\n", json_object_get_string(book, "publisher"));
            printf("page_count=%d\n", (int)json_object_get_number(book, "page_count"));
            printf("\n");
            json_object_clear(book);
        }
        break;

    default:
        break;
    }
    
    /* free */
    json_value_free(root_value);
    
    /* return 0 if OK */
    return 0;

}

int main(int argc, char *argv[]) {
    
    /*---------------------- DECLARE BASIC INFO ----------------------*/
    state user_state = NOT_LOGGED_IN;
    char *message = NULL;
    char *response = NULL;
    char *cookie = NULL;
    char *value_token = NULL;
    int sockfd;
    char *command = calloc(256, sizeof(char));
    /*----------------------------------------------------------------*/

    
    while(true) {
        
        /*---------------------- READ FROM INPUT ----------------------*/
        memset(command, 0, 256);
        scanf("%s", command);
        /*-------------------------------------------------------------*/
        
        
        if(strcmp(command, "register") == 0) {
                   
            /*---------------------- REGISTER -----------------------------*/

            /* open connection */
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
            
            /* compute message */
            char *content_type = "application/json";
            char *url = "/api/v1/tema/auth/register";
            /* create JSON payload */
            char *json_user_register;
            create_user(&json_user_register);
            /* verification */
            if(json_user_register == NULL) {
                perror("eroare creare JSON payload register");
                exit(EXIT_FAILURE);
            }
            message = compute_post_request(IP_SERVER, url, content_type, &json_user_register, 1, NULL, 0, NULL);
            
            /* send message */
            send_to_server(sockfd, message);
            // printf("Message:\n%s", message);
            
            /* receive response*/
            response = receive_from_server(sockfd);
            // printf("Response:\n%s", response);

            /* notification */
            notification_client(response, REGISTER);

            /* free */
            free(json_user_register);
            free(message);
            free(response);

            /* close connection */
            close(sockfd);

            /*---------------------- END REGISTER -------------------------*/
        }

        if(strcmp(command, "login") == 0) {
            
            /*---------------------- LOGIN --------------------------------*/

            /* check if we are logged in already */
            if(user_state != NOT_LOGGED_IN) {
                printf("You are already logged in!\n");
                continue;
            }
            
            /* open connection */
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

            /* compute message */
            char *content_type = "application/json";
            char *url = "/api/v1/tema/auth/login";
            /* create JSON payload */
            char *json_user_login;
            create_user(&json_user_login);
            if(json_user_login == NULL) {
                perror("eroare creare JSON payload login");
                exit(EXIT_FAILURE);
            }
            message = compute_post_request(IP_SERVER, url, content_type, &json_user_login, 1, NULL, 0, NULL);
            
            /* send message */
            send_to_server(sockfd, message);
            // printf("Message:\n%s", message);
            
            /* receive response */
            response = receive_from_server(sockfd);
            // printf("Response:\n%s", response);

            if(notification_client(response, LOGIN) == 0) {
                /* change state */
                user_state = LOGGED_IN;
                /* extract cookie */
                cookie = calloc(256, sizeof(char));
                extract_cookie(response, cookie);
            }
            
            /* free */
            free(json_user_login);
            free(message);
            free(response);

            /* close connection */
            close(sockfd);

            /*---------------------- END LOGIN ----------------------------*/
        }
        
        if(strcmp(command, "enter_library") == 0) {
            
            /*---------------------- ENTER LIBRARY ------------------------*/

            /* check if we are logged in */
            if(user_state == NOT_LOGGED_IN) {
                printf("You must login first!\n");
                continue;
            }

            /* check if we have already a token */
            if(user_state == ENTERED_LIBRARY) {
                printf("You have already access in library!\n");
                continue;
            }
            
            /* open connection */
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
            
            /* compute message */
            char *url = "/api/v1/tema/library/access";
            message = compute_get_request(IP_SERVER, url, NULL, NULL, &cookie, 1);
            
            /* send message */
            send_to_server(sockfd, message);
            // printf("Message:\n%s", message);
            
            /* receive response */
            response = receive_from_server(sockfd);
            // printf("Response:\n%s", response);

            if(notification_client(response, TOKEN) == -1) {
                perror("eroare bad token");
                exit(EXIT_FAILURE);
            }
            
            /* extract token */
            value_token = calloc(512, sizeof(char));
            extract_token(response, value_token);
            
            /* change state */
            user_state = ENTERED_LIBRARY;

            /* free */
            free(message);
            free(response);

            close(sockfd);
            /*---------------------- END ENTER LIBRARY --------------------*/
        }

        if(strcmp(command, "get_books") == 0) {
            
            /*---------------------- GET BOOKS ----------------------------*/

            if(user_state != ENTERED_LIBRARY) {
                printf("You must have access first!\n");
                continue;
            }
            
            /* open connection */
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

            /* compute message */
            char *url = "/api/v1/tema/library/books";
            message = compute_get_request(IP_SERVER, url, NULL, value_token, NULL, 0);
            
            /* send message */
            send_to_server(sockfd, message);
            // printf("Message:\n%s", message);
            
            /* receive response */
            response = receive_from_server(sockfd);
            // printf("Response:\n%s", response);

            notification_client(response, BOOKS);

            /* free */
            free(message);
            free(response);
            
            /* close connection */
            close(sockfd);

            /*---------------------- GET BOOKS ----------------------------*/
        }

        if(strcmp(command, "get_book") == 0) {
            
            /*---------------------- GET BOOK -----------------------------*/
            
            if(user_state != ENTERED_LIBRARY) {
                printf("You must have access first!\n");
                continue;
            }

            /* read id from input */
            char *id = calloc(256, sizeof(char));
            printf("id=");
            scanf("%s", id);

            /* open connection */
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

            /* compute message */
            char url[256] = "/api/v1/tema/library/books/";
            strcat(url, id);
            message = compute_get_request(IP_SERVER, url, NULL, value_token, NULL, 0);
            
            /* send message */
            send_to_server(sockfd, message);
            // printf("Message:\n%s", message);
            
            /* receive response */
            response = receive_from_server(sockfd);
            // printf("Response:\n%s", response);

            notification_client(response, BOOK);
            
            /* free */
            free(id);
            free(message);
            free(response);
            
            /* close connection */
            close(sockfd);
            /*---------------------- END GET BOOK ------------------------*/
        }
        
        if(strcmp(command, "add_book") == 0) {
            
            /*---------------------- ADD BOOK ----------------------------*/

            if(user_state != ENTERED_LIBRARY) {
                printf("You must have access first!\n");
                continue;
            }

            /* create JSON payload */
            char *json_book;
            create_book(&json_book);
            if (json_book == NULL) {
                continue;
            }

            /* open connection */
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

            /* compute message */
            char *content_type = "application/json";
            char *url = "/api/v1/tema/library/books";
            message = compute_post_request(IP_SERVER, url, content_type, &json_book, 1, NULL, 0, value_token);

            /* send message */
            send_to_server(sockfd, message);
            // printf("Message:\n%s", message);
            
            /* receive response */
            response = receive_from_server(sockfd);
            // printf("Response:\n%s", response);

            /* free */
            free(json_book);
            free(message);
            free(response);

            /* close connection */
            close(sockfd);

            /*---------------------- END ADD BOOK -----------------------*/
        }

        if(strcmp(command, "delete_book") == 0) {
            
            /*---------------------- DELETE BOOK -------------------------*/
            
            if(user_state != ENTERED_LIBRARY) {
                printf("You must have access first!\n");
                continue;
            }
            
            /* read id from input */
            char *id = calloc(256, sizeof(char));
            printf("id=");
            scanf("%s", id);

            /* open connection */
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

            /* compute message */
            char url[256] = "/api/v1/tema/library/books/";
            strcat(url, id);
            message = compute_delete_request(IP_SERVER, url, NULL, value_token);
            
            /* send message */
            send_to_server(sockfd, message);
            // printf("Message:\n%s", message);
            
            /* receive response */
            response = receive_from_server(sockfd);
            // printf("Response:\n%s", response);

            /* free */
            free(id);
            free(message);
            free(response);

            /* close connection */
            close(sockfd);
            /*---------------------- END DELETE BOOK ---------------------*/
        }        
        
        if(strcmp(command, "logout") == 0) {
                
            /*---------------------- LOGOUT ------------------------------*/
            
            /* change state */
            user_state = NOT_LOGGED_IN;

            /* open connection */
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

            /* compute message */
            char *url = "/api/v1/tema/auth/logout";
            message = compute_get_request(IP_SERVER, url, NULL, NULL, &cookie, 1);
            
            /* send message */
            send_to_server(sockfd, message);
            // printf("Message:\n%s", message);
            
            /* receive response */
            response = receive_from_server(sockfd);
            // printf("Response:\n%s", response);

            /* clear token */
            free(value_token);
            value_token = NULL;
            /* clear cookie */
            free(cookie);
            cookie = NULL;

            /* free */
            free(message);
            free(response);

            /* close connection */
            close(sockfd);

            printf("Logout cu succes!\n");
            
            /*---------------------- LOGOUT ------------------------------*/
        }
        
        if(strcmp(command, "exit") == 0) {
            break;
        }

    }

    /* free command */
    free(command);

    return 0;
}
