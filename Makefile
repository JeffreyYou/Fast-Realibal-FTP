all:	Server Client

Server: 
		g++ -pthread ftp_Server.cpp -o server
Client:
		g++ -pthread ftp_Client.cpp -o client