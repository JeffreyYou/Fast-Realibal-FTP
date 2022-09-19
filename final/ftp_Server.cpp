#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <thread>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <mutex>
#include <condition_variable> 

#define SERVER_PORT 9999
using namespace std;

char recv_buffer[BUFSIZ];
char file_name[BUFSIZ];
char listen_buffer[200];

char tok[6];
int tok_len = 6;

int server_socket;
struct sockaddr_in server_address;
struct sockaddr_in client_address;
socklen_t server_length;
socklen_t client_length;

mutex mtx;
mutex lk;
condition_variable cv;
int cargo = 0;
bool shipment_available() {return cargo!=0;}

void set_token(int token_, char* thistok){
	    //convert token to char array of 00xxxx format
	    string count_str;
	    count_str = to_string(token_);
	    tok_len = tok_len-count_str.length();
	    while(tok_len != 0){
	        count_str = "0" + count_str;
	        tok_len = tok_len - 1;
	    }
	    tok_len = 6;
	    strcpy(thistok, count_str.c_str());
}

int client_send_name(){

		
	if(recv_buffer[0] == 'g' && recv_buffer[1] == 'e' && recv_buffer[2] == 't'){
		char *p = strchr(recv_buffer, ' ')+1;	
		strcpy(file_name, p);
		cout <<"Client requsts the tranmission of: \""<<file_name <<"\""<< endl;
		int n = sendto(server_socket, file_name, strlen(file_name), 0,(struct sockaddr*)&client_address, sizeof(client_address));
	
		if(n<0) cout<< "Server failed to send name to client" <<endl;				
				
	}	
	return 0;
}
int client_send_packet_num(int total_packet){
		char packet_num_buffer[100];
		string packet_num_str = to_string(total_packet);
		strcpy(packet_num_buffer, packet_num_str.c_str());

		int n = sendto(server_socket, packet_num_buffer, strlen(packet_num_buffer),0,(struct sockaddr*)&client_address, sizeof(client_address));
		if(n<0) cout<< "Server failed to send packet number to client" <<endl;				
	    
	    return 0;

}
int send_lost_packet(int resend_token){
		char MTU_1500[1472];
		char MTU_9001[8973];
	    int no_packet;
	    int count = 0;
	    int seek;

	    ifstream myfile;
	    string input = file_name;
	    myfile.open(input);
    
        seek = resend_token*(1472-6);   
        myfile.seekg(seek,ios::beg);

		 //MTU_1500 = tok + packet;
        myfile.read(MTU_1500+6, sizeof(MTU_1500)-6);

        char mytok[6];
        set_token(resend_token, mytok);
        for(int x=0; x<6; x++){
            MTU_1500[x] = mytok[x];
        }
        //mtx.lock();
        int n = sendto(server_socket, MTU_1500, strlen(MTU_1500),0,(struct sockaddr*)&client_address, sizeof(client_address));
	//mtx.unlock();
		if(n<0) cout<< "Server failed resend packet: " <<resend_token<<endl;

        memset(MTU_1500, 0, sizeof(MTU_1500));
        return 0;
  
}

int listen_resend_packet(){
	int stop_num=0;
	while(true){
        memset(listen_buffer, 0, sizeof(listen_buffer));
		recvfrom(server_socket, listen_buffer, 99, 0, (struct sockaddr *)&client_address, &client_length);

	int resend_token = 0;
	for(int j =0;j<6;j++){
            resend_token = resend_token * 10 + (listen_buffer[j]-'0');
        }
        if(resend_token == 999999){
        	break;
        }
	
	if(resend_token == 999998){
		cout<<"listen request-------------------"<<stop_num<<"---------------------------: "<<resend_token<<endl;
		//unique_lock<mutex> lck(lk);
		stop_num++;
		cargo = 1;
		cv.notify_one();
		
	}else{
        //thread resend_loss_packet(send_lost_packet, resend_token);
        	send_lost_packet(resend_token);
	}
	}
	return 0;

}
int send_total_packet(){
	
	cout<< "Server Starts sending file to client" <<endl;
	char MTU_1500[1472];
	char MTU_9001[8973];
	int token = 0;
	char main_tok[6];

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
	total_packet = (end-begin)/1466 + 1;
	cout << "The total file size: " << file_size << " bytes." << endl;
	cout << "The total packets number: " << total_packet << endl;
	
	client_send_packet_num(total_packet);

	myfile.seekg(0, ios::beg);
   
        //no_packet = 1024^3 / (1472-6) + 1;
        while(!myfile.eof()){

        	set_token(token, main_tok);
        	for(n=0; n<6; n++){
                MTU_1500[n] = main_tok[n];
            }
            token++;
            myfile.read(MTU_1500+6, 1466);

            //mtx.lock();
            n = sendto(server_socket, MTU_1500, strlen(MTU_1500),0,(struct sockaddr*)&client_address, sizeof(client_address));
            //mtx.unlock();
            cout<<"Sending packet No."<<count_num<<endl;
            count_num++;
            memset(MTU_1500, 0, sizeof(MTU_1500));
 
	    if(count_num>0 && count_num%300 ==0){
		unique_lock<mutex> lck(lk);
		cv.wait(lck,shipment_available);
		mtx.lock();
		cargo = 0;
		mtx.unlock();
	    }
            
        }
    
    myfile.close();
	return 0;
}


int main(int argc, char const *argv[])
{
	bzero(recv_buffer, sizeof(recv_buffer));
	bzero(file_name, sizeof(file_name));

	
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
	client_send_name();

	//create a thread to send file
	thread file_transfer(send_total_packet);
	thread listen_loss_packet(listen_resend_packet);


	file_transfer.join();
	listen_loss_packet.join();
	close(server_socket);
	return 0;
}




