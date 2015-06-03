#include "aodv_messages.h"
#include <sstream>
#include <vector>
#include <iostream>

using namespace std;

/********************
 * AODVRequest Functions
 * ******************/

/****************************
 * Default constructor that only sets the message type field
 * Use to re-construct a message using deserialization
 * **************************/
AODVRequest::AODVRequest()
{
    type = 1;
}

/****************************
 * Constructor sets both destination and originator IP addresses
 * Use to construct a request at the source
 * TODO: add values to be added upon initial request construction
 * **************************/
AODVRequest::AODVRequest(unsigned long orig_ip, unsigned long dest_ip, int hop_ct, unsigned long send_ip, unsigned long rec_ip, bool dest_rchd): originator_ip(orig_ip)
{
    type = 1;
    reserved = 0;
    destination_ip = dest_ip;
    recipient_ip = rec_ip;    
    sender_ip = send_ip;
    destination_reached = dest_rchd;
    hop_count = hop_ct;
}

/****************************
 * Serialize the data in the message
 * Returns a char* consisting of comma seperated values
 * TODO: add more values to serialization - if necessary 
 * **************************/
char* AODVRequest::serialize()
{
    printf("Serializing...\n");
    // Store all of the values in a string
    stringstream ss;
    ss << type << ",";
    ss << reserved << ",";
    ss << destination_ip << ",";
    //ss << destination_sequence_num << ",";
    ss << hop_count << ",";
    //ss << rreq_id << ",";
    ss << originator_ip << ",";
    //ss << originator_sequence_number;
    ss << recipient_ip << ",";
    ss << sender_ip << ",";
    ss << destination_reached << ",";
    string res = ss.str();
    char* result = new char[res.size() + 1];
    copy(res.begin(), res.end(), result);
    result[res.size()] = '\0';
    return result;
}

/****************************
 * Deserialize data and populate message fields 
 * Does not perform any checks on the input char* - trusts caller
 * TODO: set values according to what is serialized - make sure it is in sync 
 * **************************/
void AODVRequest::deserialize(char* ser_data)
{
    // Parse stored values
    vector<string> values;
    istringstream ss(ser_data);
    while (!ss.eof())       
    {
        string x; 
        getline(ss, x, ','); 
        values.push_back(x);
    }
    // Write values into message fields
    istringstream(values[0]) >> type;
    istringstream(values[1]) >> reserved;
    istringstream(values[2]) >> destination_ip;
    istringstream(values[3]) >> hop_count;
    istringstream(values[4]) >> originator_ip;
    istringstream(values[5]) >> recipient_ip;
    istringstream(values[6]) >> sender_ip;
    istringstream(values[7]) >> destination_reached;
}

/********************
 * AODVResponse Functions
 * ******************/

/****************************
 * Default constructor that only sets the message type field
 * Use to re-construct a message using deserialization
 * **************************/
AODVResponse::AODVResponse()
{
    type = 2;
}

/****************************
 * Constructor sets both destination and originator IP addresses
 * Use to construct a response at destination at the source
 * TODO: set values according to what is required 
 * **************************/
AODVResponse::AODVResponse(unsigned long orig_ip, unsigned long dest_ip): originator_ip(orig_ip)
{
    type = 2;
    reserved = 0;
    destination_ip = dest_ip;
}

/****************************
 * Serialize the data in the message
 * Returns a char* consisting of comma seperated values
 * TODO: add more values to serialization - if necessary 
 * **************************/
char* AODVResponse::serialize()
{
    printf("Serializing...\n");
    // Store all of the values in a string
    stringstream ss;
    ss << type << ",";
    ss << reserved << ",";
    ss << destination_ip << ",";
    ss << destination_sequence_num << ",";
    ss << ack_required << ",";
    ss << prefix_size << ",";
    ss << hop_count << ",";
    ss << originator_ip << ",";
    ss << lifetime ;
    string res = ss.str();
    // Write to char* output - non-const 
    char* result = new char[res.size() + 1];
    copy(res.begin(), res.end(), result);
    result[res.size()] = '\0';
    return result;
}

/****************************
 * Deserialize data and populate message fields 
 * Does not perform any checks on the input char* - trusts caller
 * TODO: set values according to what is serialized - make sure it is in sync 
 * **************************/
void AODVResponse::deserialize(char* ser_data)
{
    // Parse serialized values into vector
    vector<string> values;
    istringstream ss(ser_data);
    while (!ss.eof())       
    {
        string x; 
        getline(ss, x, ','); 
        values.push_back(x);
    }
    // Write values into message fields
    istringstream(values[0]) >> type;
    istringstream(values[1]) >> reserved;
    istringstream(values[2]) >> destination_ip;
    istringstream(values[3]) >> destination_sequence_num;
    istringstream(values[4]) >> ack_required;
    istringstream(values[5]) >> prefix_size;
    istringstream(values[6]) >> hop_count;
    istringstream(values[7]) >> originator_ip;
    istringstream(values[8]) >> lifetime;
}

bool timeElapsed(){

    // clock_t start = clock (); 
    // clock_t timeElapsed = ( clock() - start ) / CLOCKS_PER_SEC;
    return true; 
}

