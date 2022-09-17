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

void FileHandler(string s,ofstream &out){
    if(out.is_open()){
            out << s <<endl;
            out.flush();
        }else{
            cout << "Not opening" <<endl;
    }
}

int main(){
    //create a socket
    int port = 31533;
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

    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    ofstream out;
    out.open("b.txt", ios_base::app);
    
    char buf[66000]; 

    while(true){
        memset(buf, 0, 66000);
        int bin = recvfrom(sck,buf,1024,0,(struct sockaddr*) &client,&len);
        string s(buf);
        
        FileHandler(s,out);   
    }

    out.close();
    close(sck);

}

