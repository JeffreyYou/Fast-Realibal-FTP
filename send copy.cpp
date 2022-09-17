
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

char MTU_1500[1472];
char MTU_9001[8973];
int token = 0;

void send(char* filename, bool MTU, int socket){
//int main(){
    int n;
    int no_packet;
    int count = 0;
    ifstream file;
    file.open("data.bin");
    //seekg() locate position of tokens
    //MTU = 1, packet is 1500 byte
    if(MTU){
        //MTU = 1500
        //decide the size of file, set header for token to be 6 byte
        //no_packet = 1024^3 / (1472-6) + 1;
        while(!file.eof()){
        
                file.read(MTU_1500, sizeof(MTU_1500));
            
                n = sendto(socket, MTU_1500, sizeof(MTU_1500));
            
                //cout << MTU_1500 << endl;
            

            //reset
            memset(MTU_1500, 0, sizeof(MTU_1500));
            //count tracks current sequence of packet;
        }
    
    file.close();


    }

}