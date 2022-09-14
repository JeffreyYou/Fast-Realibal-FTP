# Computer Networks-FTP
Linux specific, FTP Server and Client using Socket Programming in C

Prerequisites:
* Access to two computers on the same network or a virtual machine (eg. grok.missouristate.edu in case you're using a single machine)
* Program for ftp_client and ftp_server

Contents of the directory:
* ftp_client.c
* ftp_server.c
* ftp_client
* ftp_server
* makemefile

Running the tests:
1. Initialize a socket (using socket())
2. Connect the socket to the address of the server (using connect())
3. Listening socket setup (using listen())
4. Assign tasks for "ls"
5. Get file name from request and transfer it to the destination

References used:

ftp://ftp.sas.com/techsup/download/SASC/share5958-59/S5958v2.pdf
https://www.tutorialspoint.com/c_standard_library/c_function_strcpy.htm
https://www.tutorialspoint.com/c_standard_library/c_function_atoi.htm
https://www.cs.nmsu.edu/~jcook/Tools/pthreads/library.html
http://www.cs.columbia.edu/~danr/courses/6761/Fall00/hw/pa1/6761-sockhelp.pdf
http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html
https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program

## License & copyright

&copy; Vignesh Sivanandha Rao, FTP Server and Client

Licensed under the [MIT License](LICENSE)
