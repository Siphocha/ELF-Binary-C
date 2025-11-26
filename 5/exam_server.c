#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 4
#define BUFFER_SIZE 1024
#define MAX_USERNAME_LEN 50
#define MAX_QUESTION_LEN 256

//Basic user "client" info struct
typedef struct {
    int socket;
    struct sockaddr_in address;
    int active;
    char username[MAX_USERNAME_LEN];
    int authenticated;
} client_t;

client_t clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

//Exam questions struct
typedef struct {
    char question[MAX_QUESTION_LEN];
    char correct_answer;
} exam_question_t;

exam_question_t exam_questions[] = {
    {"Capital of France?\nA) London\nB) Paris\nC) My underpants\nD) Fireants\n", 'B'},
    {"Data struct using LIFO principle\nA) Queue\nB) Stack\nC) Whack\nD) None\n", 'B'},
    {"CPU Definition\nA) Central Processing Unit\nB) Computer Personal Unit\nC) Central Processor Universe\nD) Computer Processing Unix\n", 'A'},
    {"Tmother of all programming languages?\nA) C\nB) CofeeScript\nC) PythonC\nD) Assembly\n", 'A'}
};

int total_questions = sizeof(exam_questions) / sizeof(exam_question_t);

//Broadcasting message to all clients on server function.
void broadcast_message(const char *message, int exclude_socket) {
    pthread_mutex_lock(&clients_mutex);
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].authenticated && clients[i].socket != exclude_socket) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }
    
    pthread_mutex_unlock(&clients_mutex);
}

//Send user list to specific client
void send_active_users(int client_socket) {
    char user_list[BUFFER_SIZE] = "Active users: ";
    pthread_mutex_lock(&clients_mutex);
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].authenticated) {
            strcat(user_list, clients[i].username);
            strcat(user_list, " ");
        }
    }
    strcat(user_list, "\n");
    
    pthread_mutex_unlock(&clients_mutex);
    send(client_socket, user_list, strlen(user_list), 0);
}


int authenticate_user(const char *username) {
    //Most basic authentication 
    if (strlen(username) < 1) return 0;
    
    //Check if username existance
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].authenticated && 
            strcmp(clients[i].username, username) == 0) {
            pthread_mutex_unlock(&clients_mutex);
            return 0;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    
    return 1; 
}

//Handling individual client connections
void *handle_client(void *arg) {
    int client_index = *(int *)arg;
    int client_socket = clients[client_index].socket;
    char buffer[BUFFER_SIZE];
    int questions_answered = 0;
    
    printf("Client connected from %s:%d\n", 
           inet_ntoa(clients[client_index].address.sin_addr),
           ntohs(clients[client_index].address.sin_port));
    
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received <= 0) {
            printf("Client disconnected!\n");
            clients[client_index].active = 0;
            close(client_socket);
            pthread_exit(NULL);
        }
        
        buffer[bytes_received] = '\0';
        
        char *newline = strchr(buffer, '\n');
        if (newline) *newline = '\0';
        
        if (authenticate_user(buffer)) {
            strcpy(clients[client_index].username, buffer);
            clients[client_index].authenticated = 1;
            
            send(client_socket, "AUTH_SUCCESS\n", 13, 0);
            
            char welcome_msg[BUFFER_SIZE];
            snprintf(welcome_msg, BUFFER_SIZE, 
                    "Welcome %s! You are authenticated\n", buffer);
            send(client_socket, welcome_msg, strlen(welcome_msg), 0);
            
            // Notify other users
            char notification[BUFFER_SIZE];
            snprintf(notification, BUFFER_SIZE, "Student %s has joined the exam.\n", buffer);
            broadcast_message(notification, client_socket);
            
            break;
        } else {
            send(client_socket, "AUTH_FAILED\n", 12, 0);
        }
    }
    
    //Exam phase, send and recieve
    for (int i = 0; i < total_questions; i++) {
        // Send question
        char question_msg[BUFFER_SIZE + 50];
        snprintf(question_msg, BUFFER_SIZE + 50, "QUESTION_%d:%s", 
                i + 1, exam_questions[i].question);
        send(client_socket, question_msg, strlen(question_msg), 0);
        
        //Receive answer
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received <= 0) {
            printf("Client %s disconnected during exam\n", clients[client_index].username);
            break;
        }
        
        buffer[bytes_received] = '\0';
        char *newline = strchr(buffer, '\n');
        if (newline) *newline = '\0';
        
        // Check answer
        char feedback[BUFFER_SIZE];
        if (buffer[0] == exam_questions[i].correct_answer) {
            snprintf(feedback, BUFFER_SIZE, "Server/Teacher: Correct!\n");
            questions_answered++;
        } else {
            snprintf(feedback, BUFFER_SIZE, "Server/Teacher: Incorrect! The correct answer was %c\n", 
                    exam_questions[i].correct_answer);
        }
        
        send(client_socket, feedback, strlen(feedback), 0);
        
        //Send active users list after each question
        send_active_users(client_socket);
        
        sleep(1); 
    }
    
    //Exam success
    char completion_msg[BUFFER_SIZE];
    snprintf(completion_msg, BUFFER_SIZE, 
            "EXAM_END:Exam session ended. Thank you, %s! You answered %d/%d questions correctly.\n",
            clients[client_index].username, questions_answered, total_questions);
    send(client_socket, completion_msg, strlen(completion_msg), 0);
    
    // Notify other users
    char leave_msg[BUFFER_SIZE];
    snprintf(leave_msg, BUFFER_SIZE, "Student %s has completed the exam.\n", 
            clients[client_index].username);
    broadcast_message(leave_msg, client_socket);
    
    printf("Student %s completed exam with %d/%d correct answers\n", 
           clients[client_index].username, questions_answered, total_questions);
    
    //Let's clean it all up
    close(client_socket);
    clients[client_index].active = 0;
    clients[client_index].authenticated = 0;
    
    pthread_exit(NULL);
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr;
    pthread_t threads[MAX_CLIENTS];
    int client_indices[MAX_CLIENTS];
    
    //Initialise client array
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].active = 0;
        clients[i].authenticated = 0;
        memset(clients[i].username, 0, MAX_USERNAME_LEN);
    }
    
    //Creating server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    //Configuring server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);
    
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    //Binding Socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    //Listen for connections
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    printf("Exam Server started on port 8080\n");
    printf("Waiting for connections...\n");
    
    while (1) {
        //Accept new connection
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }
        
        //Finding available slot for new client
        pthread_mutex_lock(&clients_mutex);
        int slot_found = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!clients[i].active) {
                clients[i].socket = client_socket;
                clients[i].address = client_addr;
                clients[i].active = 1;
                clients[i].authenticated = 0;
                memset(clients[i].username, 0, MAX_USERNAME_LEN);
                
                client_indices[i] = i;
                slot_found = 1;
                
                // Create thread for new client
                if (pthread_create(&threads[i], NULL, handle_client, &client_indices[i]) != 0) {
                    perror("Thread creation failed");
                    clients[i].active = 0;
                    close(client_socket);
                } else {
                    printf("Client slot %d allocated\n", i);
                }
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);
        
        if (!slot_found) {
            char *reject_msg = "Server: Max student limit reached. Come back later.\n";
            send(client_socket, reject_msg, strlen(reject_msg), 0);
            close(client_socket);
            printf("Student rejected\n");
        }
    }
    
    close(server_socket);
    pthread_mutex_destroy(&clients_mutex);
    return 0;
}