#include "ListenerLogger.h"
#include "Configuration.h"

#include <vector>
#include <sstream>
#include <stdexcept>
#include <boost/asio/buffer.hpp>
#include <boost/program_options/parsers.hpp>

UDPListenerLogger::UDPListenerLogger(boost::program_options::options_description desc, boost::program_options::variables_map vm, boost::asio::io_context& io_context): local_socket_(io_context) {
    
    // configure object based on variables_map
    UDPListenerLogger::configure(desc, vm, io_context);
        // inside, opens file_ and tries to bind to socket_ constructed from address and port
    
    // default packet delimiter CHANGE LATER
    packet_delimiter = "\n";
    // initialize tracking variables for summary info on exit
    packets_received_ = 0;
    packets_written_ = 0;
    bytes_received_ = 0;

    // send the startup message to remote machine, if doing that
    if(do_startup_message) {
        print_verbose_("sending startup message\n");
        UDPListenerLogger::send(startup_message.c_str());
    }

    // start timer, then start receiving and writing
    start_time_ = boost::posix_time::microsec_clock::local_time();
    UDPListenerLogger::receive();
}

UDPListenerLogger::~UDPListenerLogger() {
    // stop the timer
    end_time_ = boost::posix_time::microsec_clock::local_time();
    std::cout << "collected for " << (end_time_ - start_time_).total_seconds() << " seconds\n";

    // stop any async traffic on the socket:
    local_socket_.cancel();

    // generate summary statistics, if requested
    if(do_summary){
        // report some basic stats on collection and logging
        std::cout << "\nSummary:\n";
        std::cout << "\treceived " << std::to_string(packets_received_) << " packets\n";
        std::cout << "\twrote " << std::to_string(packets_written_) << " packets\n";
        std::cout << "\tcollected for " << (end_time_ - start_time_).total_seconds() << " seconds\n";
        std::cout << "\taverage datarate: " << 8*bytes_received_/(end_time_ - start_time_).total_microseconds() << "Mbps\n\n";

    } else {
        // std::cout << "all done\n";
    }
    
    // close file, if it was opened in the first place
    if(file_.is_open()){
        print_verbose_("closing log file and exiting\n");
        file_.close();
    } else {
        print_verbose_("file never opened, exiting\n");
    }
    // throw "destructor finished";
}

void UDPListenerLogger::configure(boost::program_options::options_description desc, boost::program_options::variables_map vm, boost::asio::io_context& io_context) {

    // check if there is a config file to use
    if(vm.count("config")) {
        print_verbose_("found config file\n");
        std::string config_filename = vm["config"].as<std::string>();
        boost::program_options::store(boost::program_options::parse_config_file(config_filename.c_str(), desc), vm);
    }
    // update the variables map
    boost::program_options::notify(vm);

    // if caller wants help, print it and exit.
    if(vm.count("help")) {
        // cout stuff
        std::cout << HELP_MESSAGE;
        throw "got help, ending";
    }

    // if caller wants version number, print it and exit.
    if(vm.count("version")) {
        std::string version = std::to_string(VERSION_MAJOR) 
            + "." + std::to_string(VERSION_MINOR)
            + "." + std::to_string(VERSION_REVISION);
        std::cout << version + "\n";
        throw "got version, ending";
    }

    // if caller wants verbose output, turn it on.
    if(vm.count("verbose")) {
        do_verbose = true;
        print_verbose_("verbose output on\n");
    }

    // check if caller provided local machine's IP address, else use default address.
    std::string ip;
    if(vm.count("local-ip")) {
        ip = vm["local-ip"].as<std::string>();
        print_verbose_("\nconnecting to local IP address " + ip + "\n");
    } else {
        ip = DEFAULT_IP;
        std::cout << "\nno local IP input, using default: " + DEFAULT_IP + "\n";
    }
    unsigned short port;
    if(vm.count("local-port")) {
        port = vm["local-port"].as<unsigned short>();
        print_verbose_("connecting to local port " + std::to_string(port) + "\n\n");
    } else {
        port = DEFAULT_PORT;
        std::cout << "no local port number input, using default: " + std::to_string(DEFAULT_PORT) + "\n\n";
    }

    // try to open a socket to this local IP/port and bind to it
    local_address_ = boost::asio::ip::make_address(ip);
    local_port_ = port;
    local_endpoint_ = boost::asio::ip::udp::endpoint(local_address_, local_port_);
    
    try {
        local_socket_.open(boost::asio::ip::udp::v4());
        local_socket_.bind(local_endpoint_);
    } catch (std::exception& e) {
        throw "couldn't open or bind local socket";
    }

    // if caller wants to send message to remote, set up remote (using default IP and port if none provided).
    if(vm.count("message")) {
        do_startup_message = true;
        startup_message = vm["message"].as<std::string>();

        if(vm.count("remote-ip")) {
            ip = vm["remote-ip"].as<std::string>();
            print_verbose_("\nconnecting to remote IP address " + ip + "\n");
        } else {
            ip = DEFAULT_IP;
            std::cout << "\nno remote IP remote, using default: " + DEFAULT_IP + "\n";
        }
        unsigned short port;
        if(vm.count("remote-port")) {
            port = vm["remote-port"].as<unsigned short>();
            print_verbose_("connecting to remote port " + std::to_string(port) + "\n\n");
        } else {
            port = DEFAULT_PORT;
            std::cout << "no remote port number input, using default: " + std::to_string(DEFAULT_PORT) + "\n\n";
        }

        // create remote endpoint to send to
        remote_address_ = boost::asio::ip::make_address(ip);
        remote_port_ = port;
        remote_endpoint_ = boost::asio::ip::udp::endpoint(remote_address_, remote_port_);
    } else {
        do_startup_message = false;
    }

    // if caller provided log file name, use it. Otherwise, make a new file with timestamp name.
    if(vm.count("file")) {
        filename = vm["file"].as<std::string>();
    } else {
        // make a new file, name it with current datetime
        boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
        boost::gregorian::date today = now.date();

        std::string now_string = boost::posix_time::to_simple_string(now);

        std::string prefix = "UDPlog_";
        std::string suffix = ".log";

        filename = prefix + now_string + suffix;
        std::cout << "using filename: " + filename + "\n\n";
    }

    // check if the log file exists. 
    bool file_exists = std::filesystem::exists(filename);
    if(file_exists) {
        // if it exists, append to it.
        appending_ = true;
        file_.open(filename, std::ios::out | std::ios::app | std::ios::binary);
        print_verbose_("file already exists, opening\n");
    } else {
        // if it doesn't exist, open it fresh.
        appending_ = false;
        file_.open(filename, std::ios::out  | std::ios::binary);
        print_verbose_("opening new file\n");
    }

    // check that log file opened correctly
    if(file_.is_open()) {
        print_verbose_("successfully opened file\n");
    } else {
        print_verbose_("failed to open file! Ending.\n");
        throw "couldn't open file";
    }

    // if caller wants application to time out, define timeout delay. 
    if(vm.count("timeout")) {
        timeout = vm["timeout"].as<unsigned long>();
        print_verbose_("will timeout after ");
        print_verbose_(std::to_string(timeout));
        print_verbose_(" seconds\n");
    } else {
        timeout = 0;
    }
    
    // if caller wants application to stop after some number of packets received, set that up.
    if(vm.count("numpacks")) {
        // similar thing to timeout
        max_packets = vm["numpacks"].as<std::size_t>();
        print_verbose_("will end logging after ");
        print_verbose_(std::to_string(max_packets));
        print_verbose_(" packets received \n");
    } else {
        max_packets = 0;
    }

    // if caller wants packet numbering turned on in log file, turn it on and delimit the number in the log file.
    if(vm.count("line")) {
        do_packetnum = true;
        num_delimiter = vm["line"].as<std::string>();
        print_verbose_("logging with line numbers on\n");
    } else {
        do_packetnum = false;
    }

    // if caller wants timestamping turned on in log file, turn it on and delimit the timestamp in the log file.
    if(vm.count("timestamp")) {
        do_timestamp = true;
        time_delimiter = vm["timestamp"].as<std::string>();
        print_verbose_("logging with time stamping on\n");
    } else {
        do_timestamp = false;
    }

    // if caller wants summary info printed after run, turn it on.
    if(vm.count("summary")) {
        do_summary = true;
        print_verbose_("will print a summary after\n");
    } else {
        do_summary = false;
    }
}

