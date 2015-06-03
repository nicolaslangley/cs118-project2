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

void run_sender(Router* sender, unsigned int dest_addr, int dest_port)
{
    printf("Sending message to server at address %lu on port %d\n", htonl(dest_addr), dest_port);
    AODVRequest* req_message = new AODVRequest(htonl(sender->addr), htonl(dest_addr),1,htonl(sender->addr),htonl(sender->routerSequenceNumber));
    AODVResponse* res_message = new AODVResponse(htonl(sender->addr), htonl(dest_addr));
    char* serialized_message = res_message->serialize();
    sender->send_message(dest_addr, dest_port, serialized_message);
}

struct Tuple {
	string src_id;
	string dest_id; 
	int src_port; 
	int dest_port; 
	int linkCost; 
};

struct routerData {
	vector<Tuple> nodeInfo;   //includes a tuple for every edge in the graph 
	vector<int> portList;    //list of unique source ports 
}; 

routerData load_topology(string filename)
{
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
	routerData edges; 
	map<string, int> nodePort_pair;    //holds all of the unique nodes in the graph (maps id & port #) 
	for (int i = 0; i < cnt; i++) {
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

void* run_receiver(void* threadarg)
{
    Router* receiver = (Router*)threadarg;
    printf("Running receiver in thread...\n");
    receiver->receive_message();
}

int main(int argc, char* argv[])
{
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
        char input;
        cin >> input;
        // Handle input
        switch (input) {
            case 'M': cout << "M pressed" << endl; 
                      pthread_cancel(thread); // TODO: should I be calling this?
                      break;
            default: cout << "Invalid button" << endl;
        }
    }
}
