#ifndef LISTENERLOGGER_H
#define LISTENERLOGGER_H

#include <boost/program_options.hpp>
#include <boost/asio.hpp>

#include <iostream>
#include <fstream>

const extern std::size_t BUFF_SIZE = 4096;
extern std::string SENDER_IP;
extern short SENDER_PORT;

class UDPListenerLogger{
    public:
        UDPListenerLogger(
            boost::asio::io_context& io_context, 
            short port, 
            std::string infilename,
            boost::program_options::variables_map vm
        );
        
        ~UDPListenerLogger();

        int do_logging();

        void receive();
        void write();
        
        // unimplemented:
        void send();
        void read();

        void configure(boost::program_options::variables_map vm);

        bool do_verbose;
        bool do_timestamp;                                  // write timestamp to log
        bool do_packetnum;                                  // write packet numbers to log
        bool do_summary;                                    // summary statistics to console
        std::size_t max_packets;                            // end logging on max packet count
        unsigned long timeout;                              // timer to end logging

        char packet_delimiter;                              // packet delimiter
        char time_delimiter;                                // timestamp delimiter (after timestamp)
        char num_delimiter;                                 // message count delimiter (after message counter)

        std::string filename;

    private:

        std::ofstream file_;
        bool appending_;

        boost::asio::ip::address address_;
        short port_;

        boost::asio::ip::udp::socket socket_;
        boost::asio::ip::udp::endpoint sender_endpoint_;
        char last_data_[BUFF_SIZE];
        
        void print_verbose_(std::string text);

};

#endif