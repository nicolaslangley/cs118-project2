#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include "my-router.h"

void run_sender(Router* sender, unsigned int dest_addr, int dest_port)
{
    printf("Sending message to server at address %lu on port %d\n", htonl(dest_addr), dest_port);
    AODVRequest* req_message = new AODVRequest(htonl(sender->addr), htonl(dest_addr),1,htonl(sender->addr),htonl(sender->routerSequenceNumber));
    AODVResponse* res_message = new AODVResponse(htonl(sender->addr), htonl(dest_addr));
    char* serialized_message = res_message->serialize();
    sender->send_message(dest_addr, dest_port, serialized_message);
}

int main(int argc, char* argv[])
{
    if (strncmp(argv[1],"-r",2) == 0) {
        printf("Matched -r\n");
        Router* receiver = new Router(5555, 2048);
        receiver->receive_message();
    } else if (strncmp(argv[1],"-s",2) == 0) {
        printf("Matched -s\n");
        Router* sender = new Router(4444, 2048);
        run_sender(sender, htonl(0x7f000001), 5555);
    } else {
        printf("Usage: -r for receiver or -s for sender\n");
    }
}
