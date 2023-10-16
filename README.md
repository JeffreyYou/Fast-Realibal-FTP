# Fast and Reliable File Transfer Protocol
## Why This
### Traditional FTP
The traditional FTP protocol is efficient in most scenarios. 

However, when faced with an unstable network environment, its efficacy wanes considerably. This is largely due to the TCP-based FTP's exponential back-off mechanism, which curtails the transmission rate upon detecting network congestion

### UDP-based FTP
This UDP-based file transfer protocol is an alternative solution.

Leveraging multi-threading, this protocol guarantees dependable file transfers between server and client, delivering excellent performance even in high-congestion and high-loss network conditions.

## Quick Start
### Server Setup
server.cpp:

- Description: The server waiting for the transmission.

- Compile: `g++ -std=c++11 -g -pthread ftp_Server.cpp -o server`

- Run: `./<target_file_name> <file_to_transfer> <host_port>`

- Makefile shortcut:

  `make server`

### Client Setup

client.cpp:

- Description: The client that establish connection with server and sends the file.

- Compile: `g++ -std=c++11 -g -pthread ftp_Client.cpp -o client`

- Run: `./<target_file_name> <path_to_store_file> <hostname> <host_port>`

- Makefile shortcut:

  `make client`

### File Integrity Check
MD5.py:
- Description: Generate the hash code of transferred file.
  
- Run: `python MD5.py`