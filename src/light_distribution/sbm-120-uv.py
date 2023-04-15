import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator
from mpl_toolkits.mplot3d import Axes3D
from scipy import optimize
import math

def gauss(X,amplitude,offset,x0,y0,sigma_x,sigma_y):
    x, y = X
    return amplitude*np.exp(-((x-x0)**2/(2*sigma_x**2) + (y-y0)**2/(2*sigma_y**2)))

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Make data.
x = []
y = []
for i in range(-90,91,10):
    x.append(math.sin(i/180*math.pi))
    y.append(0)
for i in range(-90,91,10):
    y.append(math.sin(i/180*math.pi))
    x.append(0)
print(x)
z = [0.02, 0.05, 0.21, 0.43, 0.62, 0.68, 0.87, 0.95, 0.98, 1, 0.98, 0.95, 0.87, 0.68, 0.62, 0.43, 0.21, 0.05, 0.02,
     0.02, 0.05, 0.21, 0.43, 0.62, 0.68, 0.87, 0.95, 0.98, 1, 0.98, 0.95, 0.87, 0.68, 0.62, 0.43, 0.21, 0.05, 0.02]

initial_guess = (0,0,0,0,1,1)
params, pcov = optimize.curve_fit(gauss, (x,y), z,initial_guess)
print(params)

X = np.arange(-1, 1, 0.1)
Y = np.arange(-1, 1, 0.1)
X, Y = np.meshgrid(X, Y)
Z = gauss((X,Y),params[0],params[1],params[2],params[3],params[4],params[5])

# Plot the surface.
surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm, linewidth=0, antialiased=False)

# Customize the z axis.
ax.set_zlim(0, 1.01)
ax.zaxis.set_major_locator(LinearLocator(10))

ax.scatter(x, y, z, 'b', s=50)
# A StrMethodFormatter is used automatically
# ax.zaxis.set_major_formatter('{x:.02f}')

# Add a color bar which maps values to colors.
# fig.colorbar(surf, shrink=0.5, aspect=5)

plt.show()
