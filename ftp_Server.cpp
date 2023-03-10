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
#include <map>
#define SERVER_PORT 9999
using namespace std;

char recv_buffer[BUFSIZ];
char file_name[BUFSIZ];
char listen_buffer[200];

char tok[6];
bool* check_p;  


int server_socket;
struct sockaddr_in server_address;
struct sockaddr_in client_address;
socklen_t server_length;
socklen_t client_length;

mutex mtx;
mutex lk;
mutex sender_mx;
condition_variable cv;
int cargo = 0;
bool shipment_available() {return cargo!=0;}
bool MTU1500 = false;
bool MTU9001 = false;
char MTU_1500[1472];
char MTU_9001[8973];


map<int,int> resend_map;
int* resend_arr;

void set_token_all(int token , char* thistok){
    //convert token to char array of 00xxxx format.
    int tok_len = 6;
    string count_str;
    count_str = to_string(token);
    tok_len = tok_len-count_str.length();
    while(tok_len != 0){
        count_str = "0" + count_str;
        tok_len = tok_len - 1;
    }
    tok_len = 6;
    strcpy(thistok, count_str.c_str());

}
void set_token_resend(int token , char* thistok){
    //convert token to char array of 00xxxx format.
    int tok_len = 6;


    string count_str;
    count_str = to_string(token);
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

int client_send_packet_size(int total_packet_size){
    char packet_size_buffer[100];
    string packet_size_str = to_string(total_packet_size);
    strcpy(packet_size_buffer, packet_size_str.c_str());
    //cout<<"packet_size_buffer: "<< packet_size_buffer<<endl;
    int n = sendto(server_socket, packet_size_buffer, strlen(packet_size_buffer),0,(struct sockaddr*)&client_address, sizeof(client_address));
    if(n<0) cout<< "Server failed to send packet number to client" <<endl;
    return 0;
}

int send_lost_packet(int resend_token){
    if(MTU1500) char MTU_1500[1472];
    if(MTU9001) char MTU_9001[8973];
    int no_packet;
    int count = 0;
    int seek;
	
	
	
	if(resend_arr[resend_token]==0){
		resend_arr[resend_token]=1;
		//pair<int,int> p =(resend_token, tmp);
		//resend_map.insert(p);
	}else{
		resend_arr[resend_token]++;
	       if(resend_arr[resend_token]%5 == 0 ){
				
		}else{
		//cout<<"stop"<<endl;
		return 0;
		}       
    }
    ifstream myfile;
    string input = file_name;

    myfile.open(input,ios::binary | ios::in);
    if(MTU1500) seek = resend_token*(1472-6);
    if(MTU9001) seek = resend_token*(8973-6);
    myfile.seekg(seek,ios::beg);
     //MTU_1500 = tok + packet;
    if(MTU1500) myfile.read(MTU_1500+6, 1466);
    if(MTU9001) myfile.read(MTU_1500+6, 8967);

    char mytok[6];
    memset(mytok, 0, sizeof(mytok));

    //sender_mx.lock();
    set_token_resend(resend_token, mytok);
    //sender_mx.unlock();

    for(int x=0; x<6; x++){
        if(MTU1500) MTU_1500[x] = mytok[x];
        if(MTU9001) MTU_9001[x] = mytok[x];
    }
    //cout<< "file read finish:"<< resend_token<<endl;
    //sender_mx.lock();
    //cout<< "go to sendto:"<< resend_token<<endl;
    int n;
    if(MTU1500)
        n = sendto(server_socket, MTU_1500, 1472, 0,(struct sockaddr*)&client_address, sizeof(client_address));
    if(MTU9001)
        n = sendto(server_socket, MTU_9001, 8973, 0,(struct sockaddr*)&client_address, sizeof(client_address));

    //sender_mx.unlock();
    //	cout<<"---Resend success: "<<resend_token<<endl;
    //	cout<<"send bytes: "<< n<<endl;
    if(n<0) cout<< "Server failed resend packet: " <<resend_token<<endl;

    if(MTU1500) memset(MTU_1500, 0, sizeof(MTU_1500));
    if(MTU9001) memset(MTU_9001, 0, sizeof(MTU_9001));
    return 0;
}

// ------------------------Listen-----------------------
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

	//cout<<"listen from-------------------"<<resend_token<<"---------------------------: "<<resend_token<<endl;

	if(resend_token == 999998){
		//cout<<"Listen Restart request-------------------"<<stop_num<<"---------------------------: "<<resend_token<<endl;
		//unique_lock<mutex> lck(lk);
		stop_num++;
		cargo = 1;
		cv.notify_one();
		
	}else{
        //thread resend_loss_packet(send_lost_packet, resend_token);
		//cout<<"go to send packet: "<<resend_token<<endl;
        send_lost_packet(resend_token);
	}
	}
	return 0;

}

