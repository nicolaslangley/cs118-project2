#include "aodv_messages.h"
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream> 
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct Tuple {
  string src_id;
  string dest_id;
  int src_port;
  int dest_port;
  int linkCost; 
};

struct RouterData {
  vector<Tuple> nodeInfo;   //includes tuple for every edge in graph
  vector<int> portList; 
};

/****************************
 * Simple router class for AODV implementation 
 * **************************/
class Router {
    public:
        // Router properties
        // TODO(Frank): add variables for local link state
        //              set these in functions to be implemented below

        int node_id; // ID for router
        int sock_fd; // Socket file descriptor
        int buffer_size; // Receive buffer size
        int port; 
        unsigned long addr; // IP address
        int routerSequenceNumber;

        struct tableEntryRouting{
            int sequence;
            unsigned long destination_ip;
            unsigned long next_ip;
            int hop_count;
            bool is_neighbor;
        };

        struct tableEntryCache{
            int sequence;
            unsigned long destination_ip;
            unsigned long source_ip;
            int hop_count;
        };

        // TODO: where is tableCacheEntry defined?
        map< pair<unsigned long, unsigned long>, tableEntryCache> cache_table;  //key is source_ip,destination_ip
        map<unsigned long, tableEntryRouting> routing_table;

        // Constructor: sets link costs according to topology
         Router(int port, int buf_size, vector<Tuple>& data);

        // Send and receive AODV messages over UDP
        void send_message(unsigned long addr, int port, char* contents);
        void receive_message();

        // Wrappers for sending aodv messages and data
        void send_aodv(unsigned long addr, int port, AODVMessage* message);
        void send_data(unsigned long addr, int port, char* filename);
        
        // Find path from current router to destination
        void find_path(unsigned long dest, int port);
        // TODO(Frank): implement functions for loading and setting up topology
        //              define whatever functions you want
        tableEntryRouting delimitTopology(std::string str);	
        // TODO(Michael): implement functions for handling incoming RREQ or RREP messages 
        void handle_request(AODVRequest* req);
        void handle_response(AODVRequest* res);
};
