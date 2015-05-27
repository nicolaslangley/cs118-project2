#include "aodv_messages.h"

/****************************
 * Simple router class for AODV implementation 
 * **************************/
class Router {
  public:
    // Router properties
    // TODO: add routing table and local link state
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

    // TODO: add functions for handling incoming RREQ or RREP messages 
    void handle_request(AODVRequest* req);
    void handle_response(AODVResponse* res);
};
