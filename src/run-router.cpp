#include <stdio.h>
#include <cstdlib>
#include <pthread.h>
#include "my-router.h"

void run_sender(Router* sender, unsigned int dest_addr)
{
    printf("Sending message to server at address: %d\n", dest_addr);
    sender->send_message(dest_addr);
}

void* run_receiver(void* threadarg)
{
    Router* receiver = (Router*)threadarg;
    printf("Running receiver in thread...\n");
    receiver->receive_message();
}

int main(int argc, char* argv[])
{
    // Create the receiver and sender routers
    Router* sender = new Router(4444, 2048);
    Router* receiver = new Router(5555, 2048);

    pthread_t threads[1];
    int rc;
    rc = pthread_create(&threads[0], NULL, run_receiver, (void*)receiver);
    if (rc) {
        printf("Unable to create thread");
        exit(-1);
    }
    run_sender(sender, receiver->addr);
}
