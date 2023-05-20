#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

/* info about server*/
#define IP_SERVER "34.254.242.81"
#define PORT_SERVER 8080
#define WEATHER_SERVER "api.openweathermap.org"

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


int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;

        
    // Ex 1.1: GET dummy from main server
    printf("\n-------- START TASK1 --------\n");
    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(IP_SERVER, "/api/v1/dummy", NULL, NULL, 0);
    send_to_server(sockfd, message);
    printf("Message:\n%s", message);
    response = receive_from_server(sockfd);
    printf("Response:\n%s", response);
    close_connection(sockfd);
    printf("\n-------- END TASK1 ----------\n");

    // Ex 2: Login into main server
    printf("\n-------- START TASK2 --------\n");

    /* open connection */
    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    /* set data type */
    char *form_data = "application/x-www-form-urlencoded";
    /* set username & password */
    char **data_buffer = malloc( 10 * sizeof(char *));
    for(int i = 0; i < 10 ; i++ )
        data_buffer[i] = malloc( 100 *sizeof(char));
    strcpy(data_buffer[0], "username=student&password=student");

    /* compute message */
    message = compute_post_request(IP_SERVER, "/api/v1/auth/login", form_data, data_buffer, 1, NULL, 0);
    /* send message */
    send_to_server(sockfd, message);
    printf("Message:\n%s", message);
    response = receive_from_server(sockfd);
    printf("Response:\n%s", response);

    /* extract cookie */
    char *cookie = malloc(200 * sizeof(char));
    extract_cookie(response, cookie);

    /* free & close */
    for(int i = 0 ; i < 10 ; i++ )
        free(data_buffer[i]);
    free(data_buffer);
    close_connection(sockfd);
    printf("\n-------- END TASK2 --------\n");

    // Ex 3: GET weather key from main server
    printf("\n-------- START TASK3 --------\n");
    /* open connection */
    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    /* set cookies */
    char **cookies = malloc( 5 * sizeof(char *));
    for(int i = 0 ; i < 5 ; i++ )
        cookies[i] = malloc( 200 *sizeof(char));
    strcpy(cookies[0], cookie);
    /* compute message */
    message = compute_get_request(IP_SERVER, "/api/v1/weather/key", NULL, cookies, 1);
    /* send message */
    send_to_server(sockfd, message);
    printf("Message:\n%s", message);
    response = receive_from_server(sockfd);
    printf("Response:\n%s", response);
    
    /* free & close */
    for(int i = 0 ; i < 5 ; i++)
        free(cookies[i]);
    free(cookies);
    close_connection(sockfd);
    printf("\n-------- END TASK3 ----------\n");

    // Ex 4: GET weather data from OpenWeather API
    printf("\n-------- START TASK4 --------\n");

    FILE *file;
    char *buffer = calloc(1024, sizeof(char));
    file = fopen("json.txt", "r");
    if(file == NULL) {
        perror("eroare deschidere fisier");
        exit(EXIT_FAILURE);
    }
    fgets(buffer, 1024, file);
    printf("Buffer:\n%s\n", buffer);
    fclose(file);

    /* json part */
    JSON_Value *root_value = json_parse_string(buffer);
    printf("Root value:\n%s\n", json_serialize_to_string_pretty(root_value));

    /* query parameters */
    char *query_params = calloc(200, sizeof(char));
    strcpy(query_params, "lat=44.43&lon=26.10&appid=b912dd495585fbf756dc6d8f415a7649");
    
    sockfd = open_connection("37.139.20.5", 80, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request("api.openweathermap.org", "/data/2.5/weather", query_params, NULL, 0);
    send_to_server(sockfd, message);
    printf("Message:\n%s", message);
    response = receive_from_server(sockfd);
    printf("Response:\n%s", response);

    response = strchr(response, '{');
    JSON_Value *root_value2 = json_parse_string(response);
    printf("Root value:\n%s\n", json_serialize_to_string_pretty(root_value2));
    close_connection(sockfd);




    printf("\n-------- END TASK4 ----------\n");
    // Ex 5: POST weather data for verification to main server
    // Ex 6: Logout from main server

    // BONUS: make the main server return "Already logged in!"

    // free the allocated data at the end!

    return 0;
}