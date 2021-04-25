import numpy as np
import matplotlib.pyplot as plt
plt.rcParams['figure.dpi'] = 170
data = np.loadtxt('global_opt_out.txt')
x = data[:, 0]
y = data[:, 3]


k1cycle = 21
k2cycle = 21
k0cycle = k1cycle*k2cycle

Z = np.zeros(30)
X = np.zeros(30)

for i in range(28):
  Z[i] = x[i*k0cycle+2]
  X[i] = y[i*k0cycle+2]
print(Z)
max_value = np.amax(y)
max_index = np.argmax(y)
print("\t\tMax value = "+str(max_value)+"\t k1 = "+str(x[max_index ]));
plt.plot(Z,X)
plt.title("k0 loop results ")
plt.xlabel("k0")
plt.ylabel("percentage (%)")
plt.show()