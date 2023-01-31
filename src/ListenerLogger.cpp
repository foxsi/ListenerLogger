#include "ListenerLogger.h"
#include "Configuration.h"

#include <vector>
#include <sstream>
// #include <boost/date_time/posix_time/posix_time.hpp>

UDPListenerLogger::UDPListenerLogger(boost::asio::io_context& io_context, boost::program_options::variables_map vm): socket_(io_context) {
    
    // configure object based on variables_map
    configure(vm, io_context);
        // inside, opens file_ and tries to bind to socket_ constructed from address and port
    
    // do_logging();
}

UDPListenerLogger::~UDPListenerLogger() {
    // generate summary statistics, if requested
    if(do_summary){
        // maybe read back the file? get stats

    } else {
        std::cout<<"all done\n";
    }
    
    // close file, if it was opened in the first place
    if(file_.is_open()){
        print_verbose_("closing log file and exiting\n");
        file_.close();
    } else {
        print_verbose_("file never opened, exiting\n");
    }
    exit(0);
}

void UDPListenerLogger::configure(boost::program_options::variables_map vm, boost::asio::io_context& io_context) {
     // notify vm just in case:
     boost::program_options::notify(vm);

    if(vm.count("help")){
        // cout stuff
        std::cout << HELP_MESSAGE;
        exit(0);
    }
    if(vm.count("version")){
        std::string version = std::to_string(VERSION_MAJOR) 
            + "." + std::to_string(VERSION_MINOR)
            + "." + std::to_string(VERSION_REVISION);
        std::cout << version + "\n";
        exit(0);
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
        std::cerr << "Exception: " << e.what() << "\n";
        exit(1);
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
        exit(1);
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
        print_verbose_("will end long after ");
        print_verbose_(std::to_string(max_packets));
        print_verbose_(" seconds\n");
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

// std::wstring format_time_(boost::posix_time::ptime now) {
//     static std::locale loc(std::wcout.getloc(), new boost::posix_time::wtime_facet(L"%Y-%m-%d_%H-%M-%S"));

//     std::basic_stringstream<wchar_t> wss;
//     wss.imbue(loc);
//     wss << now;
//     return wss.str();
// }

void UDPListenerLogger::print_verbose_(std::string text) {
    if(do_verbose) {
        std::cout << text;
    }
}