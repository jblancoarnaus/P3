import numpy as np
from scipy.io import wavfile
import matplotlib.pyplot as plt
import scipy.signal as sig
import matplotlib.patches as mpatches
plt.rcParams['figure.dpi'] = 150

samplerate_in, data_in = wavfile.read('30ms.wav')
data_out = np.loadtxt('aut.txt')
fm=16000
data_out = data_out[240:480]  #obtain 2nd window

red_patch = mpatches.Patch(color='red', label='Input wav', linewidth=0.2)
blue_patch = mpatches.Patch(color='blue', label='Autocorrelation', linewidth=0.2)

#!!!not a really reliable method to find peaks (temporal domain + some suppositions). 
#Works with 30ms.wav signal given the nature of it (2nd peak is at least 90% of the first one)
#this probably wouldn't work with slightly distorted signals (it'd oscillate)
peaks = sig.find_peaks(data_in)
peaks = peaks[0] #unreliable, we can't guarantee that the first peak is a maxima
first_peak = peaks[0] 
#find second peak (90% of the maxima)
max = data_in[first_peak]
for i in range(len(peaks)-1):
  if((data_in[peaks[i+1]])> max*0.9):
    second_peak = peaks[i+1]
    break
period = (second_peak-first_peak)/fm*1000
#find 2nd maximum in the autocorrelation
max = data_out[40:400].max(); #check available pitch values
pos = np.where(data_out==max)
pos = pos[0]
fig, axs = plt.subplots(2)
last_pos = len(data_in)
x_axis = (np.linspace(0,last_pos-1,last_pos))/fm #obtain x axis in seconds

print("Period estimate: "+ str("{0:0.3f}".format(period)) + " ms")
print("Pitch estimate: "+ str("{0:0.3f}".format(1/period*1000)) + " Hz")
print("2nd Autocorrelation maxima index: " + str(pos))
print("Autocorrelation estimate: " + str(np.round(fm/pos,3)))

axs[0].plot(x_axis,data_in, 'r-', linewidth=0.2)
axs[0].plot(x_axis[first_peak:second_peak],data_in[first_peak:second_peak], 'g-', linewidth=0.3)
axs[0].set_ylabel("amplitude x")

axs[1].scatter(pos,max, c='red', s=5)
axs[1].plot( data_out, 'b-', linewidth=0.2)
fig.suptitle("Input signal and autocorrelation")

plt.ylabel("amplitude r")
plt.legend(handles=[red_patch, blue_patch],prop={'size': 6})
plt.show()