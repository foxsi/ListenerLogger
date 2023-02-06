# Examples

These are intended to demonstrate and probe capabilities and weaknesses of the ListenerLogger tool as we get it ready for flight use.

## integrity

This folder contains a Python script some C++ that can be used to send gobs of structured data from a remote machine to the local machine running `./listenerlogger`. The `examples/integrity/CMakeLists.txt` file is set up to find `pthreads` for use on a Linux system. If you are building for macOS or Windows, you will have to modify it.

To run this test, do the following:

1. On the remote machine (e.g. Formatter Raspberry Pi):
    1.1 Move `examples/integrity/CMakeLists.txt` and `examples/integrity/main.cpp` to some director `<dir>` on the remote machine. 
    1.2 Make a folder `build` inside `<dir>`.
    1.3 From the remote's command line, `$ cd build` and then `$ cmake ..` and then `$ make`.
    1.4 Run the executable generated at `<dir>/bin/integrity` like this: `$ ./<dir>/bin/integrity -l 64`. The command options will be explained later when I figure out what I'm doing with them.
    1.5 The remote machine is now ready to send lots of structured data to the local.
2. On the local machine (e.g. GSE computer)
    2.1 Run `listenerlogger` with the config file in the integrity example folder (like this, from the project root directory: `% ./bin/listenerlogger --config examples/integrity/integrity_test.cfg`)
    2.2 At least when it was uploaded, that config is set up to read 65535 packets from the remote and terminate. This should take about 1 second.
    2.3 When that's done, run `examples/integrity/analysis.py` to get out a plot of the number of packets received vs. time, and a plot of the delta between received packet values. The second plot should start at zero, then be a constant line at 1.0.

In the future, I would like to set this up so that the Python script runs everything for you, then checks results.