#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <thread>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fstream>


#define SERVER_PORT 9999
using namespace std;

char recv_buffer[BUFSIZ];
char file_name[BUFSIZ];

int server_socket, client_socket;
struct sockaddr_in server_address;
struct sockaddr_in client_address;


int send_FTP(char* file_name, int client_socket, sockaddr_in client_address){
	//send to client
	
	return 0;
}

int client_send_name(int client_socket, sockaddr_in client_address){

		
	if(recv_buffer[0] == 'g' && recv_buffer[1] == 'e' && recv_buffer[2] == 't'){
		char *p = strchr(recv_buffer, ' ')+1;	
		strcpy(file_name, p);
		cout <<"Client requsts the tranmission of: \""<<file_name <<"\""<< endl;
		int n = sendto(client_socket, file_name, strlen(file_name), 0,(struct sockaddr*)&client_address, sizeof(client_address));
	
		if(n<0) cout<< "Server failed to send name to client" <<endl;				
				
	}	
	return 0;
}
int client_send_packet_num(int client_socket, sockaddr_in client_address, int total_packet){
	char packet_num_buffer[100];
	string packet_num_str = to_string(total_packet);
	strcpy(packet_num_buffer, packet_num_str.c_str());
	cout<<"packet_num_buffer: "<< packet_num_buffer<< endl;
	int n = sendto(client_socket, packet_num_buffer, strlen(packet_num_buffer),0,(struct sockaddr*)&client_address, sizeof(client_address));
	if(n<0) cout<< "Server failed to send packet number to client" <<endl;				
    
    return 0;

}
int client_send_file(){
	
	cout<< "Server Starts sending file to client" <<endl;
	char MTU_1500[1472];
	char MTU_9001[8973];
	int token = 0;

	int n;
    int total_packet;
    int file_size;
    int count_num = 0;

    ifstream myfile;
    string input = file_name;
    myfile.open(input);

    std::streampos begin, end;
	begin = myfile.tellg();
	myfile.seekg(0, ios::end);
	end = myfile.tellg();
	file_size = end-begin;
	total_packet = (end-begin)/1472 + 1;
	cout << "The total file size: " << file_size << " bytes." << endl;
	cout << "The total packets number: " << total_packet << " bytes." << endl;
	
	client_send_packet_num(client_socket, client_address, total_packet);

	myfile.seekg(0, ios::beg);
   
        //no_packet = 1024^3 / (1472-6) + 1;
        while(!myfile.eof()){
        
            myfile.read(MTU_1500, sizeof(MTU_1500));
            
            n = sendto(client_socket, MTU_1500, strlen(MTU_1500),0,(struct sockaddr*)&client_address, sizeof(client_address));
            	
            cout<<"Sending packet No."<<count_num<<endl;
            count_num++;
            memset(MTU_1500, 0, sizeof(MTU_1500));
        }
    
    myfile.close();
	return 0;
}


int main(int argc, char const *argv[])
{
	bzero(recv_buffer, sizeof(recv_buffer));
	bzero(file_name, sizeof(file_name));

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

	recvfrom(server_socket, recv_buffer, 99, 0, (struct sockaddr *)&client_address, &client_length);
	cout <<"Client send requsts: \""<< recv_buffer <<"\""<< endl;

	//send the file name to client
	client_send_name(server_socket, client_address);

	//create a thread to send file
	thread file_transfer(client_send_file);
	file_transfer.join();



	close(client_socket);
	close(server_socket);
	return 0;
}




