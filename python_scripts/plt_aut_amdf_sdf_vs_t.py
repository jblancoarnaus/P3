import numpy as np
from scipy.io import wavfile
import matplotlib.pyplot as plt
import scipy.signal as sig
plt.rcParams['figure.dpi'] = 150

samplerate_in, data_in = wavfile.read('30ms.wav')
data_out = np.loadtxt('aut_win24ms.txt')
data_outsdf = np.loadtxt('sdfwin24ms.txt')
data_outamdf = np.loadtxt('amdfwin24ms.txt')

#data in boundaries (set to the entire signal by default) can be changed to zoom in and see
#how the time domain method isn't reliable
first_sample_in = 0
last_sample_in = data_in.size
#first_sample_in = 1100
#last_sample_in = 1700
data_in = data_in[first_sample_in:last_sample_in]

#signal parameters and window number selected
fm=16000
win_length_s = 0.024/2
frame_shift_s = win_length_s
frame_size = fm*win_length_s 
frame_shift = fm*frame_shift_s 
window_number = 19 

window_begin = round((window_number-1)*frame_shift)
window_end = round(window_begin+frame_size)

data_out = data_out[window_begin:window_end]  #obtain 2nd window 
data_outsdf = data_outsdf[window_begin:window_end]
data_outamdf = data_outamdf[window_begin:window_end]

##OBTAIN PERIOD IN THE TIME SIGNAL
#!!! not a really reliable method to find peaks (temporal domain + some suppositions). 
#Works with 30ms.wav signal given the nature of it (2nd peak is at least 90% of the first one)
#this probably wouldn't work with slightly distorted signals (it'd oscillate)
peaks = sig.find_peaks(data_in)
peaks = peaks[0] #!!! highly unreliable, we can't guarantee that the first peak is a maxima
first_peak = peaks[0] 
#find second peak (90% of the maxima)
max = data_in[first_peak]
for i in range(len(peaks)-1):
  if((data_in[peaks[i+1]])> max*0.9):
    second_peak = peaks[i+1];
    break
period = (second_peak-first_peak)/fm*1000

##OBTAIN PITCH THROUGH AUTOCORRELATION

#obtain pitch maximum/minimum values in samples
lowest_pitch_hz = 50
highest_pitch_samples = round(fm/lowest_pitch_hz)
highest_pitch_hz = 500
lowest_pitch_samples = round(fm/highest_pitch_hz)

#find 2nd maxima of the autocorrelation
max = data_out[lowest_pitch_samples:highest_pitch_samples].max(); #check available pitch values
pos = np.where(data_out==max)
pos = pos[0]

#find minima of the SDF
minsdf = min(data_outsdf[lowest_pitch_samples:highest_pitch_samples]) #check available pitch values
possdf = np.where(data_outsdf==minsdf)
possdf = possdf[0]

#find minima of the AMDF
minamdf = min(data_outamdf[lowest_pitch_samples:highest_pitch_samples]) #check available pitch values
posamdf = np.where(data_outamdf==minamdf)
posamdf = posamdf[0]

fig, axs = plt.subplots(2)
last_pos = len(data_in)
x_axis = (np.linspace(first_sample_in,last_sample_in-1,data_in.size))/fm #obtain x axis in seconds

print("Period estimate: "+ str("{0:0.3f}".format(period)) + " ms")
print("Pitch estimate: "+ str("{0:0.3f}".format(1/period*1000)) + " Hz")

print("2nd Autocorrelation maxima index: " + str(pos))
print("Autocorrelation estimate: " + str(np.round(fm/pos,3)))

print("SDF minima index: " + str(possdf))
print("SDF estimate: " + str(np.round(fm/possdf,3)))

print("AMDF minima index: " + str(posamdf))
print("AMDF estimate: " + str(np.round(fm/posamdf,3)))

plt.suptitle("Input signal vs autocorrelation, SDF and AMDF", y=1.02)
axs[0].plot(x_axis,data_in, 'r-', linewidth=0.2)
axs[0].plot(x_axis[first_peak:second_peak],data_in[first_peak:second_peak], 'g-', linewidth=0.3)

axs[0].set_ylabel("Amplitude")
axs[0].set_xlabel("Time (s)")

axs[1].set_ylabel("Amplitude")
axs[1].set_xlabel("Samples")

axs[1].scatter(pos,max, c='red', s=5, label="Autocorrelation maxima")
axs[1].scatter(possdf,minsdf, c='orange', s=5, label="SDF minima")
axs[1].scatter(posamdf,minamdf, c='black', s=5, label="AMDF minima")

axs[1].plot( data_out, 'b-', linewidth=0.3, label="Autocorrelation")
axs[1].plot( data_outsdf, 'g-', linewidth=0.3, label="SDF")
axs[1].plot( data_outamdf, 'y-', linewidth=0.3, label="AMDF")

axs[1].legend(loc='upper right', prop={'size': 6})

plt.tight_layout()

plt.show()