server:	clear ServerC ClientC runS
client:	clear ServerC ClientC runC
clear:
		clear
ServerC: 
		g++ -std=c++11 -g -pthread ftp_Server.cpp -o server
ClientC:
		g++ -std=c++11 -g -pthread ftp_Client.cpp -o client

runS:	
		./server
runC:	
		./client