
// Base class for AODV Messages
class AODVMessage
{
    int type; // 1 - request, 2 - response, 3 - error
    bool reserved;

    // Destination IP and Sequence number
    char* destination_ip;
    int destination_sequence_num;
}

class AODVRequest: AODVMessage
{
    // Header flags
    bool join_flag;
    bool repair_flag;
    bool gratuitous flag;
    bool destination_only_flag;
    bool unknown_sequence_number;

    int hop_count;
    int rreq_id;

    char* originator_ip;
    int originator_sequence_number;
}

class AODVResponse: AODVMessage
{
    bool repair_flag;
    bool ack_required;

    // Next hop can be any node with same prefix as dest
    int prefix_size; 
    int hop_count;

    char* originator_ip;
    int lifetime; // In Milliseconds
}

class AODVError: AODVMessage
{
    bool no_delete_flag;
    int dest_count; // Number of unreacheable destinations
}
