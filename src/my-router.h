
class Router {
   
    // Properties:
    //  Stored DV
    //  Local link state

    // Constructor: sets link costs according to topology

    // Send and receive UDP messages
    void send_message();
    void receive_message();

    // Handle incoming distance vectors
    void distance_vector_algorithm();
    // void update_link_costs();
    // void update_dv();
    // void propogate_dv();
}
