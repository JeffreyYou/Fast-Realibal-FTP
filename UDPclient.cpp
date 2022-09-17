#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>
using namespace std;

int main(){

    //create a socket
    int port = 30533;
    int sck = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.s_addr = INADDR_ANY;
    int test1 = ::bind(sck,(sockaddr*)&hint,sizeof(hint));
    if (test1 == -1){
        cout << "test1" << "\n";
    }
    cout << "Client is up and running using UDP on port <" << port << ">" << endl;

    ofstream out;
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    char buf[1024];
    int sendMain;

    while(true){
        memset(buf, 0, 1024);
        int bin = recvfrom(sck,buf,1024,0,(struct sockaddr*) &client,&len);
        string s(buf);
        memset(str,0,1024);
    }
    out.close()
    close(sck);

}

void ClientHandler(string s){
    out.open("myfile.txt", ios::app);
    out << str;
}