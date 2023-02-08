#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <limits>
// #include <bitset>
#include <boost/dynamic_bitset.hpp>

const std::string DEFAULT_TRIGGER = "please send me lots and lots of your best data";

const std::string REMOTE_IP = "192.168.1.100";
const unsigned short REMOTE_PORT = 9999;

const std::string LOCAL_IP = "192.168.1.8";
const unsigned short LOCAL_PORT = 9999;

const unsigned int BUFF_LEN = 2048;

// some helpful functions

// get nearest multiple of 2 to length.
unsigned short nearest_uint_length(unsigned short len) {
    const unsigned short multiple = 2;
    unsigned short result = ((len + multiple/2) / multiple) * multiple;
    return result;
}

// increment bitset
bool increment(boost::dynamic_bitset<>& bits) {
    for(int loop = 0; loop < bitset.count(); ++loop) {
        if ((bitset[loop] ^= 0x1) == 0x1){
            return false;
        }
    }
    return true;
}

// enumerate all bitstrings of length len, and send them
void enum_sender(
    const unsigned short len, 
    boost::asio::ip::udp::socket& socket, 
    boost::asio::ip::udp::endpoint endpoint) {
    
    unsigned long long out_counter = 0;

    bool no_overflow = true;
    boost::dynamic_bitset<> bits(len, 0);
    unsigned long long perms = 1LL << len;
    std::cout << "sending " << std::to_string(perms) << " packets\n";

    while(no_overflow) {
        
        std::string out;
        to_string(bits, out);
        const char* out_cstring = out.c_str();

        // now send it
        socket.send_to(boost::asio::buffer(out_cstring, std::strlen(out_cstring)), endpoint);
        no_overflow = increment(bits);
        
        out_counter++;
    }
    std::cout << "sent " << std::to_string(out_counter) << " packets\n";
}

int main(int argc, char* argv[]) {
    boost::program_options::options_description generic("Permitted options");
    generic.add_options()
        ("verbose,v",                                                           "print verbose messages")
        ("length,l",        boost::program_options::value<unsigned short>(),    "length of message to transmit (in bytes)")
        ("trigger,t",       boost::program_options::value<std::string>(),       "trigger string to start sending")
        ("local-ip,I",      boost::program_options::value<std::string>(),       "local IP to receive and bind")
        ("local-port,P",    boost::program_options::value<unsigned short>(),    "local port to receive and bind")
        ("remote-ip,i",     boost::program_options::value<std::string>(),       "remote IP to send to")
        ("remote-port,p",   boost::program_options::value<unsigned short>(),    "remote port to send to")
        ;
    
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(generic).run(), vm);
    boost::program_options::notify(vm);

    boost::asio::io_context io_context;

    // parameters
    unsigned short length = 0;              // length of packet to send, in bytes. 
    std::string trigger = DEFAULT_TRIGGER;  // message to trigger sending
    std::string local_ip = LOCAL_IP;
    std::string remote_ip = REMOTE_IP;
    unsigned short local_port = LOCAL_PORT;
    unsigned short remote_port = REMOTE_PORT;

    if(vm.count("length")) {
        length = vm["length"].as<unsigned short>();
        if(length == 0) {
            std::cout << "\nNeed to send nonzero-length packets.\n";
            return EXIT_FAILURE;
        }
        if(length > BUFF_LEN) {
            std::cout << "\nBuffer is only " << std::to_string(BUFF_LEN) << ", --length should be less\n";
        }
        length = nearest_uint_length(length);
        if(length == 0) {
            length = 32;
        }
        std::cout << "\nRounding message length to nearest multiple of 16b: " << std::to_string(length) << "\n";

    } else {
        std::cout << "\nNeed to provide some length parameter.\n";
        return EXIT_FAILURE;
    }
    if(vm.count("trigger")) {
        trigger = vm["trigger"].as<std::string>();
    }
    if(vm.count("local-ip")) {
        local_ip = vm["local-ip"].as<std::string>();
    } else {
        //
    }
    if(vm.count("local-port")) {
        local_port = vm["local-port"].as<unsigned short>();
    } else {
        //
    }
    if(vm.count("remote-ip")) {
        remote_ip = vm["remote-ip"].as<std::string>();
    } else {
        //
    }
    if(vm.count("remote-port")) {
        remote_port = vm["remote-port"].as<unsigned short>();
    } else {
        //
    }

    boost::asio::ip::address local_address = boost::asio::ip::make_address(local_ip);
    boost::asio::ip::address remote_address = boost::asio::ip::make_address(remote_ip);

    boost::asio::ip::udp::endpoint local_endpoint = boost::asio::ip::udp::endpoint(local_address, local_port);
    boost::asio::ip::udp::endpoint remote_endpoint = boost::asio::ip::udp::endpoint(remote_address, remote_port);

    boost::asio::ip::udp::socket local_socket(io_context);
    local_socket.open(boost::asio::ip::udp::v4());
    local_socket.bind(local_endpoint);

    char receive_buffer[BUFF_LEN];

    // notify remote_endpoint that this program has started
    std::string init_message = "integrity test ready to send data";
    const char* init_message_cstr = init_message.c_str();
    local_socket.send_to(boost::asio::buffer(init_message_cstr, std::strlen(init_message_cstr)), remote_endpoint);

    std::cout << "\nnotified remote of startup\n";

    std::size_t bytes_received = local_socket.receive_from(boost::asio::buffer(receive_buffer), remote_endpoint);

    std::string reply = std::string(receive_buffer);

    if(reply.compare(trigger) == 0) {
        std::cout << "sending gobs of data...\n";
        enum_sender(length, local_socket, remote_endpoint);
    } else {
        std::cout << "Received nonsense from remote. Namely, " << reply <<". Ending.\n";
        return EXIT_FAILURE;
    }

    // send remote "integrity test ready to send data"
    // recvfrom remote, check against TRIGGER
    // start sending

    // io_context.stop();
    // sleep(3);
    return 0;
}
