#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <thread>
#include <iostream>
#include <string.h>
#include <unistd.h>

#define SERVER_PORT 9999
using namespace std;
char buf[100];

int send_FTP(char* file_name, int client_socket, sockaddr_in client_address){
	//send to client
	int n = sendto(client_socket, file_name, strlen(file_name), 0,(struct sockaddr*)&client_address, sizeof(client_address));
	

	if(n<0){
		cout<< "Server failed to connect client" <<endl;
		
	}
	close(client_socket);
	return 0;
}

int clientHandle(int client_socket, sockaddr_in client_address){

	//char client_buffer[BUFSIZ];
	//char server_buffer[BUFSIZ];
	char file_name[BUFSIZ];

	while(1){
		//bzero(client_buffer, sizeof(client_buffer));
		//bzero(server_buffer, sizeof(client_buffer));
		bzero(file_name, sizeof(file_name));
		

		if(buf[0] == 'g' && buf[1] == 'e' && buf[2] == 't'){
			char *p = strchr(buf, ' ')+1;
			
			strcpy(file_name, p);
			cout << file_name << endl;
			send_FTP(file_name, client_socket, client_address);
			break;
		}
		//pthread_exit(NULL);
	}

	return 0;
}


int main(int argc, char const *argv[])
{
	int server_socket, client_socket;

	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	char str[INET_ADDRSTRLEN];

	socklen_t server_length;
	socklen_t client_length;
	int isError;

	
	//open server socket
	server_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(server_socket < 0){
		printf("Failed to open socket");
		return -1;
	}

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(SERVER_PORT);

	//bind socket to server_address
	server_length = sizeof(server_address);
	isError = bind(server_socket, (struct sockaddr *)&server_address,server_length);
	if(isError<0){
		cout<< "Failed to bind socket to address" <<endl;
		return -1;
	}

	cout<< "Server Starts listening to Client" <<endl;
	client_length = sizeof(client_address);
	recvfrom(server_socket, buf, 99, 0, (struct sockaddr *)&client_address, &client_length);
	cout << buf << endl;

	clientHandle(server_socket, client_address);
	// while(isError = recvfrom(server_socket, buf, 99, 0, (struct sockaddr *)&client_address, &client_length))
	// 	{

	// 	std::thread client_thread(clientHandle, client_socket);

	// 	client_thread.join();
	// }

	if(client_socket<0){
		printf("Failed to accept from client");
		return -1;
	}
	close(server_socket);

	return 0;
}




