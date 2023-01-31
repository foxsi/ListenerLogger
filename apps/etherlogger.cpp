#include "ListenerLogger.h"

#include <boost/program_options.hpp>
#include <boost/asio.hpp>
// #include <boost/program_options>
#include <iostream>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    // po::options_description generic("Permitted options");

    boost::program_options::options_description generic("Permitted options");
    generic.add_options()
        ("version",         "print version string")
        ("verbose,v",       "print version string")
        ("help,h",          "send help message")
        ("summary,s",       "print summary stats")
        ("ip,I",            boost::program_options::value<std::string>(),   "IP address to log from")
        ("port,p",          boost::program_options::value<unsigned short>(),"port to log from")
        ("timeout,T",       boost::program_options::value<unsigned long>(), "set logging timeout in seconds")
        ("numpacks,n",    boost::program_options::value<std::size_t>(), "set max number of packets to log")
        ("config,c",        boost::program_options::value<std::string>(), "config file")
        ("file,f",          boost::program_options::value<std::string>(), "output file")
        ("line,l",          boost::program_options::value<std::string>(), "include line numbers in log file")
        ("timestamp,t",     boost::program_options::value<std::string>(), "include timestamp in log file")
        ;
    
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(generic).run(), vm);
    po::notify(vm);

    boost::asio::io_context io_context;

    // FIX THE INITIALIZER
    UDPListenerLogger UDPLL = UDPListenerLogger(io_context, vm);

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