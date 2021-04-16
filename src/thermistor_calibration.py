import numpy as np
import matplotlib.pyplot as plt
import csv

analogValues = []
temp = []
with open("thermistor_calibration.csv") as csvfile:
    has_header = csv.Sniffer().has_header(csvfile.read(1024))
    csvfile.seek(0)  # Rewind.
    reader = csv.reader(csvfile)
    if has_header:
        next(reader)  # Skip header row
    for row in reader: # each row is a list
        analogValues.append(float(row[0]))
        temp.append(float(row[1]))

z = np.polyfit(analogValues, temp, 3)
print(analogValues)
print(temp)
plt.plot(analogValues, temp, 'ro')
xnew = np.linspace(0, 1023, 100)
y = []
for x in xnew:
    y.append(z[0]*x**3+z[1]*x**2+z[2]*x+z[3])
plt.plot(xnew, y, 'g')

print("third order polynomial fit: ")
print(z)

plt.show()
