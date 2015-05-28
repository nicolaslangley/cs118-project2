#include "aodv_messages.h"

/****************************
 * Simple router class for AODV implementation 
 * **************************/
class Router {
  public:
    // Router properties
    // TODO(Frank): add variables for local link state
    //              set these in functions to be implemented below

    // TODO(Michael): add routing table 
    int node_id; // ID for router
    int sock_fd; // Socket file descriptor
    int buffer_size; // Receive buffer size
    int port; 
    unsigned long addr; // IP address

    // Constructor: sets link costs according to topology
    Router(int port, int buf_size);

    // Send and receive AODV messages over UDP
    void send_message(unsigned long addr, int port, char* contents);
    void receive_message();

    // Wrappers for sending aodv messages and data
    void send_aodv(unsigned long addr, int port, AODVMessage* message);
    void send_data(unsigned long addr, int port, char* filename);

    // TODO(Frank): implement functions for loading and setting up topology
    //              define whatever functions you want

    // TODO(Michael): implement functions for handling incoming RREQ or RREP messages 
    void handle_request(AODVRequest* req);
    void handle_response(AODVResponse* res);
};
