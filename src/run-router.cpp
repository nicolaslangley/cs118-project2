#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include "my-router.h"

void run_sender(Router* sender, unsigned int dest_addr, int dest_port)
{
    printf("Sending message to server at address %lu on port %d\n", dest_addr, dest_port);
    sender->send_message(dest_addr, dest_port);
}

void* run_receiver(void* threadarg)
{
    Router* receiver = (Router*)threadarg;
    printf("Running receiver in thread...\n");
    receiver->receive_message();
}

int main(int argc, char* argv[])
{
    if (strncmp(argv[1],"-r",2) == 0) {
        printf("Matched -r\n");
        Router* receiver = new Router(5555, 2048);
        receiver->receive_message();
        //pthread_t threads[1];
        //int rc;
        //rc = pthread_create(&threads[0], NULL, run_receiver, (void*)receiver);
        //if (rc) {
        //    printf("Unable to create thread");
        //    exit(-1);
        //}
    } else if (strncmp(argv[1],"-s",2) == 0) {
        printf("Matched -s\n");
        Router* sender = new Router(4444, 2048);
        run_sender(sender, htonl(0x7f000001), 5555);
    } else {
        printf("Usage: -r for receiver or -s for sender\n");
    }
}
