# ListenerLogger

A command-line application for logging incoming datagrams. Currently, only UDP is supported.

## Usage
Try:
```
% ./listenerlogger --help
```
to see all the options. Here's some info on the options:

| Command | Short version | Arguments | Description |
| --- | --- | --- | --- |
| `--help`      | `-h`  |           | Print help message and exit. |
| `--version`   |       |           | Print version number and exit. |
| `--verbose`   | `-v`  |           | Turn verbose output messages on. |
| `--summary`   | `-s`  |           | Print summary statistics from transfer on exit. |
| `--config`    | `-c`  | `<path to config file>` | Read these options from an [INI-style](https://en.wikipedia.org/wiki/INI_file) config file instead of typing them all in here. Note: do not use INI headers in the file. Strings will be used as is---only put quotes if you want quotes in the command argument you're passing. |
| `--local-ip`  | `-I`  | `<IP address>` | Local machine's IP address, to monitor for incoming UDP packets. Defaults to loopback address `127.0.0.1`. |
| `--local-port`| `-P`  | `<port number>`| Port number on local machine to monitor for incoming UDP packets. Defaults to `9999`. Avoid assigning `0` to `1024`; they're reserved. |
| `--remote-ip`  | `-i`  | `<IP address>` | Remote machine's IP address, only used if sending `--message` on startup. Defaults to loopback address `127.0.0.1`. |
| `--remote-port`| `-p`  | `<port number>`| Remote machine's port number, only used if sending `--message` on startup. Defaults to `9999`. Avoid assigning `0` to `1024`; they're reserved. |
| `--message`   | `-m`  | `<message string>` | Message to send on startup to specified `--remote-ip` and `--remote-port`, sent before logging begins. Messages with spaces can be put in `--config` file, but not assigned in command prompt. |
| `--timeout`   | `-T`  | `<number of seconds>` | Stop logging packets and exit after specified number of seconds elapsed. Must be integer. |
| `--numpacks`  | `-n`  | `<number of packets>` | Stop logging packets and exit after specified number of packets has been written. Must be integer. |
| `--file`      | `-f`  | `<path to file>`  | Path to log file. If file already exists, will append to it. Will write in binary format, so prior encoding may be messed up. |
| `--line`      | `-l`  | `<delimiter string>` | Turn on line numbering in log file. Provide a string to separate the line number from the timestamp or packet contents. |
| `--timestamp` | `-t`  | `<delimiter string>` | Turn on timestamping in log file. Will timestamp machine's POSIX time, down to milliseconds. Provide a string to separate the timestamp from the packet contents. |

## Examples
Check out the [examples folder here](examples/README.md).

## Dependencies
- [cmake](https://cmake.org/cmake/help/latest/)
- [boost/asio](https://www.boost.org/doc/libs/1_81_0/doc/html/boost_asio.html)
- [boost/program_options](https://www.boost.org/doc/libs/1_81_0/doc/html/program_options.html)
- [boost/date_time](https://www.boost.org/doc/libs/1_81_0/doc/html/date_time.html)

## Getting started
### For macOS
The following takes place at the command prompt: `% `. Assuming you're starting from scratch and need to build an executable on your system, do this:
1. If you recently updated macOS, run `% xcode-select --install`. This will give you git back after your update. This command will error (good!) if you already have the tools installed. 
2. Do you have a package manager installed? You can get [homebrew here](https://brew.sh/) or [MacPorts here](https://www.macports.org/).
3. Use your package manager to install cmake. For example, `% brew install cmake`.
4. Use your package manager to install Boost. For example, `% brew install boost`.
5. In a folder you want to keep this project, `% git clone https://github.com/foxsi/ListenerLogger.git`. 
6. Now comes the tricky part. You may use some IDE (VS code, Eclipse, Xcode, whatever) for development. You need to show that thing where the Boost library lives, so that when you try to compile it understands. In VScode, I have a setting called `includePath` that will get searched for libraries when I try to build. Anyways, for homebrew at least, Boost is located by default at `/opt/homebrew/Cellar/boost/<version number>/include`. So you should tell your IDE about that path (or the equivalent for your system).
7. To build an executable, make a folder called `build` inside ListenerLogger (the downloaded git repository). Then, to build the executable inside `ListenerLogger/bin` do: 

```
% cd build
% cmake ..
% make
```

8. Now you can run the executable (from the folder `ListenerLogger`) with

```
% ./bin/listenerlogger --<options> [arguments]
```

9. If you don't like typing in so many arguments every time you run, you can also run like this:

```
% ./bin/listenerlogger --config path/to/config/file
```

where the config file (like [this one](https://github.com/foxsi/ListenerLogger/blob/main/sample_config.cfg)) stores values of each argument you want to set. So you can put things like:

```
verbose=true
file=my_log_file.log
```
to turn verbose output on and set the path to the output log file.

## Flow

When called, things happen in this order:
1. Try to open a local socket to the supplied IP address and port. If none are supplied, the default is IP is 127.0.0.1 and the default port is 9999. 
2. Try to bind to the socket. The operating system will throw an exception if the IP and port pair cannot be opened or bound.
3. Try to open the log file. If no file name is provided, a new file will be created in this directory with name `UDPLog_YYYY-MM-DD HH:MM:SS.log`.
4. Listen for incoming packets and write them to the log file.

## Networking notes

- Two machines are connected by an Ethernet cable. 
    - There is no USB converter inline. The Ethernet cable goes from RJ45 to RJ45.
- Each machine has an IP address that you can find with `% ifconfig` or System Preferences or `$ hostname -I` (or a dozen other options).
- On a given machine, a socket should `open` and `bind` to its own IP address. Then, it should `send_to` or `receive_from` a remote IP address. This is for UDP. For TCP, a local socket bound to the local machine IP address can then `connect` to the remote IP.
- Things are easier if wifi is disabled on both the local and remote machines (GSE and Formatter).
- Things are easier if neither machine is connected to the broader internet. 
- So that both machines can talk to each other over Ethernet, configure their network settings like this:
    - Set static IP addresses on both; some local network like 192.168.1.XXX is good. 
    - Set subnet mask on both to 255.255.255.0
    - Set router on both to 192.168.1.254
    - Disable WAN/wifi interface
- If it is necessary for the GSE-like machine to connect to the internet over Ethernet (not the Raspberry Pi though), do this:
    - In System Preferences/Network, add an Ethernet service (name it what you want) with the IP/subnet/router/DNS/etc settings for your internet service.
    - Then, to prioritize _this_ interface for internet stuff, on macOS click the three dots on the System Preferences/Network settings page and select "Set Service Order...". Put the internet Ethernet service you just set up first (before the Ethernet service for talkint to the remote machine with static local IP).