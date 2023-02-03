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

        int do_logging();

        void receive();
        void write();
        
        void send(const char* message);
        
        // unimplemented:
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
        std::ofstream file_;
        bool appending_;

        boost::asio::ip::address local_address_;
        short local_port_;

        boost::asio::ip::address remote_address_;
        short remote_port_;

        boost::asio::ip::udp::endpoint remote_endpoint_;

        boost::asio::ip::udp::endpoint local_endpoint_;
        boost::asio::ip::udp::socket local_socket_;

        char last_data_received_[BUFF_SIZE];
        std::size_t last_data_received_size_;

        unsigned long packets_received_;
        unsigned long packets_written_;
        
        void print_verbose_(std::string text);

};

#endif