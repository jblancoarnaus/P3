import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
plt.rcParams['figure.dpi'] = 170
data = np.loadtxt('opt_k1.txt')
x = data[:, 0]
y = data[:, 1]


data_mean = np.array(y).mean()
res = "{0:0.2f}".format(data_mean)
data_mean = np.full(y.size, data_mean)

max_value = np.amax(y)
max_index = np.argmax(y)
print("\t\tMax value = "+str(max_value)+"\t k1 = "+str(x[max_index ]))
plt.plot(x,y)
plt.title("percentage over iterations to see maximum ")
plt.xlabel("k1")
plt.ylabel("percentage (%)")
plt.show()