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

void extract_token(char *response, char *value_token) {
    /* extract token */
    char *token = strchr(response, '{');
    printf("\ntoken is %s\n", token);

    JSON_Value *jwt = json_parse_string(token);
    strcpy(value_token,(char *)json_object_get_string(json_object(jwt), "token"));
    json_value_free(jwt);
    printf("\nvalue is %s\n", value_token);

}

void create_user(char **json) {
    
    /* alloc username & password */
    char *username = calloc(256, sizeof(char));
    char *password = calloc(256, sizeof(char));
    /* show prompts & read from input */
    printf("register\n");
    printf("username=");
    scanf("%s", username);
    printf("password=");
    scanf("%s", password);

    /* verification */
    printf("\n");
    printf("username is %s\n", username);
    printf("password is %s\n", password);

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
    char *title = calloc(256, sizeof(char));
    char *author = calloc(256, sizeof(char));
    char *genre = calloc(256, sizeof(char));
    char *publisher = calloc(256, sizeof(char));
    int page_count;
    /* show prompts & read from input */
    printf("title=");
    scanf("%s", title);
    printf("author=");
    scanf("%s", author);
    printf("genre=");
    scanf("%s", genre);
    printf("publisher=");
    scanf("%s", publisher);
    printf("page_count=");
    scanf("%d", &page_count);

    /* verification */
    printf("\n");
    printf("title is %s\n", title);
    printf("author is %s\n", author);
    printf("genre is %s\n", genre);
    printf("publisher is %s\n", publisher);
    printf("page_count is %d\n", page_count);

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

}


int main(int argc, char *argv[])
{
    char *message = NULL;
    char *response = NULL;
    char *cookie = NULL;
    char *value_token = NULL;

    int sockfd;

    /* read from input */
    char *command = calloc(256, sizeof(char));
    while(true) {
        /* read from stdin */
        memset(command, 0, 256);
        scanf("%s", command);
        
        
        if(strcmp(command, "register") == 0) {
                   
            printf("\n-------- START REGISTER --------\n");

            /* create user & read from input */
            char *json;
            create_user(&json);
            printf("json is %s\n", json);

            /* open connection */
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
            
            /* compute message */
            char *content_type = "application/json";
            char *url = "/api/v1/tema/auth/register";
            message = compute_post_request(IP_SERVER, url, content_type, &json, 1, NULL, 0, NULL);
            /* send message */
            send_to_server(sockfd, message);
            printf("Message:\n%s", message);
            /* receive response*/
            response = receive_from_server(sockfd);
            printf("Response:\n%s", response);

            /* free */
            free(json);
            free(message);
            free(response);

            close(sockfd);
            printf("\n-------- END REGISTER --------\n");
        }


        if(strcmp(command, "login") == 0) {
            
            printf("\n-------- START LOGIN --------\n");

            /* login user & read from input */
            char *json;
            create_user(&json);
            printf("json is %s\n", json);


            /* open connection */
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

            /* compute message */
            char *content_type = "application/json";
            char *url = "/api/v1/tema/auth/login";
            message = compute_post_request(IP_SERVER, url, content_type, &json, 1, NULL, 0, NULL);
            /* send message */
            send_to_server(sockfd, message);
            printf("Message:\n%s", message);
            /* receive response */
            response = receive_from_server(sockfd);
            printf("Response:\n%s", response);
            
            /* extract cookie */
            cookie = calloc(256, sizeof(char));
            extract_cookie(response, cookie);

            /* free */
            free(json);
            free(message);
            free(response);

            close(sockfd);
            printf("\n-------- END LOGIN --------\n");
        }
        
        if(strcmp(command, "enter_library") == 0) {
            
            printf("\n-------- START ENTER LIBRARY --------\n");

            if(cookie == NULL) {
                printf("You must login first!\n");
                continue;
            }
            
            /* open connection */
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
            
            /* compute message */
            char *url = "/api/v1/tema/library/access";
            message = compute_get_request(IP_SERVER, url, NULL, NULL, &cookie, 1);
            /* send message */
            send_to_server(sockfd, message);
            printf("Message:\n%s", message);
            /* receive response */
            response = receive_from_server(sockfd);
            printf("Response:\n%s", response);
            
            /* extract token */
            value_token = calloc(512, sizeof(char));
            extract_token(response, value_token);
            printf("\nvalue in MAIN is %s\n", value_token);

            /* free */
            free(message);
            free(response);

            close(sockfd);
            printf("\n-------- END ENTER LIBRARY --------\n");
        }

        if(strcmp(command, "get_books") == 0) {
            
            printf("\n-------- START GET BOOKS --------\n");
            
            /* open connection */
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

            /* compute message */
            char *url = "/api/v1/tema/library/books";
            message = compute_get_request(IP_SERVER, url, NULL, value_token, NULL, 0);
            /* send message */
            send_to_server(sockfd, message);
            printf("Message:\n%s", message);
            /* receive response */
            response = receive_from_server(sockfd);
            printf("Response:\n%s", response);

            /* free */
            free(message);
            free(response);

            close(sockfd);
            printf("\n-------- END GET BOOKS --------\n");
        }

        if(strcmp(command, "get_book") == 0) {
            
            printf("\n-------- START GET BOOK --------\n");
            
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
            printf("Message:\n%s", message);
            /* receive response */
            response = receive_from_server(sockfd);
            printf("Response:\n%s", response);

            /* free */
            free(id);
            free(message);
            free(response);

            close(sockfd);
            printf("\n-------- END GET BOOK --------\n");
        }
        
        if(strcmp(command, "add_book") == 0) {
            
            printf("\n-------- START ADD BOOK --------\n");

            /* create book & read from input */
            char *json;
            create_book(&json);
            printf("json is %s\n", json);

            /* open connection */
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

            /* compute message */
            char *content_type = "application/json";
            char *url = "/api/v1/tema/library/books";
            message = compute_post_request(IP_SERVER, url, content_type, &json, 1, NULL, 0, value_token);

            /* send message */
            send_to_server(sockfd, message);
            printf("Message:\n%s", message);
            /* receive response */
            response = receive_from_server(sockfd);
            printf("Response:\n%s", response);

            /* free */
            free(json);
            free(message);
            free(response);

            close(sockfd);

            printf("\n-------- END ADD BOOK --------\n");
        }

        if(strcmp(command, "delete_book") == 0) {
            
            printf("\n-------- START DELETE BOOK --------\n");
            
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
            printf("Message:\n%s", message);
            /* receive response */
            response = receive_from_server(sockfd);
            printf("Response:\n%s", response);

            /* free */
            free(id);
            free(message);
            free(response);

            close(sockfd);
            printf("\n-------- END DELETE BOOK --------\n");
        }        
        
        if(strcmp(command, "logout") == 0) {
                
            printf("\n-------- START LOGOUT --------\n");
            
            /* open connection */
            sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);

            /* compute message */
            char *url = "/api/v1/tema/auth/logout";
            message = compute_get_request(IP_SERVER, url, NULL, NULL, &cookie, 1);
            /* send message */
            send_to_server(sockfd, message);
            printf("Message:\n%s", message);
            /* receive response */
            response = receive_from_server(sockfd);
            printf("Response:\n%s", response);

            /* clear token */
            free(value_token);
            value_token = NULL;
            /* clear cookie */
            free(cookie);
            cookie = NULL;

            /* free */
            free(message);
            free(response);

            close(sockfd);
            printf("\n-------- END LOGOUT --------\n");
        }
        
        if(strcmp(command, "exit") == 0) {
            break;
        }


    }

    // free the allocated data at the end!
    free(command);

    return 0;
}
