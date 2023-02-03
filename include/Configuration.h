#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <cstddef>
#include <string>

static const unsigned short VERSION_MAJOR = 0;
static const unsigned short VERSION_MINOR = 0;
static const unsigned short VERSION_REVISION = 1;

static const std::size_t BUFF_SIZE = 4096;

static const std::string DEFAULT_IP = "127.0.0.1";
static const unsigned short DEFAULT_PORT = 9999;

static const std::string HELP_MESSAGE = R"(usage: etherlogger [options]
    
    Log incoming UDP messages to file.
    
    General options:
        --help,         -h                  Display help message.
        --version                           Get software version.
        --verbose,      -v                  Verbose output on.
        --summary,      -s                  Print summary statistics on close.
        --config,       -c  <config/file>   Provide a config file instead of typing all these 
                                            options in at the command prompt.
    
    Connection options:
        --local-ip,     -I  <IP address>    IP address to monitor for UDP packets. This should be   
                                            the local machine's address.
        --local-port,   -P  <port number>   Port number to monitor for UDP packets. Avoid ports 
                                            0-1024.
        --remote-ip,    -i  <IP address>    If sending a packet on startup, provide the remote 
                                            IP address to send to.
        --remote-port,  -p  <port number>   If sending a packet on startup, provide the remote
                                            port to send to.
        --message,      -m  <messages>      Message to send on startup, before logging begins. If
                                            no --remote-ip or --remote-port are provided, default
                                            will be used.
        --timeout,      -T  <num seconds>   Stop logging and exit after <num seconds> seconds. 
        --numpacks,     -n  <num packets>   Stop logging and exit after <num packets> packets have  
                                            been received.

    File output options:
        --file,         -f  <file/path>     Path to file to log to. Contents may be overwritten.
        --line,         -l  <delimiter>     Number packets in log file. Provide a delimiter string 
                                            to separate packet number from packet or timestamp.
        --timestamp,    -t  <delimiter>     Timestamp packets in log file. Provide a delimiter
                                            string to separate timestamp from packet.

)";

#endif