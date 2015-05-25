// Source file for router
// Nicolas Langley 904433991
#include <cstring>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "my-router.h"

// Constructor
Router::Router(int port, int buf_size) : buffer_size(buf_size), port(port)
{
    // Arguments are: (address_family, datagram_service, protocol) 
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("Cannot create socket!");
    }

    // Bind the socket to a port number
    struct sockaddr_in router_addr;
    memset((char*)&router_addr, 0, sizeof(router_addr)); // Fill router_addr with 0s
    router_addr.sin_family = AF_INET; // Set the address family
    router_addr.sin_addr.s_addr = htonl(0x7f000001); // INADDR_ANY is the IP address
    router_addr.sin_port = htons(port); // Set the port for the socket

    // Set global class variable with address info
    addr = router_addr.sin_addr.s_addr;
    printf("Address of router on port %d is: %lu\n", port, addr);

    // Attempt to bind the socket to port
    int bind_result = bind(sock_fd, (struct sockaddr*)&router_addr, sizeof(router_addr));
    if (bind_result < 0) {
        perror("Bind failed!");
    }
}

void Router::send_message(unsigned long addr)
{
    // TODO: host info may not be needed
    struct hostent* hp; // Host information
    struct sockaddr_in serv_addr; // Server address info
    memset((char*)&serv_addr, 0, sizeof(serv_addr)); // Fill serv_addr with 0s
    serv_addr.sin_family = AF_INET; // Set the address family
    serv_addr.sin_port = htons(port); // Set the port for the socket
    int addr_length = sizeof(addr); // Get length of address - IPv4 should be 4 bytes
    memcpy((void*)&serv_addr.sin_addr.s_addr, &addr, addr_length); // Set target server address

    // This is the data to send
    char* message = "Test Message";

    // Send message to server
    int sendto_result = sendto(sock_fd, message, strlen(message), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (sendto_result < 0) {
        perror("Sending to server failed!");
    }
}

void Router::receive_message()
{
    struct sockaddr_in remote_addr; // Remote address info
    socklen_t addr_length = sizeof(remote_addr); // Length of addresses
    unsigned char buffer[buffer_size]; // Create the receive buffer
    int receive_len; // Number of bytes received

    // TODO: how do we handle the looping of server? - multiple threads?
    for (;;) {
        printf("Waiting for message on port: %d\n", port);
        receive_len = recvfrom(sock_fd, buffer, buffer_size, 0, (struct sockaddr*)&remote_addr, &addr_length);
        if (receive_len > 0) {
            buffer[receive_len] = 0;
            printf("Received: %s", buffer);
        }
    }
}

void Router::distance_vector_algorithm()
{
    printf("Distance Vector Algorithm");
}

