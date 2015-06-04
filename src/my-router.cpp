// Source file for router
// Nicolas Langley 904433991
#include <cstring>
#include <fstream>
#include <sstream>
#include <iterator>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "my-router.h"

using namespace std;

// Mutex for thread printing
mutex Router::mtx;

// Static thread_print function
void Router::thread_print(string input)
{
    Router::mtx.lock();
    cout << input;
    Router::mtx.unlock();
}

/****************************
 * Constructor sets port value and buffer size
 * Creates socket on localhost (127.0.0.1) on given port 
 * **************************/
Router::Router(int port, int buf_size, vector<Tuple>& data) : buffer_size(buf_size), port(port)
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

    stringstream ss;
    ss << "Address of router on port " << port << " is " << addr << endl;

    // Attempt to bind the socket to port
    int bind_result = bind(sock_fd, (struct sockaddr*)&router_addr, sizeof(router_addr));
    if (bind_result < 0) {
        perror("Bind failed!");
    }

    ss << "Adding neighbours for " << port << endl;
    //set up neighbors 
    for (int i = 0; i < data.size(); i++) {
        if (port == data[i].src_port){
            ss << "Neighbour is " << data[i].dest_port << endl;
            tableEntryRouting entry;
            entry.sequence = 1; 
            entry.destination_ip = data[i].dest_port; 
            entry.next_ip = data[i].dest_port;
            entry.hop_count = 1;
            entry.is_neighbor = true;
            routing_table[data[i].dest_port] = entry;
            //routing_table.insert(pair<unsigned long, tableEntryRouting>(data[i].src_port, entry));
        }
    }
    Router::thread_print(ss.str());
}

