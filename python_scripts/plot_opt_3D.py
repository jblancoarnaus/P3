import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from mpl_toolkits.mplot3d import Axes3D

plt.rcParams['figure.dpi'] = 170
data = np.loadtxt('global_opt_out.txt')

k0 = data[:, 0]
k1 = data[:, 1]
k2 = data[:, 2]
x = data[:, 3]


k1cycle = 21
k2cycle = 21
k0cycle = k1cycle*k2cycle

Y = k2[0:k2cycle]
X = np.zeros(k1cycle)
Z = np.zeros((k1cycle, k2cycle))

for i in range(k1cycle):
  X[i] = k1[i*k2cycle]  #obtain one k1 per k2 cycle
  Z[i] = x[i*k2cycle:(i+1)*k2cycle]

Xgrid, Ygrid = np.meshgrid(X,Y)
fig = plt.figure()
ax = fig.gca(projection='3d') 
ax.view_init(30, 20)  #choose angle to view the graph (elevation, azimuth) in degrees

ax.set_xlim(0, 1)
ax.set_ylim(0, 1)

max_value = np.amax(x[0:k0cycle])
max_index = np.argmax(x)
xmax = k2[max_index]
ymax = k1[max_index]

#print(Z)
#print(X)
#print(Y)
#add marker on maxima
#ax.scatter3D(xmax,ymax,max_value,s=500,c='b') 
ax.plot_surface(Xgrid, Ygrid, Z,cmap=cm.coolwarm)
fig.tight_layout()

print("\t\tMax value = "+str(max_value)+"\t k1 = "+str(ymax)+"\t k2 = "+str(xmax));
ax.set_zlabel("%")
ax.set_xlabel("k2")
ax.set_ylabel("k1")
plt.title("TOTAL (%) vs k1 and k2")
plt.show()