//-------------------Send Total-------------------
int send_total_packet(){
	
	cout<< "Server Starts sending file to client" <<endl;
    //if(MTU1500) char MTU_1500[1472];
    //if(MTU9001) char MTU_9001[8973];

	int token = 0;
	char main_tok[6];

	int n;
    int total_packet;
    int file_size;
    int count_num = 0;

    ifstream myfile;
    string input = file_name;
    myfile.open(input,ios::binary | ios::in);

    std::streampos begin, end;
	begin = myfile.tellg();
	myfile.seekg(0, ios::end);
	end = myfile.tellg();
	file_size = end-begin;

    if(MTU1500) total_packet = (end-begin)/1466 + 1;
    if(MTU9001) total_packet = (end-begin)/8967 + 1;

	cout << "The total file size: " << file_size << " bytes." << endl;
	cout << "The total packets number: " << total_packet << endl;

	int resend_check_buffer[total_packet];
	for(int j=0; j<total_packet;j++){
		resend_check_buffer[j]=0;
	}
	resend_arr = resend_check_buffer;
	
	client_send_packet_num(total_packet);
	client_send_packet_size(file_size);

	myfile.seekg(0, ios::beg);
    int k = 0;

    //no_packet = 1024^3 / (1472-6) + 1;
    while(count_num < total_packet){

        //sender_mx.lock();
        set_token_all(token, main_tok);
        //sender_mx.unlock();
//            if(MTU1500) char* this_buffer = (char*)malloc(1472);
//            if(MTU9001) char* this_buffer = (char*)malloc(8973);
//
//        	for(n=0; n<6; n++){
//                this_buffer[n] = main_tok[n];
//            }
//            token++;

        if(MTU1500) {
            char* this_buffer = (char*)malloc(1472);
            for(n=0; n<6; n++){
            this_buffer[n] = main_tok[n];
            }
            token++;
            myfile.read(this_buffer+6, 1466);
            n = sendto(server_socket, this_buffer, 1472,0,(struct sockaddr*)&client_address, sizeof(client_address));
            free(this_buffer);
        }
        if(MTU9001) {
            char* this_buffer = (char*)malloc(8973);
            for(n=0; n<6; n++){
            this_buffer[n] = main_tok[n];
            }
            token++;
            myfile.read(this_buffer+6, 8967);
            n = sendto(server_socket, this_buffer, 8973,0,(struct sockaddr*)&client_address, sizeof(client_address));
            free(this_buffer);
        }
        //n = sendto(server_socket, MTU_1500, strlen(MTU_1500),0,(struct sockaddr*)&client_address, sizeof(client_address));
        //free(this_buffer);
        if(n>0){
            cout<<"Normal sending packet No."<<count_num<<", packet size: "<<n<<endl;
        }else{
            cout<<"Failed to Send packet No."<<count_num<<endl;
        }
	   //system("cat /proc/net/udp");
	    
        usleep(50);
        count_num++;
        //memset(MTU_1500, 0, sizeof(MTU_1500));
 

	    if(count_num>0 && count_num%150 ==0){
            unique_lock<mutex> lck(lk);
            cv.wait(lck,shipment_available);
            //usleep(500);
            cargo = 0;
        }
            
    }
    //cout<<"Main send close."<<endl;
    myfile.close();
	return 0;
}


int main(int argc, char const *argv[])
{
    if(argv[1][0] == '1') MTU1500 = true;
    if(argv[1][0] == '9') MTU9001 = true;
    //cout<<"MTU1500: "<< MTU1500<<endl;
    //cout<<"MTU9001: "<< MTU9001<<endl;


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




