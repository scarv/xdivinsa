#!/usr/bin/python3

import random
import time
import numpy as np
import argparse
import matplotlib.pyplot as plt

from scipy import signal
from scipy.spatial.distance import euclidean
from fastdtw import fastdtw
from tqdm import tqdm

from SAFTraceSet    import SAFTraceSet
from SassEncryption import SassEncryption


def butter_highpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = signal.butter(order, normal_cutoff, btype='high', analog=False)
    return b, a

def butter_bandpass(lowcut, highcut, fs, order=5):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = signal.butter(order, [low, high], btype='band')
    return b, a

def make_graph(p1, p2, p3, d):
    plt.show()

    dat1 = p1
    dat2 = p2[0,:]
    dat3 = p3[0,:]
    L = np.size(p3,1)

    plt.subplot(3,1,2)
    axes1 = plt.gca()
    axes1.set_xlim(0, L+10)
    axes1.set_ylim(min(p1), max(p1))
    line1, = axes1.plot(np.arange(0,L), dat1,  'r-')
    plt.ylabel("Power")
    plt.xlabel("Time")
    plt.title("Set 1 Average Trace",fontsize= 9)

    plt.subplot(3,1,1)
    axes2 = plt.gca()
    axes2.set_xlim(0, L+10)
    axes2.set_ylim(-0.001, 0.005)
    line2, = axes2.plot(np.arange(0,L), dat2, 'r-')
    plt.ylabel("Power")
    plt.xlabel("Time")
    plt.title("Set 2 Average Trace",fontsize= 9)

    plt.subplot(3,1,3)
    axes3 = plt.gca()
    axes3.set_xlim(0, L+10)
    axes3.set_ylim(-0.001, 0.005)
    line3, = axes3.plot(np.arange(0,L), dat3, 'r-')
    plt.ylabel("Power")
    plt.xlabel("Time")
    plt.title("Set 3 Average Trace",fontsize= 9)


    for i in range(0, np.size(p2,0)):
        axes2.set_ylim(min(p2[i,:]), max(p2[i,:]))
        line2.set_ydata(p2[i,:])
		
        axes3.set_ylim(min(p3[i,:]), max(p3[i,:]))
        line3.set_ydata(p3[i,:])
        plt.draw()
        plt.pause(1e-17)
        time.sleep(1)

    fig = plt.figure(2)

    plt.subplot(3,1,1)
    plt.plot(p1, linewidth=0.25)
    plt.ylabel("Power")
    plt.xlabel("Time")
    plt.title("Set 1 Average Trace",fontsize= 9)
    plt.subplot(3,1,2)
    plt.plot(np.average(p2,  axis=0), linewidth=0.25, color="red")
    plt.plot(np.std(p2,  axis=0), linewidth=0.25, color="blue")
    plt.ylabel("Power")
    plt.xlabel("Time")
    plt.title("Set 2 Average Trace",fontsize= 9)

    plt.subplot(3,1,3)
    plt.plot(np.average(p3,  axis=0), linewidth=0.25, color="red")
    plt.plot(np.std(p3,  axis=0), linewidth=0.25, color="blue")
    plt.ylabel("Power")
    plt.xlabel("Time")
    plt.title("Aligned Trace",fontsize= 9)

    plt.tight_layout(pad=0)

    plt.figure(3)
    plt.plot(d, linewidth=0.25)
    

    plt.figure(4)
    plt.subplot(3,1,1)
    plt.plot(p1, linewidth=0.25, color="black")
    plt.plot(p2[0,:], linewidth=0.25, color="red")
    plt.plot(p3[0,:], linewidth=0.25, color="blue")
    plt.subplot(3,1,2)
    plt.plot(p1, linewidth=0.25, color="black")
    plt.plot(p2[9,:], linewidth=0.25, color="red")
    plt.plot(p3[9,:], linewidth=0.25, color="blue")
    plt.subplot(3,1,3)
    plt.plot(p1, linewidth=0.25, color="black")
    plt.plot(p2[4,:], linewidth=0.25, color="red")
    plt.plot(p3[4,:], linewidth=0.25, color="blue")
    plt.show()

def main():
    """
    python DTW-TTestEval.py --TS1 path1 --TS2 path2
  
    python DTW-TTestEval.py --TS1 /home/thinhpham/Documents/SCARV/bsp-sakura-x/work/TTestDataRef1/TTest_25M_xdivinsa_d0dt2_nonzero/tt_set1_10000.trs --TS2 /home/thinhpham/Documents/SCARV/bsp-sakura-x/work/TTestDataRef1/TTest_25M_xdivinsa_d0dt1_nonzero/tt_set1_10000.trs

    """
    parser = argparse.ArgumentParser()
    
    parser.add_argument("--TS1", type=str,
        help="Trace Set 1 file")
    parser.add_argument("--TS2", type=str,
        help="Trace Set 2 file")
    
    args = parser.parse_args()

    #Fb, Fa = butter_highpass(25000000, 125000000, order=5)
    Fb, Fa = butter_bandpass(25000000, 50000000, 125000000, order=5)

    ts1 = SAFTraceSet.LoadTRS(args.TS1) 
    trace1_m = ts1.traces[:,0:700]
    trace1_v = np.average(trace1_m,  axis=0)

    trace1_v = signal.filtfilt(Fb, Fa, trace1_v)
    s1= trace1_v[4::5]
    #s1       = [np.max(trace1_v[i:i+5]) for i in np.arange(0,len(trace1_v),5)]

    ts2 = SAFTraceSet.LoadTRS(args.TS2) 
    trace2_m = ts2.traces[:,0:700]
    #trace2_v = np.average(trace2_m,  axis=0)

    atr = tqdm(range(0,np.size(trace2_m,0)))
    atr.set_description("Aligning Traces")
    
    s2 =np.zeros((np.size(trace2_m,0),len(s1)))
    s3 =np.zeros((np.size(trace2_m,0),len(s1)))
    d  =np.zeros(np.size(trace2_m,0))
    for i in atr:    
        trace2_v = signal.filtfilt(Fb, Fa, trace2_m[i,:])
        s2[i,:]= trace2_v[4::5]
        #s2       = [np.max(trace2_v[i:i+5]) for i in np.arange(0,len(trace2_v),5)]

        distance, path = fastdtw(s1, s2[i,:], dist=euclidean)
        d[i] = distance
        #print("distance=%f" %distance)
        #s3 =np.zeros(len(s1))
        k=0
        for j in range(0,len(s1)):
            s3[i,j] = s2[i,path[k][1]] 
            while (path[k][0]==j):        
                k   +=1
                if (k==len(path)):
                    break
        
        #k=0
        #for j in range(0,len(s1)):
        #    s3[j] = 0 
        #    nk    = 0 
        #    while (path[k][0]==j):        
        #        s3[j] += s2[path[k][1]]
        #        nk    += 1
        #        k     += 1
        #        if (k==len(path)):
        #            break
        #   s3[j] = s3[j]/nk
   
    make_graph(s1, s2, s3, d)


if(__name__ == "__main__"):
    main()
        
