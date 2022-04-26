#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

// max concurrent connections
#define CONCURRENT_CONNECTION 3

// max connection requests queued
#define QUEUE_CONNECTION 40

// default port number
#define PORT 8888

// default buffer size 1024 bytes
#define BUFFER_SIZE 1024

// current number of connections
int connection = 0;

//the thread function
void *connection_handler(void *);

int main(int argc , char *argv[]) {
	int master_socket, conn_id, c;
	struct sockaddr_in server , client;
	
	sockaddr_in client_addr;
	
	if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		std::cout << "[ERROR] CAN'T CREATE TO SOCKET" << std::endl;
		return 0;
	} else {
		std::cout << "[NOTE] SOCKET CREATED DONE" << std::endl;
	}
	
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    
    // binding
    if (bind(master_socket, (struct sockaddr *)&server , sizeof(server)) == -1) {
    	perror("[ERROR]");
        return 0;
    } else {
    	std::cout << "[NOTE] BINDING DONE" << std::endl;
    }
    
    // Listen on the socket, with 40 max connection requests queued
    if (listen(master_socket, QUEUE_CONNECTION) == -1) {
    	std::cout << "[ERROR] CAN'T LISTEN" << std::endl;
        return 0;
    } else {
    	std::cout << "[INFO] WAITING FOR INCOMING CONNECTIONS" << std::endl;
    }
    
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	
    while (true) {
    	// accept client connection
        conn_id = accept(master_socket, (struct sockaddr *)&client, (socklen_t*)&c);
        // if client connection not accepted
        if (conn_id == -1) {
        	std::cout << "[WARNING] CAN'T ACCEPT NEW CONNECTION" << std::endl;
        } else { // else client connection accepted
            if (connection >= CONCURRENT_CONNECTION) {
            	std::cout << "[WARNING] TOO MANY CONNECTION" << std::endl;
                close(conn_id);
            } else {
            	std::cout << "[INFO] NEW CONNECTION ACCEPTED" << std::endl;
                // create thread for new connection
                if (pthread_create(&thread_id, NULL, connection_handler, (void*)&conn_id) == -1) {
                	std::cout << "[WARNING] CAN'T CREATE NEW THREAD" << std::endl;
                    // if the thread is not made then we will close the client connection
                    close(conn_id);
                } else {
                	std::cout << "[INFO] NEW THREAD CREATED" << std::endl;
                    connection++;
                }
            }
        }
    }
    return 0;
}

// This will handle connection for each client
void *connection_handler(void *sock_fd){
	// Get the socket descriptor
	int conn_id = *(int*)sock_fd, read_size;
	// store buffer
	char buffer[BUFFER_SIZE];
	
	// server response message
	const char* response = "Hello, client!";
	
	// receiving messages from client
	while ((read_size = recv(conn_id, buffer, BUFFER_SIZE, 0)) > 0) {
		// end of string marker
		buffer[read_size] = '\0';
		
		// printing client message
		std::cout << "[REQUEST] " << buffer << std::endl;
		
		// send message to client
		if (send(conn_id, response, strlen(response), 0) == -1) {
			std::cout << "[WARNING] CAN'T SEND RESPONSE" << std::endl;
		} else {
			std::cout << "[RESPONSE] " << response << std::endl;
		}
		
		// clear the message buffer
		memset(buffer, 0, BUFFER_SIZE);
	}
	
	// close client connection
	if (read_size!= 0) {
		if (close(conn_id) == 0) {
			std::cout << "[INFO] CONNECTION CLOSED" << std::endl;
		} else {
			std::cout << "[ERROR] CAN'T CLOSE CONNECTION" << std::endl;
		}
	} else {
		std::cout << "[INFO] CONNECTION CLOSED FROM CLIENT" << std::endl;
	}
	
	// terminate thread process
	if (pthread_detach(pthread_self()) == 0) {
		std::cout << "[INFO] THREAD TERMINATED" << std::endl;
	} else {
		std::cout << "[ERROR] CAN'T TERMINATE THREAD" << std::endl;
	}
	
	// 
	connection--;
	
	return NULL;
} 