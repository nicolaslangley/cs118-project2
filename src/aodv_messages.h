#include <sstream>
#include <vector>
#include <iostream>


using namespace std;
// Base class for AODV Messages
class AODVMessage
{
  public:
    int type; // 1 - request, 2 - response, 3 - error
    bool reserved;

    // Destination IP and Sequence number
    unsigned long destination_ip;
    int destination_sequence_num;

    virtual const char* serialize()
    {
        // TODO: should this be made abstract?
        return "Base";
    }

    virtual void deserialize(char* ser_data)
    {}
};

// Sent to request a path
class AODVRequest: public AODVMessage
{
  public:
    // Header flags
    bool join_flag;
    bool repair_flag;
    bool gratuitous_flag;
    bool destination_only_flag;
    bool unknown_sequence_number;

    int hop_count;
    int rreq_id;

    unsigned long originator_ip;
    int originator_sequence_number;

    AODVRequest(unsigned long orig_ip, unsigned long dest_ip): originator_ip(orig_ip)
    {
        type = 1;
        reserved = 0;
        destination_ip = dest_ip;
    }

    const char* serialize()
    {
        printf("Serializing...\n");
        // Store all of the values in a string
        stringstream ss;
        ss << type << "\n";
        ss << reserved << "\n";
        ss << destination_ip << "\n";
        ss << destination_sequence_num << "\n";
        ss << hop_count << "\n";
        ss << rreq_id << "\n";
        ss << originator_ip << "\n";
        ss << originator_sequence_number << "\n";
        printf("%s\n", ss.str().c_str());
        return ss.str().c_str();
    }

    void deserialize(char* ser_data)
    {
        // Parse stored values
        vector<string> values;
        istringstream ss(ser_data);
        while (!ss.eof())       
        {
          string x;               // here's a nice, empty string
          getline(ss, x, '\n' );  // try to read the next field into it
          cout << x << endl;      // print it out, EVEN IF WE ALREADY HIT EOF
          values.push_back(x);
        }
        istringstream(values[0]) >> type;
        istringstream(values[1]) >> reserved;
        istringstream(values[2]) >> destination_ip;
        istringstream(values[3]) >> destination_sequence_num;
        istringstream(values[4]) >> hop_count;
        istringstream(values[5]) >> rreq_id;
        istringstream(values[6]) >> originator_ip;
        istringstream(values[7]) >> originator_sequence_number;
    }
};

// Sent in response to a request if:
//      1 - current node is the destination
//      2 - current node has an active route to destination
class AODVResponse: public AODVMessage
{
    bool repair_flag;
    bool ack_required;

    // Next hop can be any node with same prefix as dest
    int prefix_size; 
    int hop_count;

    char* originator_ip;
    int lifetime; // In Milliseconds
};

class AODVError: public AODVMessage
{
    bool no_delete_flag;
    int dest_count; // Number of unreacheable destinations
};
