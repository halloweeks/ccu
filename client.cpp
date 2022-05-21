#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define ADDRESS "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]) {
	char buffer[BUFFER_SIZE];
	std::string msg;
	
	int sock = 0, valread = 0;
	struct sockaddr_in serv_addr;
		
	memset(&serv_addr, 0, sizeof(serv_addr));
		
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		std::cout << "[ERROR] CAN'T CREATE SOCKET\n";
		return 0;
	}
	
	// assign IP, PORT
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
		
	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, ADDRESS, &serv_addr.sin_addr) == -1) {
		std::cout << "[ERROR] INVALID ADDRESS/ ADDRESS NOT SUPPORTED" << std::endl;
		return 0;
	}
	
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
		std::cout << "[ERROR] CAN'T CONNECT TO THE HOST " << ADDRESS << ":" << PORT << std::endl;
		return 0;
	}
	
	for (;;) {
		std::cout << "[message]> ";
		getline(std::cin, msg);
		
		if (msg == "") {
			msg = "default message";
		} else if (msg == "exit") {
			std::cout << "[EXIT] BYE!\n";
			break;
		}
		
		if (send(sock, msg.c_str(), msg.size(), 0) > 0) {
			std::cout << "[SEND] " << msg << std::endl;
		} else {
			std::cout << "[ERROR] CAN'T SEND MESSAGE TO SERVER\n";
		}
		
		if (recv(sock, buffer, BUFFER_SIZE, 0) > 0) {
			std::cout << "[RECEIVE] " << buffer << std::endl;
		} else {
			std::cout << "[ERROR] CAN'T READ SERVER RESPONSE\n";
		}
		
		memset(buffer, 0, BUFFER_SIZE);
	}
	
	close(sock);
	return 0; 
}
