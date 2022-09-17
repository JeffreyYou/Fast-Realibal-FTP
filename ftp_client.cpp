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
    int Bportnum = 31533;

    int c = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverhint;
    serverhint.sin_family = AF_INET;
    serverhint.sin_port = htons(Bportnum);
    serverhint.sin_addr.s_addr = inet_addr("127.0.0.1"); //
    int test = connect(c, (struct sockaddr*)&serverhint, sizeof(serverhint));
    if (test == -1){
        cout << "test1" << "\n";
    }


    //tcp
    while (true){
        memset(buf,size(buf),0);
        string s = "filename";
        int test2 =  send(c,s,strlen(s),0);
        if (test2 == -1){
            cout << "test2" << "\n";
        }
        
        memset(buf, 0, sizeof(buf));
        int len = recv(c, buf, sizeof(buf), 0);
        fstream yourfile;
        if (yourfile.is_open()){  
            myfile <<buf;
            myfile.close();
        } 
       
    
    }

    

    //udp
    int Aportnum = 30533;
    int sck = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(Aportnum);
    hint.sin_addr.s_addr = INADDR_ANY;
    int test2 = ::bind(sck,(sockaddr*)&hint,sizeof(hint));
    if (test2 == -1){
        cout << "test2" << "\n";
    }
    while(true){
        memset(buf,size(buf),0);
        int bin = recvfrom(sck,buf,1024,0,(struct sockaddr*) &client,&len);
        string message(buf,bin);
        message.pop_back();
        if (yourfile.is_open()){  
            myfile <<buf;
            myfile.close();
        } 
    }
    yourfile.close();
}