// don't use this
int UDPListenerLogger::do_logging() {
    // while (packets_received_ <= max_packets) {
        // receive();
        // write();
    // }
    return 0;
}

// receive data on local socket asynchronously. 
// AFAIK, the writing is part of the same asynchronous thread because it is called from this async context.
void UDPListenerLogger::receive() {
    local_socket_.async_receive_from(boost::asio::buffer(last_data_received_, BUFF_SIZE), local_endpoint_, [this](boost::system::error_code ec, std::size_t bytes_received) {
        if (!ec && bytes_received > 0) {
            // if this was a good receive, increment relevant counters
            ++packets_received_;
            bytes_received_ += bytes_received;
            last_data_received_size_ = bytes_received;
            print_verbose_("got a packet of length " + std::to_string(last_data_received_size_) + "\n");
            write();
        } else {
            receive();
        }
    });
}

// write new data to disk. This method checks for timeouts and packet number overruns to stop application.
void UDPListenerLogger::write() {
    
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    
    // probably a much faster way to do all this
    if(do_packetnum) {
        const char* packets_received_charray = std::to_string(packets_received_).c_str();
        file_.write(packets_received_charray, std::strlen(packets_received_charray));
        file_.write(num_delimiter.c_str(), std::strlen(num_delimiter.c_str()));
    }
    if(do_timestamp) {
        std::string timestamp_string = boost::posix_time::to_simple_string(now);
        const char* timestamp_charray = timestamp_string.c_str();
        file_.write(timestamp_charray, std::strlen(timestamp_charray));
        file_.write(time_delimiter.c_str(), std::strlen(time_delimiter.c_str()));
    }
    file_.write(last_data_received_, last_data_received_size_);
    
    memset(last_data_received_, 0, BUFF_SIZE);
    
    ++packets_written_;

    // max_packets = 0 and timeout = 0 indicate that no packet number or time limit have been specified.
    if(packets_received_ >= max_packets && max_packets > 0) {
        throw "reached max packets";
    }
    if((now - start_time_).total_seconds() > timeout && timeout > 0) {
        throw "reached timeout";
    }
    print_verbose_("wrote a packet\n");

    // go back to receiving data.
    receive();
}

// send message to remote
void UDPListenerLogger::send(const char* message) {
    local_socket_.send_to(
        boost::asio::buffer(message, std::strlen(message)), 
        remote_endpoint_
    );
    print_verbose_("sent startup message to remote\n");
}

// verbose printing method
void UDPListenerLogger::print_verbose_(std::string text) {
    if(do_verbose) {
        std::cout << text;
    }
}