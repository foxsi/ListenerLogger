#include "ListenerLogger.h"

#include <boost/program_options.hpp>
#include <boost/asio.hpp>
// #include <boost/program_options>
#include <iostream>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {

    // all the input options at the command line
    boost::program_options::options_description generic("Permitted options");
    generic.add_options()
        ("version",         "print version string")
        ("verbose,v",       "print version string")
        ("help,h",          "send help message")
        ("summary,s",       "print summary stats")
        ("local-ip,I",      boost::program_options::value<std::string>(),   "local IP address to receive from")
        ("remote-ip,i",     boost::program_options::value<std::string>(),   "remote IP address to send to")
        ("local-port,P",    boost::program_options::value<unsigned short>(),"local port to log from")
        ("remote-port,p",   boost::program_options::value<unsigned short>(),"remote port to send to")
        ("message,m",       boost::program_options::value<std::string>(),   "startup message")
        ("timeout,T",       boost::program_options::value<unsigned long>(), "set logging timeout in seconds")
        ("numpacks,n",      boost::program_options::value<std::size_t>(),   "set max number of packets to log")
        ("config,c",        boost::program_options::value<std::string>(),   "config file")
        ("file,f",          boost::program_options::value<std::string>(),   "output file")
        ("line,l",          boost::program_options::value<std::string>(),   "include line numbers in log file")
        ("timestamp,t",     boost::program_options::value<std::string>(),   "include timestamp in log file")
        ;
    
    // make a variables_map to store and parse command line input
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(generic).run(), vm);
    po::notify(vm);

    // set up the I/O context for async UDP communication
    boost::asio::io_context io_context;
    
    try {
        // start the Logger object
        UDPListenerLogger UDPLL = UDPListenerLogger(io_context, vm);
        // start listening for UDP packets
        io_context.run();

    } catch(const std::exception& e) {
        std::cout << "std exception: " << e.what() << "\n";
        return EXIT_FAILURE;
    } catch(const char* error) {
        std::cout << "char exception: " << error << "\n";
        return EXIT_FAILURE;
    }

    return 0;
}