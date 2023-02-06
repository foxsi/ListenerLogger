import numpy as np
import matplotlib.pyplot as plt

filename = "integrity_test.log"

ints = []
counts = []
with open(filename) as file:
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
