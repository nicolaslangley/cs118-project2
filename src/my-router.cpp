// Source file for router
// Nicolas Langley 904433991
#include <cstring>
#include <fstream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "my-router.h"

using namespace std;

/****************************
 * Constructor sets port value and buffer size
 * Creates socket on localhost (127.0.0.1) on given port 
 * **************************/
Router::Router(int port, int buf_size) : buffer_size(buf_size), port(port)
{
    // Arguments are: (address_family, datagram_service, protocol) 
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("Cannot create socket!");
    }

    routerSequenceNumber = 1;

    // Bind the socket to a port number
    struct sockaddr_in router_addr;
    memset((char*)&router_addr, 0, sizeof(router_addr)); // Fill router_addr with 0s
    router_addr.sin_family = AF_INET; // Set the address family
    router_addr.sin_addr.s_addr = htonl(0x7f000001); // IP address is 127.0.0.1 (localhost)
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

// Parse topology string from file
tableEntryRouting Router::delimitTopology(std::string str)
{
	int itr = 0;
	std::string dest = "";
	int dest_port;
	int linkCost; 
	dest.append(str, 4, 5);  //where the dest_port starts/ends in topology
	char const* cstr = dest.c_str(); 
	dest_port = atoi(cstr);      //convert string to int
 	
	linkCost = str[10] - '0'; 
	//if the process of obtaining ip address is completed in send_message anyway, 
		//should the routing table just have ports? 
	struct sockaddr_in serv_addr;  //server address info 
	memset((char*)&serv_addr, 0, sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(dest_port);
	int addr_length = sizeof(addr_length);
	memcpy((void*)&serv_addr.sin_addr.s_addr, &serv_addr, addr_length);
	
	tableEntryRouting entry;
	entry.sequence = 0;
	entry.hop_count = linkCost; 
	entry.destination_ip = serv_addr.sin_addr.s_addr;
	entry.next_ip = serv_addr.sin_addr.s_addr;
	return entry; 
}


/****************************
 * Sends char* message to given address and port using UDP 
 * **************************/
void Router::send_message(unsigned long addr, int dest_port, char* contents)
{
    // Set up destination address information
    struct sockaddr_in serv_addr; // Server address info
    memset((char*)&serv_addr, 0, sizeof(serv_addr)); // Fill serv_addr with 0s
    serv_addr.sin_family = AF_INET; // Set the address family
    serv_addr.sin_port = htons(dest_port); // Set the port for the socket
    int addr_length = sizeof(addr); // Get length of address - IPv4 should be 4 bytes
    memcpy((void*)&serv_addr.sin_addr.s_addr, &addr, addr_length); // Set target server address

    printf("Target IP address is %d on port %d\n", serv_addr.sin_addr.s_addr, dest_port);

    // Send contents to destination 
    int sendto_result = sendto(sock_fd, contents, strlen(contents), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (sendto_result < 0) {
        perror("Sending to server failed!");
    }
}

/****************************
 * Listens on socket for incoming messages 
 * recvfrom() blocks while waiting for message 
 * TODO: parse incoming message contents
 * **************************/
void Router::receive_message()
{
    // Set up receiving properties
    struct sockaddr_in remote_addr; // Remote address info
    socklen_t addr_length = sizeof(remote_addr); // Length of addresses
    unsigned char buffer[buffer_size]; // Create the receive buffer
    int receive_len; // Number of bytes received

    // Listen on socket for incoming message
    for (;;) {
        printf("Waiting for message on port: %d\n", port);
        receive_len = recvfrom(sock_fd, buffer, buffer_size, 0, (struct sockaddr*)&remote_addr, &addr_length);
        if (receive_len > 0) {
            buffer[receive_len] = 0;
            printf("Received %d bytes\n", receive_len);
            printf("Text data received: %s\n", buffer);
            char* message = (char*)(&buffer);
            int message_type = buffer[0]; // TODO: this is not checked
            printf("AODV message type is: %c\n", buffer[0]);
            if (message_type == 1) {
                // Create a new AODV request message and load serialized data
                AODVRequest* req_message = new AODVRequest();
                req_message->deserialize(message);
                handle_request(req_message);
            } else if (message_type == 2) {
                // Create a new AODV response message and load serialized data
                AODVRequest* res_message = new AODVRequest();
                res_message->deserialize(message);
                handle_response(res_message);
            } else {
                // TODO: determine if message was an RERR or data
            }
        }
    }

}

/****************************
 * Simple wrapper around send_message() that takes AODVMessage instead of char* 
 * **************************/
void Router::send_aodv(unsigned long addr, int port, AODVMessage* message)
{
    char* serialized_message = message->serialize();
    send_message(addr, port, serialized_message);
}

/****************************
 * Simple wrapper around send_message() that loads a binary file 
 * **************************/
void Router::send_data(unsigned long addr, int port, char* filename)
{
    ifstream cur_file(filename, ios::in | ios::binary | ios::ate);

    ifstream::pos_type filesize;
    char* file_contents;

    // Load the lines of the file    
    if (cur_file.is_open()) {
        filesize = cur_file.tellg();
        file_contents = new char[filesize];
        cur_file.seekg(0, ios::beg);
        if (!cur_file.read(file_contents, filesize)) {
            perror("Failed to read file");
        }
        cur_file.close();
    } else {
        perror("Error opening file");
    }
    
    // Send data over UDP
    send_message(addr, port, file_contents);
}

void Router::find_path(unsigned long dest, int port)
{
    // TODO(Michael): create AODV message and send it to neighbours
}

void Router::handle_request(AODVRequest* req)
{
    //logic needed to handle if the receiving node is the final destination

    //check cache_table to see if message has already been handled
    pair<unsigned long,unsigned long> incomingRequestKey = make_pair(req->originator_ip,req->destination_ip);

    bool destination_in_routing_table;

    // do something if RREQ is not in the cache_table
    if (cache_table.find(incomingRequestKey) == cache_table.end())
    {         
            tableEntryCache reqCacheEntry;

            reqCacheEntry.destination_ip = req->destination_ip;
            reqCacheEntry.source_ip = req->originator_ip;
            reqCacheEntry.sequence = req->originator_sequence_number;  //sequence number of source
            reqCacheEntry.hop_count = req->hop_count;

            cache_table[incomingRequestKey]=reqCacheEntry;

        
        // do something if originator_ip not in the routing_table
        if (routing_table.find(req->originator_ip) == routing_table.end())
        {
            // sequence number functionality to be added later, if(req->destination_sequence_num > routing_table[req->sender_ip].sequence){ 
                tableEntryRouting previousNodeCumulative;
                
                previousNodeCumulative.destination_ip = req->originator_ip;
                previousNodeCumulative.next_ip = req->sender_ip;
                previousNodeCumulative.sequence = req->destination_sequence_num;
                previousNodeCumulative.hop_count = req->hop_count;        
                previousNodeCumulative.is_neighbor = false;

                routing_table[req->originator_ip]=previousNodeCumulative;
            // }
        }
        
        // do something if destination_ip is in the routing_table
        if (!(routing_table.find(req->destination_ip) == routing_table.end()))
        {
            destination_in_routing_table = true;
        }        
        else
        {
            destination_in_routing_table = false;
        }
        
        //  ---AODVRequest Generating Phase---

        //  AODVRequest(unsigned long orig_ip, unsigned long dest_ip, int hop_ct, unsigned long send_ip, unsigned long rec_ip, bool dest_fd);
 
        if      (addr == req->destination_ip && req->destination_reached == true)
        {}
        else if (addr == req->destination_ip && req->destination_reached == false)
        {
        //      else set turnaround_flag = true
        //      and issue RREQ with switched originator and destination. 
        //      AODVRequest(unsigned long orig_ip, unsigned long dest_ip, int hop_ct, unsigned long send_ip, unsigned long rec_ip, bool dest_fd);        
        //      Generate request flip origin and destination
                AODVRequest(req->destination_ip,req->originator_ip,0,addr,req->sender_ip,true);       
        }
        else if (addr != req->destination_ip && req->destination_reached == true) //don return all destinations will be in routing tables
        {
                tableEntryRouting destination_entry = routing_table[req->destination_ip];
                AODVRequest(req->originator_ip,req->destination_ip,req->hop_count+1,addr,destination_entry.next_ip,false);
        }
        else if (addr != req->destination_ip && destination_in_routing_table == true && destination_reached == false)
        {
        //      add to tables and retransmit only to the destination->next
        //      follow routing_table to next
                tableEntryRouting destination_entry = routing_table[req->destination_ip];
                AODVRequest(req->originator_ip,req->destination_ip,req->hop_count+1,addr,destination_entry.next_ip,false);
        }
        else if (addr != req->destination_ip && destination_in_routing_table == false)
        {        
        //      standard replication
        //      add to tables and retransmit to all neighbors
        //      for(each neighbor in neighborlist){send AODVRequest()}

        //      Generate request for each neighbor 
        
                int neighbor = 0;
                AODVRequest(req->originator_ip,req->destination_ip,req->hop_count+1,addr,neighbor,false);           

        }        

    }

}


void Router::handle_response(AODVRequest* res){}

    // //check cache_table to see if message has already been handled
    // pair<unsigned long,unsigned long> incomingResponseKey = make_pair(res->destination_ip,res->originator_ip);

    // if(cache_table.find(incomingResponseKey) == cache_table.end()){         
    //     //Do nothing, we've already responded to this RREQ
    // } else {
    //     // cache entry
    //         tableEntryCache reqCacheEntry;

    //         reqCacheEntry.destination_ip = res->originator_ip;  //in RREP the destination becomes the origin
    //         reqCacheEntry.source_ip = res->destination_ip;
    //         reqCacheEntry.sequence = res->destination_sequence_num;  //sequence number of destination
    //         reqCacheEntry.hop_count = res->hop_count;

    //         cache_table[incomingResponseKey]=reqCacheEntry;

    //     // 1st routing table entry
    //     if(routing_table.find(res->sender_ip) == routing_table.end())
    //     {
    //         if(res->originator_sequence_number > routing_table[res->sender_ip].sequence)
    //         {
    //             tableEntryRouting previousNode;
                
    //             previousNode.destination_ip = res->sender_ip;
    //             previousNode.next_ip = res->sender_ip;
    //             previousNode.sequence = res->destination_sequence_num;
    //             previousNode.hop_count = 1;

    //             routing_table[res->sender_ip] = previousNode;
    //         }

    //     }

    //     // 2nd routing table entry
    //     if(routing_table.find(res->originator_ip) == routing_table.end())
    //     {
    //         if(res->originator_sequence_number > routing_table[res->sender_ip].sequence)
    //         { 
    //             tableEntryRouting previousNodeCumulative;
                
    //             previousNodeCumulative.destination_ip = res->destination_ip;
    //             previousNodeCumulative.next_ip = res->sender_ip;
    //             previousNodeCumulative.sequence = res->destination_sequence_num;
    //             previousNodeCumulative.hop_count = (res->hop_count)+1;        

    //             routing_table[res->destination_ip]=previousNodeCumulative;
    //         }
        // }

        // if current != destination && destination_found == false
        //      add to tables and retransmit to all neighbors
        // if current != destination && destination_found == true
        //      add to tables and retransmit only to the destination->next

        // if current == destination && turnaround_flag == true, die. 
        // else set turnaround_flag = true
        // and issue RREQ with switched originator and destination. 
         

        //for(each neighbor in neighborlist){send AODVRequest()}
        //AODVRequest(unsigned long orig_ip, unsigned long dest_ip, int hop_ct, unsigned long send_ip, int dest_seq_num, bool dest_fd);
        // AODVRequest(res->originator_ip,res->destination_ip,res->hop_count,addr,res->destination_sequence_num,false);

    // }
        
// }



















