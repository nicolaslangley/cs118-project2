#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <iostream>
#include <ostream>
#include <sstream>
#include <map>
#include <arpa/inet.h>
#include "my-router.h"

using namespace std;


// Load node topology file
RouterData load_topology(string filename)
{
    //Node Topology: 
    string tuple;  
    vector<string> topology;     //holds each line of topology file 
    ifstream tfile(filename.c_str());

    int cnt = 0;
    //separate the topology file into a vector of strings, each string is one line in the file 
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
    //parse each line of the topology file 
    RouterData edges; 
    map<string, int> nodePort_pair;    //holds all of the unique nodes in the graph (maps id & port #) 
    Tuple filler;   //filler to avoid segfaults for accessing out of range values in vector 
    filler.src_id = ""; filler.dest_id = ""; filler.src_port=0; filler.dest_port=0; filler.linkCost=0; 
    for (int i = 0; i < cnt; i++) {
        edges.nodeInfo.push_back(filler);
        string input = topology[i]; 
        istringstream ss(input); 
        int itr = 0;
        string token;           //use stringstream read comma delimited string
        int portNum; 
        while(getline(ss, token, ',')) {
            if (itr == 0) {   //read source name from file 
                edges.nodeInfo[i].src_id = token;  
            }
            else if (itr == 1) {   //read destination name from file 
                edges.nodeInfo[i].dest_id = token; 
            }
            else if (itr == 2) {   //read destination port info from file 
                string tmp = token; 
                char const* cstr = tmp.c_str(); 
                edges.nodeInfo[i].dest_port = atoi(cstr);
                nodePort_pair.insert(pair<string, int>(edges.nodeInfo[i].dest_id, edges.nodeInfo[i].dest_port));  //give map all possible pairings of node/port 
            }
            else if (itr == 3) {  //read link cost from file 
                string tmp = token; 
                char const* cstr = tmp.c_str(); 
                edges.nodeInfo[i].linkCost = atoi(cstr);   //necessary b/c cannot assume link cost less than 10
            }			
            itr++; 
        }
    }
    typedef std::map<string, int>::iterator it_type; 
    for (it_type it = nodePort_pair.begin(); it != nodePort_pair.end(); ++it) {
        edges.portList.push_back(it->second);    //list of unique ports 
    }
    for (int i = 0; i < edges.nodeInfo.size(); i++) {
        edges.nodeInfo[i].src_port = nodePort_pair[edges.nodeInfo[i].src_id]; 
    }
    return edges; 	
}

// Run receive_message function in thread
void* run_receiver(void* threadarg)
{
    Router* receiver = (Router*)threadarg;
    receiver->receive_message();
}

