import numpy as np
from scipy.io import wavfile
import matplotlib.pyplot as plt
plt.rcParams['figure.dpi'] = 200

samplerate_in, data_in = wavfile.read('rl001.wav')
our_pitch = np.loadtxt('our_pitch.f0')
wav_pitch = np.loadtxt('pitch_wavesurfer.txt')
wav_pitch = wav_pitch[:, 0]
plt.rcParams.update({'font.size': 6})

fig, axs = plt.subplots(3)
last_pos = len(data_in)
x_axis = (np.linspace(0,last_pos-1,last_pos))/fm #obtain x axis in seconds

#plot voice signal and pitchs
axs[0].plot(x_axis,data_in, 'r-', linewidth=0.2,label='Voice signal rl001')
axs[0].set_ylabel("Amplitude")

axs[1].plot(wav_pitch, 'D',c='blue',markersize = 0.5,linewidth=0.2,label='Wavesurfer`s pitch')
axs[1].set_ylabel("Pitch (Hz)")

axs[2].plot(our_pitch, '8',c= 'green',markersize = 0.5,linewidth=0.2,label='get_pitch`s pitch')
axs[2].set_ylabel("Pitch (Hz)")
fig.suptitle("Input signal vs pitch contours")

#display legends
axs[0].legend()
axs[1].legend()
axs[2].legend()

plt.show()