/****************************
 * Base class for AODV Messages 
 * **************************/
class AODVMessage
{
    public:
        // Common AODV message header properties
        int type; // 1 - request, 2 - response, 3 - error
        bool reserved;
        unsigned long destination_ip;  //this is the final destination, not the recipient of the rreq
        int destination_sequence_num;

        // Virtual serialization functions
        virtual char* serialize() = 0;
        virtual void deserialize(char* ser_data) = 0;
};

/****************************
 * AODV message sent to request a path 
 * **************************/
class AODVRequest: public AODVMessage
{
    public:
        // Message header properties
        bool join_flag;
        bool repair_flag;
        bool gratuitous_flag;
        bool destination_only_flag;
        bool unknown_sequence_number;
        int hop_count;
        int rreq_id;
        unsigned long originator_ip;
        int originator_sequence_number;

        bool destination_reached;
        unsigned long sender_ip;   //this can be derived from the source field of the header
        unsigned long recipient_ip;

        // Constructors
        AODVRequest(); // Default used when deserializing
        // Takes the following:
        //      orig_ip   : origin IP address
        //      dest_ip   : destination IP address
        //      send_ip   : current sender IP address
        //      rec_ip    : next hop IP address
        //      dest_rchd : RREP flag
        AODVRequest(unsigned long orig_ip, unsigned long dest_ip, int hop_ct, unsigned long send_ip, unsigned long rec_ip, bool dest_rchd);

        bool timeElapsed();

        // Serialization functions
        char* serialize();
        void deserialize(char* ser_data);
};

/****************************
 * AODV message sent in response to a request if:
 *      1 - current node is the destination
 *      2 - current node has an active route to destination
 * **************************/
class AODVResponse: public AODVMessage
{
    public:
        // Message header properties
        bool repair_flag;
        bool ack_required;
        int prefix_size; // Next hop can be any node with same prefix as dest
        int hop_count;
        unsigned long originator_ip;
        int lifetime; // In Milliseconds

        unsigned long sender_ip;

        // Constructors
        AODVResponse(); // Default used when deserializing
        AODVResponse(unsigned long orig_ip, unsigned long dest_ip);

        // Serialization functions
        char* serialize();
        void deserialize(char* ser_data);
};

/****************************
 * AODV message sent due to error in transmission 
 * **************************/
class AODVError: public AODVMessage
{
public:
    unsigned long destination_ip;
    unsigned long originator_ip;
    AODVError();
    AODVError(unsigned long orig_ip, unsigned long dest_ip);
    // Serialization functions
    char* serialize();
    void deserialize(char* ser_data);
};

class AODVAck: public AODVMessage
{
public:
    unsigned long destination_ip;
    unsigned long originator_ip;
    AODVAck();
    AODVAck(unsigned long orig_ip, unsigned long dest_ip);
    char* serialize();
    void deserialize(char* ser_data);
};