int main(int argc, char* argv[])
{
    // Stringstream for printing
    stringstream ss;

    if (argc != 2) {
        cerr << "Incorrect usage!" << endl;
        cerr << "Usage: " << endl << "router.exe <topology file>" << endl;
        exit(-1);
    }

    // Load the topology file
    string topology_fname(argv[1]);
    RouterData data = load_topology(topology_fname);
    int router_count = data.portList.size();
    // TODO: make this a vector?
    map<int, Router*> routers_map;
    Router* routers[router_count];
    pthread_t threads[router_count];
    for (int i = 0; i < router_count; i++) {
        Router* router = new Router(data.portList[i], 2048, data.nodeInfo);
        routers_map[i] = router;
        // For each router set it to listen in a new thread
        int rc = pthread_create(&threads[i], NULL, run_receiver, (void*)router);
        if (rc) {
            perror("Unable to create thread\n");
            exit(-1);
        }
    }
    
    Router::display_menu = true;
    // Loop menu
    while (1) {
        if (1) {
            ss.str("");
            ss << "====================" << endl << "       MENU" << endl << "====================" << endl;
            ss << "Enter command:" << endl;
            ss << "Usage:" << endl;
            ss << "\'L\' to list routers" << endl;
            ss << "\'M\' to send a message" << endl;
            ss << "\'K\' to kill a router" << endl;
            ss << "\'P\' to print all routing tables" << endl;
            ss << "====================" << endl;
            Router::thread_print(ss.str());
            ss.str("");
        }

        char input;
        cin >> input;
        // Handle input
        switch (input) {
            // List the routers
            case 'L':{
                         map<int, Router*>::iterator it;
                         for (it = routers_map.begin(); it != routers_map.end(); it++) {
                             ss << "Router " << it->first << " on " << it->second->port << endl;
                             Router::thread_print(ss.str());
                             ss.str("");  
                         }
                         break;
                     }
                     // Send a message from source to destination router
            case 'M':{
                         ss << "---------" << endl;
                         ss << "Enter message to send: " << endl;
                         Router::thread_print(ss.str());
                         ss.str("");  
                         string message_input;
                         cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                         getline(cin, message_input);
                         ss << "Enter source router: " << endl;
                         Router::thread_print(ss.str());
                         ss.str("");
                         int sender;
                         cin >> sender;
                         ss << "Source port: " << routers_map[sender]->port << endl;
                         ss << "Enter destination router: " << endl; 
                         Router::thread_print(ss.str());
                         ss.str("");
                         int receiver;
                         cin >> receiver;
                         ss << "Destination port: " << routers_map[receiver]->port << endl;
                         ss << "---------" << endl << endl;
                         Router::thread_print(ss.str());

                         // Stop the sender from listening by killing thread
                         pthread_cancel(threads[sender]); 
                         int destination_port = routers_map[receiver]->port;
                         unsigned long destination_addr = routers_map[receiver]->addr;
                         
                         // Queue data to be sent after the path
                         char* data_message = new char[message_input.size() + 1];
                         copy(message_input.begin(), message_input.end(), data_message);
                         data_message[message_input.size()] = '\0';
                         routers_map[sender]->queued_message = data_message; 
                         routers_map[sender]->find_path(destination_addr, destination_port);

                         int rc = pthread_create(&threads[sender], NULL, run_receiver, (void*)routers_map[sender]);
                         if (rc) {
                             perror("Unable to create thread\n");
                             exit(-1);
                         }
                         break;
                     }
            case 'K':{
                         ss << "---------" << endl;
                         ss << "Enter router to kill:" << endl;
                         Router::thread_print(ss.str());
                         ss.str("");
                         int to_delete;
                         cin >> to_delete;
                         Router* td_router = routers_map[to_delete];
                         map<unsigned long, Router::tableEntryRouting>::iterator it;
                         for (it = td_router->routing_table.begin(); it != td_router->routing_table.end(); it++) {
                             if (it->second.is_neighbor) {
                                 map<int, Router*>::iterator it2;
                                 for (it2 = routers_map.begin(); it2 != routers_map.end(); it2++) {
                                     if (it2->second->port == it->second.destination_ip) {
                                        it2->second->routing_table.erase(td_router->port);
                                     } 
                                 }
                             }
                         }
                         routers_map.erase(to_delete);
                         break;
                     }
            case 'P':{
                         ss << "---------" << endl;
                         ss << "Printing routing tables..." << endl;
                         Router::thread_print(ss.str());
                         ss.str("");
                         map<int, Router*>::iterator it;
                         for (it = routers_map.begin(); it != routers_map.end(); it++) {
                             ss << "Router " << it->first << " on " << it->second->port << endl;
                             ss << it->second->print_routing_table() << endl;
                         }
                         Router::thread_print(ss.str());  
                         break;
                     }
            case 'D':{
                         // Convert string message to char*
                         //char* data_message = new char[message_input.size() + 1];
                         //copy(message_input.begin(), message_input.end(), data_message);
                         //data_message[message_input.size()] = '\0';
                         //
                         //routers_map[sender]->send_data_text(htonl(0x7f000001), routers_map[receiver]->port, data_message);
                         //break;
                     }
            default:{
                        ss << "Invalid usage!" << endl;
                        ss << "Usage:\n \'L\' to list routers\n \'M\' to send a message" << endl;
                    }
        }
    }
}
