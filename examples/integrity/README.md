# Link integrity test

This folder contains a Python script some C++ that can be used to send gobs of structured data from a remote machine to the local machine running `./listenerlogger`. The `examples/integrity/CMakeLists.txt` file is set up to find `pthreads` for use on a Linux system. If you are building for macOS or Windows, you will have to modify it.

## Running this test

To run this test, do the following:

- On the remote machine (e.g. Formatter Raspberry Pi):
    1. Move `examples/integrity/CMakeLists.txt` and `examples/integrity/main.cpp` to some director `<dir>` on the remote machine. 
    2. Make a folder `build` inside `<dir>`.
    3. From the remote's command line, `$ cd build` and then `$ cmake ..` and then `$ make`.
    4. Run the executable generated at `<dir>/bin/integrity` like this: `$ ./<dir>/bin/integrity -l 64`. The command options will be explained later when I figure out what I'm doing with them.
    5. The remote machine is now ready to send lots of structured data to the local.
- On the local machine (e.g. GSE computer)
    1. Run `listenerlogger` with the config file in the integrity example folder (like this, from the project root directory: `% ./bin/listenerlogger --config examples/integrity/integrity_test.cfg`)
    2. At least when it was uploaded, that config is set up to read 65535 packets from the remote and terminate. This should take about 1 second.
    3. When that's done, run `examples/integrity/analysis.py` to get out a plot of the number of packets received vs. time, and a plot of the delta between received packet values. The second plot should start at zero, then be a constant line at 1.0.

Example results look like this:
![Error-free link test](../assets/feb6-2023-better_integrity_test_data.png?raw=true "Link test without error")

In the future, I would like to set this up so that the Python script runs everything for you, then checks results. Maybe using [Fabric](https://docs.fabfile.org/en/stable/getting-started.html) to run stuff on the remote machine.

## More info

This test checks that all sent packets are delivered intact to the receiver. UDP does not guarantee this (unlike TCP), so we need to check packet drop rates at a given packet size and throughput rate. To do this, I send ASCII strings encoding binary words of a given length. The user supplies the length of the binary string. Then, the sender enumerates and sends all possible words (in lexicographic order) for the provided length. Although UDP packets are not guaranteed to arrive in the order they are sent, the receiver can easily sort the received packets and check that all possible binary words of the set length are present.
