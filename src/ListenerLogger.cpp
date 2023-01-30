#include "ListenerLogger.h"

#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>

UDPListenerLogger::UDPListenerLogger(boost::asio::io_context& io_context, short port, std::string infilename, boost::program_options::variables_map vm): socket_(io_context, boost::asio::ip::udp::endpoint(address_, port_)) {
    
    // configure object based on variables_map
    configure(vm);
    
    // start io_context
    // open socket to port

    // try opening file, see if there's stuff in there
    file_.open(filename);

}

UDPListenerLogger::~UDPListenerLogger() {
    
    // generate summary statistics, if requested
    if(do_summary){

    } else {
        std::cout<<"all done\n";
    }
    
    // close file, if it was opened in the first place
    if(file_.is_open()){
        print_verbose_("\tclosing log file and exiting\n");
        file_.close();
    } else {
        print_verbose_("\tfile never opened, exiting\n");
    }
    exit(0);
}

void UDPListenerLogger::configure(boost::program_options::variables_map vm) {
     // notify vm just in case:
     boost::program_options::notify(vm);

    if(vm.count("help")){
        // cout stuff
        exit(0);
    }
    if(vm.count("verbose")){
        do_verbose = true;
        print_verbose_("\tverbose output on\n");
    }
    if(vm.count("summary")){
        do_summary = true;
        print_verbose_("\twill print a summary after\n");
    }
    if(vm.count("timeout")){
        std::vector<std::string> strvect_timeout = vm["timeout"].as<std::vector<std::string>>();
        if (strvect_timeout.size() > 0){
            timeout = std::stoul(strvect_timeout[0]);
            print_verbose_("\twill timeout after ");
            print_verbose_(strvect_timeout[0]);
            print_verbose_(" seconds\n");
        } else {
            // CERR
            std::cout << "no timeout time specified!\n";
            exit(1);
        }
    }
    if(vm.count("numpack")){
        // similar thing to timeout
    }
    if(vm.count("line")){
        print_verbose_("\tlogging with line numbers on\n");
    }
    if(vm.count("time")){
        print_verbose_("\tlogging with time stamping on\n");
    }
    if(vm.count("ip")){

        print_verbose_("\tconnecting to IP address ");
        // then print the address
    }
    if(vm.count("port")){

        print_verbose_("\tconnecting to port ");
        // then print the port
    }
    if(vm.count("file")){
        // try to open the file
        std::vector<std::string> strvect_filename = vm["file"].as<std::vector<std::string>>();
        filename = strvect_filename[0];

    } else {
        // make a new file, name it
        // current time, 1-s resolution
        boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
        boost::gregorian::date today = now.date();
        print_verbose_(boost::gregorian::to_simple_string(today));

        std::string prefix = "UDPlog_";
        std::string suffix = ".log";

        filename = prefix + boost::gregorian::to_simple_string(today) + suffix;

    }



    bool file_exists = std::filesystem::exists(filename);
}



void UDPListenerLogger::print_verbose_(std::string text) {
    if(do_verbose) {
        std::cout << text;
    }
}