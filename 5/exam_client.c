#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    
    //Creating socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    //Configuring server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    
    //Connecting to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    
    printf("Connected to exam server successfully!\n\n");
    //Get your auuutthhhh
    char username[100];
    while (1) {
        printf("Enter your username (student ID): ");
        if (fgets(username, sizeof(username), stdin) == NULL) {
            perror("Error reading username");
            close(client_socket);
            exit(EXIT_FAILURE);
        }
        
        if (username[strlen(username) - 1] == '\n') {
            username[strlen(username) - 1] = '\0';
        }
        
        send(client_socket, username, strlen(username), 0);
        
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received <= 0) {
            printf("Server disconnected while authenticating man!\n");
            close(client_socket);
            exit(EXIT_FAILURE);
        }
        
        buffer[bytes_received] = '\0';
        
        char *newline = strchr(buffer, '\n');
        if (newline) *newline = '\0';
        
        if (strcmp(buffer, "AUTH_SUCCESS") == 0) {
            printf("Authentication successful!\n");
            
            // Receive welcome message
            memset(buffer, 0, BUFFER_SIZE);
            recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
            printf("%s", buffer);
            break;
        } else {
            printf("Authentication failed. Please try again with a different username.\n");
        }
    }
    
    // Exam phase
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received <= 0) {
            printf("Server disconnected\n");
            break;
        }
        
        buffer[bytes_received] = '\0';
        
        // Check for exam end message
        if (strstr(buffer, "EXAM_END") != NULL) {
            char *end_msg = strstr(buffer, "Thank you");
            if (end_msg) {
                printf("\n%s", end_msg);
            }
            break;
        }
        
        if (strstr(buffer, "QUESTION") != NULL) {
            char *question = strchr(buffer, ':');
            if (question) {
                printf("%s\n", question + 1);
            }
            
            printf("Your answer (A/B/C/D): ");
            fflush(stdout);
            
            char answer[10];
            if (fgets(answer, sizeof(answer), stdin) == NULL) {
                perror("Error reading answer");
                break;
            }
            
            send(client_socket, answer, strlen(answer), 0);
            
            memset(buffer, 0, BUFFER_SIZE);
            recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
            printf("%s", buffer);
            
            memset(buffer, 0, BUFFER_SIZE);
            recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
            printf("%s\n", buffer);
        } 
        //Handles notifications of Students/Users
        else if (strstr(buffer, "Student") != NULL || strstr(buffer, "Active users") != NULL) {
            printf("%s", buffer);
        }
    }
    
    printf("Disconnected from server.\n");
    close(client_socket);
    return 0;
}