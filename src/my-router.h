class Router {
   
    // Properties:
    //  Stored DV
    //  Local link state
    //  Connection details - port, etc..
  public:
    int sock_fd;
    int buffer_size;
    int port;
    // Address information
    unsigned long addr;

    // Constructor: sets link costs according to topology
    Router(int port, int buf_size);

    // Send and receive UDP messages
    void send_message(unsigned long addr);
    void receive_message();

    // Handle incoming distance vectors
    void distance_vector_algorithm();
    // void update_link_costs();
    // void update_dv();
    // void propogate_dv();
};
