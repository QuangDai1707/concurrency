# 1. 

## 1.1. Computer Layer Architecture
    -   Applications
    -   System call Interface (socket, etc)
    -   OS/Kernel
    -   Hardware layer

# 2. Socket
- Unix/Linux OS provide Socket Interface (a bunch of Socket programming related APIs) to carry out communication between various types of entities
- Two main types:
    - Unix Domain Sockets: processes on same machine
    - Network Sockets: processes on difference machine over network
- Step to implement a socket communication: (using socket system call APIs)
    1. Remove the socket, if already exists
    2. Create a Unix socket using **socket()**
    3. Specify the socket name
    4. Bind the socket using **bind()**
    5. **listen()**
    6. **accept()**
    7. Read the data recvd on socket using **recvfrom()**
    8. send back the result using **sendto()**
    9. **close()** the data socket
    10. **close()** the connection socket
    11. Remove the socket
    12. exit
- Socket message types
    - Connection initiation request messages
        - this msg is used by the client process to request the server process to establish a dedicated connection
        - only after the connection has been established, then only client can send **service request messages** to server
    - Service request messages
        - client can send these msg to server once the connection is fully established
        - through these messages, client requests server to provide a service
- State machine of Socket based Client Server Communication
    1. When Server boots up, it creates a connection socket (also called “master socket file descriptor” using socket())
    2. M is the mother of all Client Handles. M gives birth to all Client handles. Client handles are also called “data_sockets”
    3. Once Client handles are created for each client, Server carries out Communication (actual data exchange) with the client using Client handle (and not M).
    4. Server Has to maintain the database of connected client handles Or data sockets
    5. M is only used to create new client handles. M is not used for data exchange with already connected clients.
    6. accept() is the system call used on server side to create client handles.
    7. In Linux Terminology, handles are called as “file descriptors” which are just positive integer numbers. Client handles are called ”communication file descriptors” Or “Data Sockets” and M is called “Master socket file descriptor” Or “Connection socket”
- Unix domain socket
    - setup STREAM or DATAGRAM based communication
        - STREAM: when large files need to be moved or copied from one location to another (continuous flow of bytes)
        - DATAGRAM: when small units of data need to be moved from one process to another within a system
    - Bind socket to socket name: 
        - Purpose of bind() system call is that application() dictate the underlying operating system the criteria of recieving the data. 
        - Here, bind() system call is telling the OS that if sender process sends the data destined to socket "/tmp/DemoSocket", then such data needs to be delivered to this server process (the server process)
- Multiplexing
    - Multiplexing is a mechanism through which the Server process can monitor multiple clients at the same time
    - Without Multiplexing, server process can entertain only one client at a time, and cannot entertain other client’s requests until it finishes with the current client
    - With Multiplexing, Server can entertain multiple connected clients simultaneously