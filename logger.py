import matplotlib.pyplot as plt
import numpy as np
import serial
import time


Fs = 100.0;  # sampling rate
Ts = 10.0/Fs; # sampling interval
t = np.arange(0,10,Ts) # time vector; create Fs samples between 0 and 1.0 sec.
x = np.arange(0,10,Ts) # time vector; create Fs samples between 0 and 1.0 sec.
y = np.arange(0,10,Ts) # signal vector; create Fs samples
z = np.arange(0,10,Ts) # signal vector; create Fs samples
tilt = np.arange(0,10,Ts)

serdev = '/dev/ttyACM0'
s = serial.Serial(serdev,115200)
for i in range(400):
    line=s.readline()
    #print (line)
    if(i%4 == 0): x[int(i/4)] = float(line)
    elif(i%4 == 1): y[int(i/4)] = float(line)
    elif(i%4 == 2): z[int(i/4)] = float(line)
    elif(i%4 == 3): tilt[int(i/4)] = float(line)
   


fig, ax = plt.subplots(2, 1)

ax[0].plot(t,x, label = 'x')
ax[0].plot(t,y, label = 'y')
ax[0].plot(t,z, label = 'z')
ax[0].set_xlabel('t')
ax[0].set_ylabel('vector')
ax[0].legend()

ax[1].stem(t,tilt) 
ax[1].set_xlabel('time')
ax[1].set_ylabel('tilt')

plt.show()

s.close()