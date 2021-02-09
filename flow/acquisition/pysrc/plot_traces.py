#!/usr/bin/python3

import os
import gc
import sys
import pickle
import numpy as np
import argparse
import matplotlib.pyplot as mpl
from scipy import signal
from tqdm import tqdm
from multiprocessing import Pool
from functools import partial
from sklearn.decomposition import PCA
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis as LDA
from sklearn.discriminant_analysis import QuadraticDiscriminantAnalysis as QDA
from sklearn.neighbors import KNeighborsClassifier
from sklearn import svm as SVM

def butter_highpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = signal.butter(order, normal_cutoff, btype='high', analog=False)
    return b, a

def LoadTRS(filepath):
    """
    Load a trs file from disk and return a new SAFTraceSet object
    :param str filepath: The path to the TRS file to load.
    :param bool infoOnly: If true, only the header information will be
    loaded. If False, the trace set data will be loaded as well.
    :rtype: SAFTraceSet
    """

    with open(filepath, "rb") as fh:

        ctrlcode            = fh.read(1)

        while(ctrlcode != b"\x5f"):

            if(ctrlcode == b"\x41"):
                # Number of traces
                num_traces = int.from_bytes(fh.read(4),"little")
                 
            elif(ctrlcode == b"\x42"):
                # Samples per trace
                trace_length= int.from_bytes(fh.read(4),"little")

            elif(ctrlcode == b"\x43"):
                # Sample coding type (float, 4 bytes each)
                coding_type = fh.read(1)

                if(coding_type != b"\x14"):
                    raise Exception("Unsupported sample encoding: %s" % (coding_type))

            elif(ctrlcode == b"\x44"):
                # Length of data (msg/cipher text) associated with a trace
                datin_length = int.from_bytes(fh.read(2),"little")
            
            elif(ctrlcode == b"\x47"):
                # Trace description
                lenb = int.from_bytes(fh.read(1),"little")
                if(not lenb & 0x80):
                    trace_description = fh.read(lenb).decode("ascii")
                else:
                    lenb = int.from_bytes(fh.read(lenb&0x7F),"little")
                    trace_description = fh.read(lenb).decode("ascii")

            else:
                raise Exception("Unknown byte marker in TRS file: %x"%(ctrlcode))

            ctrlcode = fh.read(1)

        # We have finished reading the header, now we just read the
        # rest of the data and traces.

        pb = tqdm(range(0,num_traces))
        pb.set_description("Loading Traces")

        datin       = np.empty((num_traces,datin_length),dtype=np.int8)
        tracedata   = np.empty((num_traces,trace_length),dtype=np.float32)

        for i in pb:
            datin[i,:] = np.fromfile(fh,count=datin_length, dtype=np.int8)
            tracedata[i,:] = np.fromfile(fh,count=trace_length,dtype=np.float32)

    return [datin, tracedata]

class TraceData:
    def __init__(self, Nd, Ns):
        self.Nd = Nd
        self.Ns = Ns 
        self.datin = np.empty([Nd, 16],dtype=np.uint8)
        self.trace = np.empty([Nd, Ns],dtype=np.float32)
    def extract(self, trace_m, S0=0, Sn=-1):
        if (Sn == -1) or (Sn >= (np.size(trace_m,1))):
           Sn = np.size(trace_m,1)   
        self.Nd = np.size(trace_m,0)
        self.Nd = Sn-S0
        self.trace = trace_m[:,int(S0):int(Sn)]

def readtrace(data_file, Nd, S0, Sn):

    [datin, trace] = LoadTRS(data_file)

    if (Sn != -1) and (Sn <= (np.size(trace,1))):
       Ns = Sn-S0       
    else:
       Ns = np.size(trace,1)-S0
    
    if Nd > np.size(datin,0):
         Nd = np.size(datin,0)
    TempTrace = TraceData(Nd, Ns)

    TempTrace.datin = datin[0:Nd,:]

    Fb, Fa = butter_highpass(1500000, 125000000, order=11)
#    for i in range(0,np.size(trace,0)):
#        trace[i,:] = signal.filtfilt(Fb, Fa, trace[i,:])

    TempTrace.trace = trace[0:Nd,S0:S0+Ns]
    
    return TempTrace


def plot_traces(traces1):
    """
    Creates and returns a matplotlib figure showing the plots
    of each part of the ttrace calculation.
    """
    fig = mpl.figure()
    mpl.subplot(2,1,1)
    mpl.plot(np.average(traces1,  axis=0), linewidth=0.25)
    #mpl.axhline(y=0.0)
    mpl.ylabel("Average")
    
    mpl.subplot(2,1,2)
    mpl.plot(np.std(traces1,  axis=0), linewidth=0.25)
    mpl.ylabel("Std")
    mpl.xlabel("Time")
    mpl.title("Average Trace",fontsize= 9)
    mpl.show()



def parse_args():
    """
    Parse command line arguments.
    """
    parser = argparse.ArgumentParser()

    parser.add_argument("--S0", type=int, default=0,
        help="the first sample position in a trace")
    parser.add_argument("--Sn", type=int, default=-1,
        help="the last sample position in a trace")
    parser.add_argument("--Nd", type=int, default=100,
        help="Number of traces are used")
    parser.add_argument("--trace_file", type=str,  default='work/traces.trs',
        help="Where the trace files are stored")
    return parser.parse_args()

def main():
    """
    Main program loop.
    Example:
    python plot_traces.py --S0 0 --Sn -1 --Nd 400 --trace_file work/traces.trs
    """

    cmdargs  = parse_args()
    trace_file  = os.path.abspath(cmdargs.trace_file)

    IN1=readtrace(trace_file, cmdargs.Nd, cmdargs.S0, cmdargs.Sn)
    plot_traces(IN1.trace)

    sys.exit(0)

if(__name__ == "__main__"):
    main()
