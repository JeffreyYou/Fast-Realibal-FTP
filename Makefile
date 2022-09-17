all:	Server Client

Server: 
		g++ -pthread ftp_Server.cpp -o ftp_Server
Client:
		g++ -pthread ftp_Client.cpp -o ftp_Client