#include <stdlib.h>

class Router {
   
    // Properties:
    //  Stored DV
    //  Local link state
    //  Connection details - port, etc..
  public:
    int node_id; // ID for router
    int sock_fd; // Socket file descriptor
    int buffer_size; // Receive buffer size
    int port; 
    unsigned long addr; // IP address
   
    // Routing table for forwarding 
    //std::map<int, int> routing_table;

    // Constructor: sets link costs according to topology
    Router(int port, int buf_size);

    // Send and receive UDP messages
    void send_message(unsigned long addr, int port, char* contents);
    void receive_message();

    // Handle incoming distance vectors
    void distance_vector_algorithm();
    // void update_link_costs();
    // void update_dv();
    // void propogate_dv();
};
