#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <iostream>
#include <ostream>
#include "my-router.h"

using namespace std;

void run_sender(Router* sender, unsigned int dest_addr, int dest_port)
{
    printf("Sending message to server at address %u on port %d\n", htonl(dest_addr), dest_port);
    AODVRequest* req_message = new AODVRequest(htonl(sender->addr), htonl(dest_addr),1,htonl(sender->addr),htonl(sender->addr), false);
    // AODVRequest(unsigned long orig_ip, unsigned long dest_ip, int hop_ct, unsigned long send_ip, unsigned long rec_ip, bool dest_rchd);

    AODVResponse* res_message = new AODVResponse(htonl(sender->addr), htonl(dest_addr));
    char* serialized_message = res_message->serialize();
    sender->send_message(dest_addr, dest_port, serialized_message);
}

void load_topology(string filename)
{
    //Node Topology: 
    node_id = port-9935;   //results in A->F 
    std::string tuple;  
    std::vector<std::string> topology; 
    std::ifstream tfile("topology.txt");
    int cnt = 0;
    if(tfile.is_open()) {
        while(getline(tfile, tuple)) {
            topology.push_back(tuple); 
            cnt++;
        }
        tfile.close();  		
    }
    else {
        fprintf(stderr, "Could not open file 'topology.txt'");
        exit(EXIT_FAILURE); 
    }
    for (int i = 0; i < cnt; i++) {
        if ((int)topology[i][0] == node_id) {
            tableEntryRouting entry = delimitTopology(topology[i]);
            routingTable.insert(std::pair<int, tableEntryRouting>(addr, entry));	
            printf("%c: Dest_ip: %lu Next_ip: %lu Hop_count: %d \n", node_id, entry.destination_ip, entry.next_ip, entry.hop_count);
        }	
    }
}

void* run_receiver(void* threadarg)
{
    Router* receiver = (Router*)threadarg;
    printf("Running receiver in thread...\n");
    receiver->receive_message();
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        cerr << "Incorrect usage!" << endl;
        exit(-1);
    }
    string topology_fname(argv[1]);
    // load_topology()
    // TODO: create routers 
    int router_count = 1;
    Router* routers[router_count];
    pthread_t threads[router_count];
    for (int i = 0; i < router_count; i++) {
        // For each router set it to listen in a new thread
        int rc = pthread_create(&threads[i], NULL, run_receiver, (void*)routers[i]);
        if (rc) {
            perror("Unable to create thread\n");
            exit(-1);
        }
    }
    pthread_t thread;
    if (strncmp(argv[1],"-r",2) == 0) {
        printf("Matched -r\n");
        // Create router
        Router* receiver = new Router(5556, 2048);
        // Run receive_message on new thread
        int rc = pthread_create(&thread, NULL, run_receiver, (void*)receiver);
        if (rc) {
            perror("Unable to create thread\n");
            exit(-1);
        }
    } else if (strncmp(argv[1],"-s",2) == 0) {
        printf("Matched -s\n");
        Router* sender = new Router(4444, 2048);
        run_sender(sender, htonl(0x7f000001), 5555);
    } else {
        printf("Usage: -r for receiver or -s for sender\n");
    }

    // Loop menu
    while (1) {
        cout << "Enter command:" << endl;
        cout << "Usage:\n \'L\' to list routers\n \'M\' to send a message" << endl;
        char input;
        cin >> input;
        // Handle input
        switch (input) {
            // List the routers
            case 'L':{
                         for (int i = 0; i < router_count; i++) {
                             cout << "Router " << i << " on " << routers[i]->port << endl;  
                         }
                         break;
                     }
                     // Send a message from source to destination router
            case 'M':{
                         cout << "Enter source router: " << endl;
                         int sender;
                         cin >> sender;
                         cout << "Enter destination router: " << endl; 
                         int receiver;
                         cin >> receiver;
                         // Stop the sender from listening
                         pthread_cancel(&thread[sender]); // TODO: should I be calling this?
                         int destination_port = routers[receiver]->port;
                         unsigned long destination_addr = routers[receiver]->addr;
                         routers[sender]->find_path(destination_addr, destination_port);
                         break;
                     }
            default:{
                        cout << "Invalid usage!" << endl;
                        cout << "Usage:\n \'L\' to list routers\n \'M\' to send a message" << endl;
                    }
        }
    }
}
