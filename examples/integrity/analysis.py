import sys, getpass
import numpy as np
import matplotlib.pyplot as plt
from fabric import Connection

local_log_filename = "integrity_test.log"

summary = True

local_ip = "192.168.1.100"
local_port = 9999
remote_ip = "192.168.1.8"
remote_port = 9999
trigger_phrase = "send_data"

line = "L"
timestamp = "T"
timeout = 10            # seconds
numpacks = 10000000000  # max number packets to receive
bitlength = 16          # length of word to write

remote_proc = "./switch/integrity/bin/integrity" 
remote_proc += " -l " + str(bitlength) 
remote_proc += " -t " + trigger_phrase
remote_proc += " --local-ip " + remote_ip
remote_proc += " --local-port " + str(remote_port)
remote_proc += " --remote-ip " + local_ip
remote_proc += " --remote-port " + str(local_port)

local_proc = "./bin/listenerlogger"

if summary:
    local_proc = local_proc + " --summary"

local_proc += " --file " + local_log_filename
local_proc += " --local-ip " + local_ip
local_proc += " --local-port " + str(local_port)
local_proc += " --remote-ip " + remote_ip
local_proc += " --remote-port " + str(remote_port)
local_proc += " --line " + line
local_proc += " --timestamp " + timestamp
local_proc += " --timeout " + str(timeout)
local_proc += " --numpacks " + str(numpacks)
local_proc += " --message " + trigger_phrase

remote_pass = getpass.getpass(prompt="Enter remote system password: ")
conn = Connection(
    host="raspberrypi.local", 
    user="foxsi", 
    connect_kwargs={
        "password": remote_pass
    }
)

conn.run(
    remote_proc,
    # asynchronous=True,
    timeout=timeout,
    warn=True
)

print("on remote: " + remote_proc)
print("on local: " + local_proc)

# local_conn.local(local_proc)


print("analyzing...")
ints = []
counts = []
with open(local_log_filename) as file:
    for line in file:
        cleaned = line.rstrip()
        binary_string = cleaned.partition("-")[2].partition("-")[2]
        counts_string = cleaned.partition("-")[0]
        counts.append(int(counts_string))
        ints.append(int(binary_string, 2))

ints.sort()

diffs = np.diff(ints)
diffs = np.insert(diffs, 0, 0, axis=0)
oddities = [i for i in diffs if i != 1]

print("found " + str(len(oddities)) + " weirdos")

fig, axs = plt.subplots(1,2, figsize=(4,2))
axs[0].plot(counts, ints)
axs[1].plot(counts, diffs)

axs[0].set_xlabel("packet counter")
axs[0].set_ylabel("packet value")

axs[1].set_xlabel("packet counter")
axs[1].set_ylabel("packet value offset to next packet")

fig.suptitle("Packet log file info")
plt.show()