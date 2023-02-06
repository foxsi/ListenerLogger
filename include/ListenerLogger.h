#ifndef LISTENERLOGGER_H
#define LISTENERLOGGER_H

#include "Configuration.h"

#include <boost/program_options.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/asio.hpp>

#include <iostream>
#include <fstream>

extern std::string SENDER_IP;
extern short SENDER_PORT;

class UDPListenerLogger{
    public:
        UDPListenerLogger(
            boost::program_options::options_description desc,
            boost::program_options::variables_map vm,
            boost::asio::io_context& io_context
        );
        
        ~UDPListenerLogger();

        // unimplemented
        int do_logging();

        void receive();                                     // receive UDP datagrams asynchronously
        void write();                                       // write data to disk
        
        void send(const char* message);                     // send message to remote (address, port)
        
        // unimplemented
        void read(char* buffer);

        void configure(
            boost::program_options::options_description desc,
            boost::program_options::variables_map vm, 
            boost::asio::io_context& io_context
        );

        bool do_verbose;                                    // cout verbose status
        bool do_startup_message;                            // send a packet to remote_endpoint_ on startup
        bool do_timestamp;                                  // write timestamp to log
        bool do_packetnum;                                  // write packet numbers to log
        bool do_summary;                                    // summary statistics to console
        std::size_t max_packets;                            // end logging on max packet count
        unsigned long timeout;                              // timer to end logging

        std::string startup_message;                        // message to send to remote from consructor
        std::string packet_delimiter;                       // packet delimiter
        std::string time_delimiter;                         // timestamp delimiter (after timestamp)
        std::string num_delimiter;                          // message count delimiter (after message counter)

        std::string filename;                               // name/path of file to log to

    private:
        void print_verbose_(std::string text);              // verbose printing of text
        std::ofstream file_;                                // log file stream
        bool appending_;                                    // append to file or not
        char last_data_received_[BUFF_SIZE];                // buffer for incoming data
        std::size_t last_data_received_size_;               // size (# bytes) of last received UDP packet

        boost::asio::ip::address local_address_;            // IP address of local machine to bind to
        short local_port_;                                  // port # on local machine to bind to

        boost::asio::ip::address remote_address_;           // IP address of remote machine (if sending startup message)
        short remote_port_;                                 // port # on remote machine (if sending startup message)

        boost::asio::ip::udp::endpoint local_endpoint_;     // endpoint (address, port) pair for the local machine
        boost::asio::ip::udp::socket local_socket_;         // socket for the local machine to listen to (and send from)
        boost::asio::ip::udp::endpoint remote_endpoint_;    // endpoint (address, port) pair for remote machine (if startup message)

        unsigned long packets_received_;                    // total number of packets received while running
        unsigned long packets_written_;                     // total number of packets written to disk while running

        unsigned long bytes_received_;                      // total number of bytes received while running

        boost::posix_time::ptime start_time_;               // time receiving started
        boost::posix_time::ptime end_time_;                 // time writing ended

};

#endif