// Parse topology string from file
Router::tableEntryRouting Router::delimitTopology(std::string str)
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

    stringstream ss;
    //ss << "Target IP address is " << serv_addr.sin_addr.s_addr << " on port " << dest_port << endl;
    Router::thread_print(ss.str());

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
    stringstream ss;
    for (;;) {
        ss.str("");
        //ss << "Waiting for message on port: " << port << endl;
        Router::thread_print(ss.str());
        receive_len = recvfrom(sock_fd, buffer, buffer_size, 0, (struct sockaddr*)&remote_addr, &addr_length);
        if (receive_len > 0) {
            buffer[receive_len] = 0;
            ss << "---------" << endl;
            ss << "Message Received at " << port << endl;
            ss << "---------" << endl;
            ss << "Received " << receive_len << " bytes" << endl;
            ss << "Text data received: " << buffer << endl;
            char* message = (char*)(&buffer);
            int message_type = message[0] - '0'; // TODO: this is not checked
            ss << "AODV message type is " << message_type << endl;
            ss << "---------" << endl;
            Router::thread_print(ss.str());
            if (message_type == 1) {
                //cout << "Received REQ" << endl;
                // Create a new AODV request message and load serialized data
                AODVRequest* req_message = new AODVRequest();
                req_message->deserialize(message);
                handle_request(req_message);
            } else if (message_type == 2) {
                //cout << "Received RREP" << endl;
                // Create a new AODV response message and load serialized data
                AODVRequest* res_message = new AODVRequest();
                res_message->deserialize(message);
                handle_response(res_message);
            } 
			else if (message_type == 4) {   //forward message to next port
				vector <string> values;    //values[0] = message_type 
				int commaCnt = 0;  	//values[1] = dest_port    values[2] = actual data    
				int c = 0;
				while (message[c] != '\0') {
					if (message[c] != ',' || commaCnt >= 3) 
						values[commaCnt] += message[c];
					else if (commaCnt < 3)
						commaCnt++; 
					c++; 
				}
				char const* finalPortC = values[2].c_str();   
				unsigned long finalPortNum = (unsigned long)atoi(portC);   //final node 
				tableEntryRouting tmp = routing_table[finalPortNum];    //look up node
				string finalPortandData = values[2];      
				finalPortandData += values[3];    //send final port and data together

				char* message = new char[finalPortandData.size() + 1];
				copy(str.begin(), str.end(), message);
				message[finalPortandData.size()] = '\0'; 
				
				//send_data (address, next node, final node, data) 
				send_data_text(htonl(0x7f000001), tmp.next_ip, message); 
							//ip             next router    final port value, and data
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
 * Simple wrapper around send_message() that loads a text message 
 * **************************/
void Router::send_data_text(unsigned long addr, int port, char* text)
{
    // Append data type (4) and port value to message

    stringstream ss;
    ss << "4" << "," << port << "," << text;
    
    string str = ss.str();
    char* message = new char[str.size() + 1];
    copy(str.begin(), str.end(), message);
    message[str.size()] = '\0';
    // Send data over UDP
    send_message(addr, port, message);
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

void Router::find_path(unsigned long dest, int dest_port)
{
    stringstream ss;
    ss << "---------" << endl;
    ss << "find_path()" << endl;
    ss << "---------" << endl;
    ss << "Finding best path to router at " << dest << " and " << dest_port << endl;
    // TODO(Michael): create AODV message and send it to neighbours
    map<unsigned long, tableEntryRouting>::iterator it;
    ss << "Number of neighbours: " << routing_table.size() << endl;
    for (it = routing_table.begin(); it != routing_table.end(); it++) {
        if (it->second.is_neighbor) {
            ss << "Sending to neighbour on port " << it->first << endl;
            // We have found a neighbour
            AODVRequest* req_message = new AODVRequest(port,dest_port,1,port,it->first, false);
            ss << "DEBUG: " << req_message->serialize() << endl;
            ss << "---------" << endl << endl;
            Router::thread_print(ss.str());
            // TODO: fix the port and address stuff
            
            // Adding initial message to cache table 
            pair<unsigned long,unsigned long> incomingRequestKey = make_pair(req_message->originator_ip,req_message->destination_ip);
            tableEntryCache originCacheEntry;
            originCacheEntry.destination_ip = req_message->destination_ip;
            originCacheEntry.source_ip = req_message->originator_ip;
            originCacheEntry.sequence = req_message->originator_sequence_number;  //sequence number of source
            originCacheEntry.hop_count = req_message->hop_count;

            cache_table[incomingRequestKey]=originCacheEntry;
            // NOTE: it->first is the port value
            send_aodv(htonl(0x7f000001), it->first, req_message);
        }
    }
}

void Router::handle_request(AODVRequest* req)
{
    stringstream ss;
    ss << "---------" << endl;
    ss << "Handling request at " << port << endl;
    ss << req->serialize() << endl;
    ss << "---------" << endl;
    
    //cout << "Printing cache table begin at " << port << endl;
    // ss << print_cache_table();
    //logic needed to handle if the receiving node is the final destination

    //check cache_table to see if message has already been handled
    pair<unsigned long,unsigned long> incomingRequestKey = make_pair(req->originator_ip,req->destination_ip);

    bool destination_in_routing_table;

    // do something if RREQ is not in the cache_table
    if (cache_table.find(incomingRequestKey) == cache_table.end())
    {
        ss << "REQ not in cache table at " << port << endl;        
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
        // else if its already in the routing_table compare the hop_counts
        // if new hop_count is less, then overwrite old and pass on 



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


        bool is_rrep;
        bool is_dest;
        bool is_in_table;

        if(req->destination_reached == true)
            is_rrep = true;
        else
            is_rrep = false;

        if(port == req->destination_ip)
            is_dest = true;
        else
            is_dest = false;

        if(destination_in_routing_table == true)
            is_in_table = true;
        else
            is_in_table = false;

        if (is_dest && is_rrep)
        {
            ss << print_routing_table();
            //      Route complete.
        }
        else if (is_dest && !is_rrep)
        {
            //      else set turnaround_flag = true
            //      and issue RREQ with switched originator and destination. 
            //      Generate request flip origin and destination
            ss << "Generate request turnaround" << endl;
            AODVRequest* req_message = new AODVRequest(req->destination_ip,req->originator_ip,1,port,req->sender_ip,true);       
            ss << req_message->serialize() << endl;
            send_aodv(htonl(0x7f000001), req->sender_ip, req_message);
        }
        else if (!is_dest && is_rrep) //don return all destinations will be in routing tables
        {
            ss << "RREP forwarding back to origin" << endl;
            tableEntryRouting destination_table_entry = routing_table[req->destination_ip];
            AODVRequest* req_message = new AODVRequest(req->originator_ip,req->destination_ip,req->hop_count+1,port,destination_table_entry.next_ip,true);
            ss << req_message->serialize() << endl;
            send_aodv(htonl(0x7f000001), destination_table_entry.next_ip, req_message);
        }
        else if (!is_dest && is_in_table && !is_rrep)
        {
            ss << "Forwarding using routing table" << endl;

            //      add to tables and retransmit only to the destination->next
            //      follow routing_table to next
            tableEntryRouting destination_table_entry = routing_table[req->destination_ip];
            AODVRequest* req_message = new AODVRequest(req->originator_ip,req->destination_ip,req->hop_count+1,port,destination_table_entry.next_ip,false);
            ss << req_message->serialize() << endl;
            send_aodv(htonl(0x7f000001), destination_table_entry.next_ip, req_message);
        }
        else if (!is_dest && !is_in_table)
        {        
            //      standard replication
            //      add to tables and retransmit to all neighbors
            //      for(each neighbor in neighborlist){send AODVRequest()}

            //      Generate request for each neighbor
            map<unsigned long, tableEntryRouting>::iterator it;
            ss << "Propogating REQ" << endl;
            ss << "Number of neighbours: " << routing_table.size() << endl;

            for (it = routing_table.begin(); it != routing_table.end(); it++) {
                if (it->second.is_neighbor) {
                    ss << "Sending to neighbour on port " << it->first << endl;
                    // We have found a neighbour
                    AODVRequest* req_message = new AODVRequest(req->originator_ip,
                                                               req->destination_ip,
                                                               1,
                                                               port,
                                                               it->first,
                                                               false);
                    ss << "DEBUG: " << req_message->serialize() << endl;
                    ss << "---------" << endl << endl;
                    // TODO: fix the port and portess stuff
                    // NOTE: it->first is the port value
                    send_aodv(htonl(0x7f000001), it->first, req_message);
                }
            }
        }        

        //cout << "Printing cache table end at " << port << endl;
        //ss << print_cache_table();
    }
    else
    {

        //if(req->hop_count < routing_table[originator_ip].hop_count)
        //{
        //    
        //}
        // compare rreq to cacheTableEntry and choose if hop_count lesser then 
    }
    Router::thread_print(ss.str());
}

string Router::print_routing_table()
{
    stringstream ss;
    ss << "\nRouting Table\n";
    
    map<unsigned long, tableEntryRouting>::iterator it;
    for(it = routing_table.begin(); it != routing_table.end(); ++it)
    {
        ss << "| key : " << it->first << " | destination_ip : " << it->second.destination_ip << " |  next_ip : " << it->second.next_ip <<
         " | sequence : " << it->second.sequence << " | hop_count : " << it->second.hop_count << " | is_neighbor : " << it->second.is_neighbor <<
        " |\n";
    }
    return ss.str();
}

string Router::print_cache_table()
{

    stringstream ss;
    ss << "\nCache Table\n";
    
    map<pair<unsigned long, unsigned long>, tableEntryCache>::iterator it;
    for(it = cache_table.begin(); it != cache_table.end(); ++it)
    {
        ss.str("");
        ss << "| key : (" << it->first.first << "," << it->first.second << ") | destination_ip : " << it->second.destination_ip << " |  source_ip : " << it->second.source_ip <<
        " | sequence : " << it->second.sequence << " | hop_count : " << it->second.hop_count <<
        " |\n";
    }
    return ss.str();

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



















