# ListenerLogger

A command-line application for logging incoming datagrams. Currently, only UDP is supported.

## Usage
Try:
```
$ etherlogger --help
```
to see all the options. BUT I WILL WRITE MORE INFO HERE LATER!

## Examples
I'll write some later on (maybe never)

## Dependencies
- [cmake](https://cmake.org/cmake/help/latest/)
- [boost/asio](https://www.boost.org/doc/libs/1_81_0/doc/html/boost_asio.html)
- [boost/program_options](https://www.boost.org/doc/libs/1_81_0/doc/html/program_options.html)
- [boost/date_time](https://www.boost.org/doc/libs/1_81_0/doc/html/date_time.html)

## Getting started (for development)
### For macOS
The following takes place at the command prompt: `% `. Assuming you're starting from scratch, do this:
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
````

8. Now you can run the executable (from the folder `ListenerLogger`) with

```
% ./bin/listenerlogger --<options> [arguments]
```

## Flow
When called, things happen in this order:
1. Try to open a local socket to the supplied IP address and port. If none are supplied, the default is IP is 127.0.0.1 and the default port is 9999. 
2. Try to bind to the socket. The operating system will throw an exception if the IP and port pair cannot be opened or bound.
3. Try to open the log file. If no file name is provided, a new file will be created in this directory with name `UDPLog_YYYY-MM-DD HH:MM:SS.log`.
4. Listen for incoming packets and write them to the log file.