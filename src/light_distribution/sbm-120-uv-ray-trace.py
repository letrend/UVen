import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator
from mpl_toolkits.mplot3d import Axes3D
from scipy import optimize
import math

class SBM_120_UV:
    poly = None
    def __init__(self):
        x = range(-90, 91, 10)
        y = [0.02, 0.05, 0.21, 0.43, 0.62, 0.68, 0.87, 0.95, 0.98, 1, 0.98, 0.95, 0.87, 0.68, 0.62, 0.43, 0.21, 0.05,
             0.02]
        p = np.polyfit(x, y, deg=6)
        self.poly = np.poly1d(p)

fig = plt.figure()
xx = []
yy = []
for i in range(-120,120,1):
    xx.append(i)
    yy.append(poly(i))

ax = fig.subplots()
ax.plot(x,y,'rx')
ax.plot(xx,yy,'b')

plt.show()
