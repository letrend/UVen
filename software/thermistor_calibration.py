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

print(analogValues)
print(temp)
plt.plot(analogValues, temp, 'ro')
plt.show()
