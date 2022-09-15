

// Handle the Client
void clientHandle(int client_socket){

	char client_buffer[BUFSIZ];
	char server_buffer[BUFSIZ];
	char file_buffer[BUFSIZ];

	while(1){
		bzero(client_buffer, sizeof(client_buffer));
		bzero(server_buffer, sizeof(client_buffer));
		if(recv(client_socket, client_buffer, sizeof(client_buffer), 0) == 0){
			//if no data received, exit
			return 0;
		}

		if(client_buffer[0] == 'g' && client_buffer[0] == 'e' && client_buffer[0] == 't'){
			int *p = strchr(client_buffer, ' ') + 1;
			strcpy(file_buffer, p);
		}
	}

	return;
}