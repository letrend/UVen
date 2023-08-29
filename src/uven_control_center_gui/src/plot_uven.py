import numpy as np
import matplotlib.pyplot as plt
import csv
import sys

time_stamp = []
temp = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]
target_current = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]
current = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]
gate = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]

print("reading " + sys.argv[1])

first = True

with open(sys.argv[1]) as csvfile:
    # has_header = csv.Sniffer().has_header(csvfile.read(1024))
    # csvfile.seek(0)  # Rewind.
    reader = csv.reader(csvfile)
    # if has_header:
    next(reader)  # Skip header row
    for row in reader: # each row is a list
        time_stamp.append(float(row[0]))
        for i in range(0,17):
            temp[i].append(float(row[1+i]))
        for i in range(0,16):
            target_current[i].append(float(row[1+17+i]))
            current[i].append(float(row[1 + 17 + 16 + i]))
            gate[i].append(float(row[1 + 17 + 16 + 16 + i]))

print(len(time_stamp))
t0 = time_stamp[0]
for i in range(0,len(time_stamp)):
    time_stamp[i] = (time_stamp[i]-t0)/1000

while time_stamp[0]<=float(sys.argv[2]):
    time_stamp.pop(0)
    for i in range(0,16):
        temp[i].pop(0)
        target_current[i].pop(0)
        current[i].pop(0)
        gate[i].pop(0)
    temp[16].pop(0)

for i in range(0,len(time_stamp)):
    if time_stamp[i]>float(sys.argv[3]):
        time_stamp = time_stamp[:i]
        for j in range(0, 16):
            temp[j] = temp[j][:i]
            current[j] = current[j][:i]
            gate[j] = gate[j][:i]
        temp[16] = temp[16][:i]
        break
fig, axs = plt.subplots(4, sharex=True)

axs[0].set_title('LED currents')
for i in range(0,16):
    axs[0].plot(time_stamp, current[i])

axs[1].set_title('LED Temp')
for i in range(0,16):
    axs[1].plot(time_stamp, temp[i])

axs[2].set_title('LED gates')
for i in range(0,16):
    axs[2].plot(time_stamp, gate[i])

axs[3].set_title('driver Temp')
axs[3].plot(time_stamp, temp[16])


plt.xlabel("time[s]")
plt.ylabel("temp[C]")


plt.show()
