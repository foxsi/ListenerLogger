#include "ListenerLogger.cpp"

#include <boost/program_options.hpp>
#include <boost/asio.hpp>
// #include <boost/program_options>
#include <iostream>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    // po::options_description generic("Permitted options");

    boost::program_options::options_description generic("Permitted options");
    generic.add_options()
        ("version",     "print version string")
        ("verbose,v",   "print version string")
        ("help,h",      "send help message")
        ("summary,s",   "print summary stats")
        ("timeout",     "set logging timeout in seconds")
        ("numpackets,n", "set max number of packets to log")
        ("config,c",    boost::program_options::value<std::vector<std::string>>(), "config file")
        ("file,f",    boost::program_options::value<std::vector<std::string>>(), "output file")
        ("line,l",      "include line numbers in log file")
        ("timestamp, t", "include timestamp in log file")
        ;
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(generic).run(), vm);
    po::notify(vm);

    // FIX THE INITIALIZER
    // UDPListenerLogger UDPLL = UDPListenerLogger();

    // if (vm.count("help")) {
    //     std::cout << "sorry, can't help you\n";
    //     // return 0;
    // } else {
    //     std::cout << "didn't ask for help\n";
    //     // return 0;
    // }

    // if (vm.count("version")) {
    //     std::cout << "etherlogger version 0.1\n";
    //     // return 0;
    // }

    // if (vm.count("verbose")) {
    //     std::cout << "lalalalalalalalalalalalalalalalalalala\n";
    //     std::cout << "btw, found " << std::to_string(argc) << " arguments\n";
    //     // return 0;
    // }

    return 0;
}