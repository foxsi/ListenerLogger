#include "ListenerLogger.h"
#include "Configuration.h"

#include <vector>
#include <sstream>
#include <stdexcept>

// #include <boost/date_time/posix_time/posix_time.hpp>

UDPListenerLogger::UDPListenerLogger(boost::asio::io_context& io_context, boost::program_options::variables_map vm): socket_(io_context) {
    
    // configure object based on variables_map
    UDPListenerLogger::configure(vm, io_context);
        // inside, opens file_ and tries to bind to socket_ constructed from address and port
    
    packet_delimiter = "\n";
    packets_received_ = 0;
    packets_written_ = 0;

    UDPListenerLogger::receive();

    // do_logging();
}

UDPListenerLogger::~UDPListenerLogger() {
    // generate summary statistics, if requested
    if(do_summary){
        // maybe read back the file? get stats
        std::cout << "\nSummary:\n";
        std::cout << "\treceived " << std::to_string(packets_received_) << " packets\n";
        std::cout << "\twrote " << std::to_string(packets_written_) << " packets\n\n";

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

void UDPListenerLogger::configure(boost::program_options::variables_map vm, boost::asio::io_context& io_context) {
     // notify vm just in case:
     boost::program_options::notify(vm);

    if(vm.count("help")){
        // cout stuff
        std::cout << HELP_MESSAGE;
        throw "got help, ending";
    }
    if(vm.count("version")){
        std::string version = std::to_string(VERSION_MAJOR) 
            + "." + std::to_string(VERSION_MINOR)
            + "." + std::to_string(VERSION_REVISION);
        std::cout << version + "\n";
        throw "got version, ending";
    }
    if(vm.count("verbose")){
        do_verbose = true;
        print_verbose_("verbose output on\n");
    }

    std::string ip;
    if(vm.count("ip")){
        ip = vm["ip"].as<std::string>();
        print_verbose_("\nconnecting to IP address " + ip + "\n");
    } else {
        ip = DEFAULT_IP;
        std::cout << "\nno IP input, using default: " + DEFAULT_IP + "\n";
    }
    unsigned short port;
    if(vm.count("port")){
        port = vm["port"].as<unsigned short>();
        print_verbose_("connecting to port " + std::to_string(port) + "\n\n");
    } else {
        port = DEFAULT_PORT;
        std::cout << "no port number input, using default: " + std::to_string(DEFAULT_PORT) + "\n\n";
    }

    // try to open a socket to this local IP/port and bind to it
    address_ = boost::asio::ip::make_address(ip);
    sender_endpoint_ = boost::asio::ip::udp::endpoint(address_, port);
    // socket_ = boost::asio::ip::udp::socket(io_context, sender_endpoint_);
    
    try {
        socket_.open(boost::asio::ip::udp::v4());
        socket_.bind(sender_endpoint_);
    } catch (std::exception& e) {
        // std::cerr << "Exception: " << e.what() << "\n";
        throw e.what();
    }

    if(vm.count("file")){
        filename = vm["file"].as<std::string>();
    } else {
        // make a new file, name it
        // current time, 1-s resolution
        boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
        boost::gregorian::date today = now.date();

        // std::string now_string = format_time_(now);

        std::string now_string = boost::posix_time::to_simple_string(now);
        now_string.find(" ");

        std::string prefix = "UDPlog_";
        std::string suffix = ".log";

        filename = prefix + now_string + suffix;
        std::cout << "using filename: " + filename + "\n\n";
    }

    // check if the log file exists. 
    bool file_exists = std::filesystem::exists(filename);
    if(file_exists) {
        appending_ = true;
        file_.open(filename, std::ios::out | std::ios::app | std::ios::binary);
        print_verbose_("file already exists, opening\n");
    } else {
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

    // more CLI options:
    if(vm.count("timeout")){
        timeout = vm["timeout"].as<unsigned long>();
        print_verbose_("will timeout after ");
        print_verbose_(std::to_string(timeout));
        print_verbose_(" seconds\n");
    }
    if(vm.count("numpacks")){
        // similar thing to timeout
        max_packets = vm["numpacks"].as<std::size_t>();
        print_verbose_("will end logging after ");
        print_verbose_(std::to_string(max_packets));
        print_verbose_(" packets received \n");
    }
    if(vm.count("line")){
        do_packetnum = true;
        num_delimiter = vm["line"].as<std::string>();
        print_verbose_("logging with line numbers on\n");
    } else {
        do_packetnum = false;
    }
    if(vm.count("timestamp")){
        do_timestamp = true;
        time_delimiter = vm["timestamp"].as<std::string>();
        print_verbose_("logging with time stamping on\n");
    } else {
        do_timestamp = false;
    }

    if(vm.count("summary")){
        do_summary = true;
        print_verbose_("will print a summary after\n");
    } else {
        do_summary = false;
    }

    // global options
    packet_delimiter = "\n";
}

int UDPListenerLogger::do_logging() {
    while (packets_received_ <= max_packets) {
        receive();
        write();
    }
    return 0;
}

void UDPListenerLogger::receive() {
    socket_.async_receive_from(boost::asio::buffer(last_data_, BUFF_SIZE), sender_endpoint_, [this](boost::system::error_code ec, std::size_t bytes_received) {
        if (!ec && bytes_received > 0) {
            ++packets_received_;
            last_data_size_ = bytes_received;
            print_verbose_("got a packet of length " + std::to_string(last_data_size_) + "\n");
            write();
            if (packets_received_ >= max_packets) {
                throw "reached max packets";
            }
        } else {
            receive();
        }
    });
}

void UDPListenerLogger::write() {
    // write to file
    // if(do_packetnum) {
    //     if(do_timestamp) {
    //         file_ << "timestamp??" << time_delimiter << std::to_string(packets_received_) << packet_delimiter << last_data_;
    //     } else {
    //         file_ << std::to_string(packets_received_) << packet_delimiter << last_data_;
    //     }
    // } else {
    //     if(do_timestamp) {
    //         file_ << "timestamp??" << time_delimiter << last_data_;
    //     } else {
    //         file_ << last_data_;
    //     }
    // }
    // file_ << packet_delimiter;

    file_.write(last_data_, last_data_size_);
    memset(last_data_, 0, BUFF_SIZE);
    
    ++packets_written_;
    print_verbose_("wrote a packet\n");
    receive();
}

void UDPListenerLogger::print_verbose_(std::string text) {
    if(do_verbose) {
        std::cout << text;
    }
}