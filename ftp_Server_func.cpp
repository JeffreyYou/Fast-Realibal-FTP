#include <stdio.h>
#include <string.h>
#include <sys/socket.h>


int send_FTP(char* file_name, int client_socket){
	//send to client
	int n = send(client_socket, file_name, sizeof(file_name), 0);
	if(n<0){
		printf("Server failed to connect client");
	}
	close(client_socket);
	return 0;
}

// Handle the Client
int clientHandle(int client_socket){

	char client_buffer[BUFSIZ];
	char server_buffer[BUFSIZ];
	char file_name[BUFSIZ];

	while(1){
		bzero(client_buffer, sizeof(client_buffer));
		bzero(server_buffer, sizeof(client_buffer));
		if(recv(client_socket, client_buffer, sizeof(client_buffer), 0) == 0){
			//if no data received, exit
			return 0;
		}

		if(client_buffer[0] == 'g' && client_buffer[0] == 'e' && client_buffer[0] == 't'){
			char *p = strchr(client_buffer, ' ');
			
			strcpy(file_name, p);
			send_FTP(file_name, client_socket);
		}
		
		pthread_exit(NULL);
	}

	return 0;
}