#include "aodv_messages.h"
#include <map>

using namespace std;

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

    struct tableEntryRouting{
        int sequence;
        unsigned long destination_ip;
        unsigned long next_ip;
        int hopCount;
    };

    struct tableEntryCache{
        int sequence;
        unsigned long destination_ip;
        unsigned long source_ip;
        int hopCount;

    // bool operator==(const tableEntryCache& a) const{
    //     if(destination_ip == a.destination_ip && source_ip == a.source_ip)
    //         return true;
    //     else
    //         return false;
    //     }

    // int operator<(const tableEntryCache& a) const{
    //     return destination_ip < a.destination_ip;
    //     }

    };

    map< pair<unsigned long, unsigned long>, tableEntryCache> reqCacheTable;  //key is source_ip,destination_ip
    map<unsigned long, tableEntryRouting> routingTable;

    // Constructor: sets link costs according to topology
    Router(int port, int buf_size);

    // Send and receive AODV messages over UDP
    void send_message(unsigned long addr, int port, char* contents);
    void receive_message();

    // TODO: add functions for handling incoming RREQ or RREP messages 
    void handle_request(AODVRequest* req);
    void handle_response(AODVResponse* res);



};
