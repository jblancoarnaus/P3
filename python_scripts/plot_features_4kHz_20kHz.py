import numpy as np
from scipy.io import wavfile
import matplotlib.pyplot as plt
import scipy.signal as sig
plt.rcParams['figure.dpi'] = 150

data20khz = np.loadtxt('out20khz.txt')
data4khz = np.loadtxt('out4khz.txt')

pot_20kHz = data20khz[:,0]
r1norm_20kHz = data20khz[:,1]
rmaxnorm_20kHz = data20khz[:,2]

pot_4kHz = data4khz[:,0]
r1norm_4kHz = data4khz[:,1]
rmaxnorm_4kHz = data4khz[:,2]

fig, axs = plt.subplots(3)

plt.suptitle("Features in 20kHz and 4kHz", y=1.02)
axs[0].plot(pot_20kHz, 'r-', linewidth=0.2, label ="power 20khz")
axs[0].plot(pot_4kHz, 'b-', linewidth=0.2, label ="power 4khz")

axs[1].plot(r1norm_20kHz, 'r-', linewidth=0.2, label ="r1norm 20khz")
axs[1].plot(r1norm_4kHz, 'b-', linewidth=0.2, label ="r1norm 4khz")

axs[2].plot(rmaxnorm_20kHz, 'r-', linewidth=0.2, label ="r1maxnorm 20khz")
axs[2].plot(rmaxnorm_4kHz, 'b-', linewidth=0.2, label ="r1maxnorm 4khz")

axs[0].set_ylabel("Amplitude")
axs[0].set_xlabel("Samples")

axs[1].set_ylabel("Amplitude")
axs[1].set_xlabel("Samples")

axs[2].set_ylabel("Amplitude")
axs[2].set_xlabel("Samples")


axs[0].legend(loc='upper right', prop={'size': 6})
axs[1].legend(loc='upper right', prop={'size': 6})
axs[2].legend(loc='upper right', prop={'size': 6})

plt.tight_layout()

plt.show()