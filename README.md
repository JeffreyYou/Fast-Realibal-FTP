# Fast and Reliable File Transfer Protocol

This file transfer protocol uses multi-thread to reliably transfer files from server to client over UDP, optimizing for performance under high-congestion and high-loss conditions.

## Instruction

server.cpp:

- Compile: `g++ -std=c++11 -g -pthread ftp_Server.cpp -o server`

- Run: `./<target_file_name> <file_to_transfer> <host_port>`

- Makefile shortcut:

  `make server`

client.cpp:

- Description:

- Compile: `g++ -std=c++11 -g -pthread ftp_Client.cpp -o client`

- Run: `./<target_file_name> <path_to_store_file> <hostname> <host_port>`

- Makefile shortcut:

  `make client`

