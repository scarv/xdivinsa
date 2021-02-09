#!/usr/bin/python3

import os
import gc
import sys
import numpy as np
import argparse
import matplotlib.pyplot as mpl

from scipy.spatial.distance import euclidean
from fastdtw import fastdtw
from SAFTraceSet import SAFTraceSet
from tqdm import tqdm

class DPAdata:
    def __init__(self, Nd, Ns):
        self.Nd = Nd	# Number of Traces
        self.Ns = Ns 	# Number of Samples per trace
        self.plaintext = np.empty([Nd, 16],dtype=np.uint8)
        self.ciphrtext = np.empty([Nd, 16],dtype=np.uint8)
        self.traces = np.empty([Nd, Ns],dtype=np.float32)

class CORRdata:
    def __init__(self, H, T, Ns):
        self.H  = H
        self.T  = T
        self.Ns = Ns

def readtrace(Nd, S0, Sn, TraceSet):
    if Sn != -1:
        Ns = Sn-S0
    else:
        Ns = len(TraceSet.traces[0])-S0
    DPA = DPAdata(Nd, Ns)
    for i in range(1,Nd):
        DPA.plaintext[i,:]=np.uint8(TraceSet.plaintexts[i,:])
        DPA.traces[i,:]=np.float32(TraceSet.traces[i,S0:S0+Ns])   
    return DPA

def aligning(size, traceset, ref_trace):
    """
    aligning trace set to filter hiding technique's effect.
    :param trace_m: the trace-set to be alligned.
    :param ref_trace: the reference trace.
    :rtype: tuple
    :return: A tuple of the form 
    (size, aligned trace-set)
    """

    atr = tqdm(range(0,size))
    atr.set_description("Aligning Traces")

    #ref_fil = signal.filtfilt(self.Fb, self.Fa, trace_ref)
    #s1 = ref_fil[4::5]
    s1=[np.max(ref_trace[i:i+5]) for i in np.arange(0,len(ref_trace),5)]
    #s1=ref_trace

    alitraces = SAFTraceSet()
    alitraces.trace_description = traceset.trace_description
    alitraces.coding_type       = traceset.coding_type
    alitraces.Allocate(size, len(s1), traceset.plaintext_len)

    trace_m   = np.float32(traceset.traces) 
    aln_trace =np.zeros(len(s1))                    
    #size = 0
    for t in atr:
        #fil = signal.filtfilt(self.Fb, self.Fa, trace_m[i,:])
        #s2 = fil[4::5]
        s2 = [np.max(trace_m[t,i:i+5]) for i in np.arange(0,len(trace_m[0,:]),5)]   
        #s2 = trace_m[t,:]
        d, c = fastdtw(s1, s2, dist=euclidean)
                
#                k=0
#                for j in range(0,len(s1)):
#                    aln_trace[i,j] = 0 
#                    nk             = 0 
#                    while (c[k][0]==j):        
#                        aln_trace[i,j] += s2[c[k][1]]
#                        nk    += 1
#                        k     += 1
#                        if (k==len(c)):
#                            break
#                    aln_trace[i,j] = aln_trace[i,j]/nk
                #if (d < 0.1):
        k=0
        for j in range(0,len(s1)):
            aln_trace[j]= s2[c[k][1]]
            while (c[k][0]==j):                       
                k  += 1
                if (k==len(c)):
                    break
        alitraces.AddTrace(traceset.plaintexts[t,:], aln_trace)
            #size += 1
    print("number of aligned traces:%d"%size)  
    return alitraces


def parse_args():
    """
    Parse command line arguments.
    """
    parser = argparse.ArgumentParser()

    parser.add_argument("--Nd", type=int, default=100,
        help="Number of traces are used")
    parser.add_argument("--tracefile", type=str, default='.',
        help="Where the trace files are stored")
    parser.add_argument("--referfile", type=str, default='.',
        help="Where the reference traces are stored")

    return parser.parse_args()

def main():
    """
    Main program loop
    Example:
    python3 TraceAlign.py --Nd 10000 --tracefile ./traces.trs --referfile ./referfile.trs
    """
    cmdargs  = parse_args()

    #find the reference trace by averaging the reference traceset
    referfile = os.path.abspath(cmdargs.referfile) 
    reftraceset = SAFTraceSet.LoadTRS(referfile)
    reftrace = np.average(reftraceset.traces, axis=0)
    del reftraceset

    #loading traceset to be alligned
    tracefile = os.path.abspath(cmdargs.tracefile)   
    traceset = SAFTraceSet.LoadTRS(tracefile)

    alitraces = aligning(cmdargs.Nd, traceset, reftrace)
    alitraces.DumpTRS(tracefile+".aligned")
    sys.exit(0)


if(__name__ == "__main__"):
    main()
