import numpy as np
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import scipy.optimize as opt
import matplotlib.cm as cm
inten_angle = [[-80,-70,-60,-50,-40,-30,-20,-10,0,10,20,30,40,50,60,70,80],
                [0,0.25,0.45,0.65,0.8,0.88,0.95,0.98,1,0.98,0.95,0.88,0.8,0.65,0.45,0.25,0]]

# plt.plot(inten_angle[0],inten_angle[1])
# plt.show()

doex = inten_angle[0]
doey = inten_angle[0]
doez = np.array(inten_angle[1])

def paraBolEqn(data,a,b,c,d):
    x,y = data
    return -(((x-b)/a)**2+((y-d)/c)**2)+1.0


data = np.vstack((doex,doey))
zdata = doez

popt,pcov=opt.curve_fit(paraBolEqn,np.vstack((doex,doey)),doez,p0=[1,1,1,1])

zs = np.array([])
xs = np.arange(-90,90,10)
ys = np.arange(-90,90,10)

for x in xs:
        for y in ys:
            # need to subtract the zero to center the surface
            zs=np.append(zs,paraBolEqn((x,y),popt[0],popt[1],popt[2],popt[3]))

Z = zs.reshape(len(xs), len(ys))

X, Y = np.meshgrid(xs, ys)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot_surface(X,Y,Z, rstride=1, cstride=1, cmap=cm.coolwarm,linewidth=0, antialiased=False)
ax.plot(doex,doey,doez,marker='o', markersize=15)
plt.show()