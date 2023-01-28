import numpy as np
import matplotlib.pyplot as plt
import csv

analogValues = [[],[],[]]
temp = []
with open("thermistor_calibration.csv") as csvfile:
    has_header = csv.Sniffer().has_header(csvfile.read(1024))
    csvfile.seek(0)  # Rewind.
    reader = csv.reader(csvfile,delimiter=',')
    if has_header:
        next(reader)  # Skip header row
    for row in reader: # each row is a list
        analogValues[0].append(float(row[0]))
        analogValues[1].append(float(row[1]))
        analogValues[2].append(float(row[2]))
        temp.append(float(row[3]))

z = []
order = 2
z.append(np.polyfit(analogValues[0], temp, order))
z.append(np.polyfit(analogValues[1], temp, order))
z.append(np.polyfit(analogValues[2], temp, order))
print(analogValues)
print(temp)
plt.plot(analogValues[0], temp, 'bo')
plt.plot(analogValues[1], temp, 'ro')
plt.plot(analogValues[2], temp, 'go')
xnew = np.linspace(200, 800, 100)
y = [[],[],[]]
for x in xnew:
    for s in range(0,3):
        val = 0
        poly = 0
        for zz in z[s]:
            val = val + zz*x**(len(z[s])-1-poly)
            poly = poly +1
        y[s].append(val)
    # y[0].append(z[0][0]*x**3+z[0][1]*x**2+z[0][2]*x+z[0][3])
    # y[1].append(z[1][0]*x**3+z[1][1]*x**2+z[1][2]*x+z[1][3])
    # y[2].append(z[2][0]*x**3+z[2][1]*x**2+z[2][2]*x+z[2][3])
plt.plot(xnew, y[0], 'b')
plt.plot(xnew, y[1], 'g')
plt.plot(xnew, y[2], 'r')

print("third order polynomial fit: ")
print(z)

plt.show()
