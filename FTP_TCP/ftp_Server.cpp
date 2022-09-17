#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <thread>
#include <iostream>
#include <string>

#include "ftp_Server_func.h"

//using namespace std;



int main(int argc, char const *argv[])
{
	int myPort = 99999;
	int server_socket, client_socket;

	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	socklen_t server_length;
	socklen_t client_length;
	int isError;

	
	//open server socket
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0){
		printf("Failed to open socket");
		return -1;
	}

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(myPort);

	//bind socket to server_address
	server_length = sizeof(server_address);
	isError = bind(server_socket, (struct sockaddr *)&server_address,server_length);
	if(isError<0){
		printf("Failed to bind socket to address");
		return -1;
	}

	//listen for incoming request
	listen(server_socket, 5);


	client_length = sizeof(client_address);
	while(client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_length)){

		std::thread client_thread(clientHandle, client_socket);

		client_thread.join();
	}

	if(client_socket<0){
		printf("Failed to accept from client");
		return -1;
	}
	

	return 0;
}




