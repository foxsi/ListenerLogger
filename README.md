# ListenerLogger

A command-line application for logging incoming datagrams. Currently, only UDP is supported.

## Usage
Try:
```
$ etherlogger --help
```
to see all the options. BUT I WILL WRITE MORE INFO HERE LATER!

## Examples


## Dependencies
- cmake
- boost/asio
- boost/program_options
- boost/date_time

## Flow
When called, things happen in this order:
1. Try to open a local socket to the supplied IP address and port. If none are supplied, the default is IP is 127.0.0.1 and the default port is 9999. 
2. Try to bind to the socket. The operating system will throw an exception if the IP and port pair cannot be opened or bound.
3. Try to open the log file. If no file name is provided, a new file will be created in this directory with name `UDPLog_YYYY-MM-DD HH:MM:SS.log`.
4. Listen for incoming packets and write them to the log file.