all:	Server

Server: 
		g++ -pthread ftp_Server.cpp ftp_Server_func.cpp -o ftp_Server