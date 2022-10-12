#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <map>
#include <iostream>
#include <thread>
#include <fstream>
#include <mutex>
#include <arpa/inet.h>
#include <condition_variable>
#include <sys/time.h>
#include <sstream>
 
using namespace std;

int server_sockfd;
#define SERVER_PORT 9999
map<int,char*> packet_map;
int current_token=0;
int token_num=0;
int max_token;

char recv_buffer[1472];
char file_name[100];
char packet_num[100];
char packet_size[100];
char write_buffer[1472];

bool is_map_full = false;
bool isFinish = false;
int total_packetNum = 0;
int total_packetSize = 0;

bool* check_p;  
bool* c_p;  
bool* c_p_e;  

int number_store;

struct sockaddr_in client_recv_address, client_send_address;

struct timeval start_time;
struct timeval finish_time;
struct timeval time1;
struct timeval time2;
struct timeval time3;
mutex mtx;
void set_token(int token_, char* thistok){
        //convert token to char array of 00xxxx format
        int tok_len = 6;

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
void set_token_2(int token_, char* thistok){
        //convert token to char array of 00xxxx format
        int tok_len = 6;

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

int resend_packet(int number){
        char resend_buffer[6];
        set_token(number, resend_buffer);
        sendto(server_sockfd, resend_buffer, sizeof(resend_buffer), 0, (struct sockaddr*)&client_send_address, sizeof(client_send_address));

        return 0;
}
int resend_packet_write(int number){
        char resend_buffer[6];
        set_token_2(number, resend_buffer);
        sendto(server_sockfd, resend_buffer, sizeof(resend_buffer), 0, (struct sockaddr*)&client_send_address, sizeof(client_send_address));

        return 0;
}

//----------------------------Store-----------------------
int store_packet_in_map(){
          
        int total_stored_packet=0;
	max_token=0;
	int buffer_full_num=0;
	bool isSent = false;
	ifstream udp_buffer;
        while(total_stored_packet != total_packetNum){
            memset(recv_buffer, 0, 2000);
            int numbytes = recvfrom(server_sockfd, recv_buffer, 1472, 0, NULL, 0);

            token_num = 0;
            for(int j =0;j<6;j++){
                token_num = token_num * 10 + (recv_buffer[j]-'0');
            }

            if( (total_stored_packet>0) && (buffer_full_num==0)){
    		      is_map_full = true;
            }
	        if(packet_map.size()>300000){
	 	      is_map_full = false;
	        }

            // Read the UDP Buffer size
            udp_buffer.open("/proc/net/udp");
            stringstream buf;
            buf << udp_buffer.rdbuf();
            udp_buffer.close();
            string temp = buf.str();
            size_t found;
            found = temp.find("  1000  ");
            string mystr = temp.substr(found-29, 8);
            string mystr1 = mystr.substr(2, 6); //Buffer Size
            string mystr2 = temp.substr(found+43, 4); // Dropped Packet

            if(mystr1.at(0) == '0' && mystr1.at(1) < '3'){
			    if(mystr1.at(1) == '0' && mystr1.at(1) < '3'){
				    if(isSent == false) is_map_full = true;
		      	    cout<<"--UDP buffer:--:"<<mystr1<<", droped packet: "<<mystr2<<", map size: "<<packet_map.size()<<", recv token: "<<token_num<<", Writing packet: "<<current_token<<endl;
		     	 }
            }else{
                //Add Compensation
			    isSent = false;
		      	is_map_full = false;		
		      	if(mystr1.at(0) == '1') buffer_full_num = 300;
		      	if(mystr1.at(0) == '2') buffer_full_num = 500;
		     	cout<<"--UDP buffer full:--:"<<mystr1<<" bytes, Add compensate: "<< buffer_full_num<<", droped packet: "<<mystr2<<", map size: "<<packet_map.size()<<", recv token: "<<token_num<<", Writing packet: "<<current_token<<endl;
            }

	        if(buffer_full_num !=0){
		        buffer_full_num--;
		        udp_buffer.open("/proc/net/udp");
		        stringstream buf;
		        buf << udp_buffer.rdbuf();
		        udp_buffer.close();
		        string temp = buf.str();
		        found = temp.find("  1000  ");
                string mystr3 = temp.substr(found-29, 8);
		        string mystr4 = mystr.substr(2, 6);
		        string mystr5 = temp.substr(found+43, 4);
		        cout<<"buffer_full_num: "<<buffer_full_num<<", buffer contains: "<<mystr4<<endl;
	        }
	        if(buffer_full_num !=0) buffer_full_num--;
	        if(max_token<token_num) max_token = token_num;
            number_store = max_token;


    	    if(is_map_full && token_num<500000){
		        char ok_buffer[] = {'9','9','9','9','9','8','\0'};
		        int c = sendto(server_sockfd, ok_buffer, strlen(ok_buffer), 0, (struct sockaddr*)&client_send_address, sizeof(client_send_address));
		        isSent =true;
            }else{
		        if(is_map_full && buffer_full_num ==0){
		            char ok_buffer[] = {'9','9','9','9','9','8','\0'};
                    int c = sendto(server_sockfd, ok_buffer, strlen(ok_buffer), 0, (struct sockaddr*)&client_send_address, sizeof(client_send_address));
		    }
	        }
            if(check_p[token_num] == false){ //not flase means drop this packet
                check_p[token_num] = true;
                
                char* ptr = (char*) malloc(1472);
                for(int u=0; u<1472; u++){
                   ptr[u] = recv_buffer[u];
                }
                pair<int,char*> p(token_num, ptr);
                mtx.lock();
                packet_map.insert(p);
                mtx.unlock();
                total_stored_packet++;
            }
        }
        return 0;
}

// -----------------------Write---------------------------
int write_packet_func(){
        ofstream write_file;
        write_file.open("./data.bin", ios::binary | ios::out);

        while(current_token != total_packetNum){
            if(packet_map.count(current_token)>0){

                if(current_token != (total_packetNum-1)){
		            //cout<<"write: "<<packet_map[current_token]<<endl;
		            //cout<<"write+6: "<<packet_map[current_token]+6<<endl;
                    write_file.write(packet_map[current_token]+6, 1466);
                }else{
		            //cout<<"write: "<<packet_map[current_token]<<endl;
                    // cout<<"write+6: "<<packet_map[current_token]+6<<endl;
                    write_file.write(packet_map[current_token]+6, total_packetSize-(total_packetNum-1)*1466);
		            cout<<"last wirte size: "<< total_packetSize-(total_packetNum-1)*1466<<endl;
                }

                //cout<<packet_map[current_token]<<endl;
                //cout<<"write packet: "<<current_token<< ", Map size: "<<packet_map.size()<<", is_Map_full : "<<is_map_full<<endl;
		        char* deletp = packet_map[current_token];
	            mtx.lock();
                packet_map.erase(current_token);
                mtx.unlock();
                current_token++;
		        delete deletp;
            }else{
	            for(int s = current_token; s<(current_token+5000 < max_token ? current_token+50:max_token); s++){
			        if(s >= total_packetNum-1) break;
			        if(check_p[s] == false){
				        resend_packet(s);
				        resend_packet(s);
			        }
		    }
                usleep(30);
            }
        }
        isFinish = true;
        return 0;
}

int main(int argc, char const *argv[]){

    char send_buffer[] = "get data.bin";
    
    string fileName;
    int packetNum;

    //memset(recv_buffer, 0, sizeof(recv_buffer));
    //create socket to send message to server
    server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&client_send_address, sizeof(client_send_address));

    client_send_address.sin_family = AF_INET;
    client_send_address.sin_port = htons(SERVER_PORT);//9999
    //client_send_address.sin_addr.s_addr = INADDR_ANY;
    //inet_pton(AF_INET, "10.0.1.108", &client_send_address.sin_addr); 
    inet_pton(AF_INET, "10.0.1.108", &client_send_address.sin_addr);

    int numbytes = sendto(server_sockfd, send_buffer, sizeof(send_buffer), 0, (struct sockaddr*)&client_send_address, sizeof(client_send_address));
    if(numbytes < 0){
        cout<<"sendto error"<<endl;
    }
    memset(send_buffer, 0, sizeof(send_buffer));
    //recv file name
    memset(file_name, 0, sizeof(file_name));
    numbytes = recvfrom(server_sockfd, file_name, 100, 0, NULL, 0);
    //store the file name
    fileName = file_name; 
    cout<<"the name of the file: \""<< fileName <<"\""<< endl;
    //create file
    ofstream create_file;
    create_file.open(fileName, ios::binary | ios::out | ios::trunc);
    create_file.close();
    
    memset(packet_num, 0, sizeof(packet_num));
    numbytes = recvfrom(server_sockfd, packet_num, 100, 0, NULL, 0);
    //store the packet number
    for(int s=0; s<strlen(packet_num); s++){
        total_packetNum = total_packetNum*10 + (packet_num[s]-'0');
    }
    cout<<"the total packet to be sent: \""<< total_packetNum <<"\""<< endl;

    bool recv_check[total_packetNum];
    for(int i = 0;i<total_packetNum;i++){
        recv_check[i] = false;
    }
    check_p = recv_check;

    memset(packet_size, 0, sizeof(packet_size));
    numbytes = recvfrom(server_sockfd, packet_size, 100, 0, NULL, 0);
    for(int s=0; s<strlen(packet_size); s++){
        total_packetSize = total_packetSize*10 + (packet_size[s]-'0');
    }
    cout<<"the total packet size: \""<< total_packetSize <<"\""<< endl;
    
    gettimeofday(&start_time, NULL);
    cout<<"-------------Tranmission Start---------------"<<endl;

    thread store_packet(store_packet_in_map);
    thread write_packet(write_packet_func);

    store_packet.join();
    write_packet.join();

    char finish_buffer[] = {'9','9','9','9','9','9','\0'};
    gettimeofday(&finish_time, NULL);
    
    cout<<"------------Trasmission Finish!----------------" << endl;
    cout<<"Total time cost: "<<finish_time.tv_sec-start_time.tv_sec<<" s"<<endl; 
    cout<<"Transmission Speed: "<<(double)(1024*8)/(finish_time.tv_sec-start_time.tv_sec)<<" Mbps"<<endl;

    if(isFinish){
       int c = sendto(server_sockfd, finish_buffer, strlen(finish_buffer), 0, (struct sockaddr*)&client_send_address, sizeof(client_send_address));
        if(c<0){
        cout<<"send fail"<<endl;
        }
    }
    close(server_sockfd);
    
    return 0;

}

