#!/usr/bin/python3

import os
import gc
import sys
import gzip, pickle
import numpy as np
from   SAFTraceSet import SAFTraceSet
import argparse
import matplotlib.pyplot as mpl
from tqdm import tqdm
from multiprocessing import Pool
from functools import partial

sbox = [
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 
  0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, 0xca, 0x82, 0xc9, 0x7d, 
  0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 
  0x72, 0xc0, 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 
  0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, 0x04, 0xc7, 
  0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 
  0xeb, 0x27, 0xb2, 0x75, 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 
  0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 
  0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, 0xd0, 0xef, 0xaa, 0xfb, 
  0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 
  0x9f, 0xa8, 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 
  0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0x0c, 
  0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 
  0x64, 0x5d, 0x19, 0x73, 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 
  0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, 
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 
  0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 0xe7, 0xc8, 0x37, 0x6d,
  0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 
  0xae, 0x08, 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 
  0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, 0x70, 0x3e, 
  0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 
  0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 
  0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 
  0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16]

GKR = 64;  # guessing key range

def estimate(b):
    """
    Return the hamming weight of the byte b.
    """
    return  np.sum((np.array([1,2,4,8,16,32,64,128]) & b)!=0)

class Tracedata:
    def __init__(self, Nd, Ns):
        self.Nd = Nd	# Number of Traces
        self.Ns = Ns 	# Number of Samples per trace
        self.plaintext = np.empty([Nd, 16],dtype=np.uint8)
        self.ciphrtext = np.empty([Nd, 16],dtype=np.uint8)
        self.traces = np.empty([Nd, Ns],dtype=np.float32)

def readtrace(Nd, S0, Sn, data_dir):
    TR = SAFTraceSet.LoadTRS(data_dir)
    if Sn != -1:
        Ns = Sn-S0
    else:
        Ns = len(TR.traces[0])-S0
    CPA = Tracedata(Nd, Ns)
    for i in range(1,Nd):
        CPA.plaintext[i,:]=np.uint8(TR.plaintexts[i,:])
        CPA.traces[i,:]=np.float32(TR.traces[i,S0:S0+Ns])
    return CPA

def sw_prepro(IN, lsw, nspc):
    """
    Sliding window pre-processing

    :param IN    : the trace set to operate on.
          lsw    : the len of sliding window in cycles
          nspc   : number samples per cycle    
    :rtype: tuple
    :return: A tuple of (size, trace-set)
    """   
    atr = tqdm(range(0,IN.Ns-nspc*lsw,nspc))
    atr.set_description("Aligning Traces")
    
    IN.Ns = IN.Ns - nspc*lsw
    trace_aln = np.zeros((IN.Nd, IN.Ns))
    for i in atr:
        for j in range(0,lsw):
            for k in range(0,nspc):    
                trace_aln[:,i + k] += IN.traces[:,i + k + j*nspc]
    
    IN.traces = trace_aln

def corr(kk, datin):
    R = np.zeros([GKR,datin.Ns],dtype=np.float32)
    mh = np.mean(datin.H[:,kk])
    Hkk= datin.H[:,kk]-mh
    Hkk2 = Hkk*Hkk    
    for tt in range(0,datin.Ns):
        mt = np.mean(datin.T[:,tt])
        Ttt= datin.T[:,tt]-mt
        Ttt2= Ttt*Ttt
        R[kk,tt]=abs(np.dot(Hkk,Ttt)/np.sqrt(np.sum(Hkk2)*np.sum(Ttt2)+0.001))

    return R[kk,:]

def keyguess(keybyte, keyref, IN):
    T = np.float32(IN.traces)
    H = np.empty([IN.Nd,GKR],dtype=np.float32)
    R = np.zeros([GKR,IN.Ns],dtype=np.float32)

    D = IN.plaintext[:,keybyte]
    for dd in range(0,IN.Nd):
        for kk in range(0,GKR):
            vdk=sbox[D[dd] ^ kk]
            H[dd,kk]=np.float32(estimate(vdk))


    for kk in tqdm(range(0,GKR)):
        mh = np.mean(H[:,kk])
        Hkk= H[:,kk]-mh
        Hkk2 = Hkk*Hkk
        for tt in range(0,IN.Ns):
            mt = np.mean(T[:,tt])
            Ttt= T[:,tt]-mt
            Ttt2= Ttt*Ttt
            R[kk,tt]=abs( np.dot(Hkk,Ttt)/np.sqrt(np.sum(Hkk2)*np.sum(Ttt2)) )

    g=np.unravel_index(np.argmax(R, axis=None), R.shape)
    ind = np.argmax(R[keyref,:])
    print('key byte[%d] is guessed %x and max correlation at sample %dth' %(keybyte,g[0],g[1]) )
    print('The correct key byte value: %d has the max correlation of %1.4f at sample %dth' %(keyref,R[keyref,ind], ind) )
    fwr = open('attack_R.pikl','wb')
    pickle.dump(keyref, fwr)
    pickle.dump(R, fwr)
    fwr.close()

    mpl.figure(keybyte) 
    for i in range(0,GKR):
        mpl.plot(R[i ,:], linewidth=0.15, color='#CFCFCFCF')
    mpl.plot(R[keyref,:], linewidth=0.25, color='blue')
    mpl.plot(R[g[0]  ,:], linewidth=0.25, color='black')
    mpl.savefig('attack_res.svg')
    return g[0]

def NTtS_Analyse(keybyte,refkey, Sp,T0,Tn,IN):
    T = np.float32(IN.traces)
    H = np.empty([Tn ,GKR],dtype=np.float32)
    CR = np.zeros([GKR,IN.Ns],dtype=np.float32)

    D = IN.plaintext[:,keybyte]
    for dd in range(0,Tn):
        for kk in range(0,GKR):
            vdk=sbox[D[dd] ^ kk]
            H[dd,kk]=np.float32(estimate(vdk))
    
    # calculate a quick CPA with 1000 traces to get the maximum correlation points(mcp) of the guessed keys
    print('finding the maximum correlation points of the guessed keys...')
    for kk in tqdm(range(0,GKR)):
        mh    = np.mean(H[0:Tn,kk])
        CHkk  = H[0:Tn,kk]-mh
        CHkk2 = CHkk*CHkk
        for tt in range(0,IN.Ns):
            mt    = np.mean(T[0:Tn,tt])
            CTtt  = T[0:Tn,tt]-mt
            CTtt2 = CTtt*CTtt
            CR[kk,tt]=abs( np.dot(CHkk,CTtt)/np.sqrt(np.sum(CHkk2)*np.sum(CTtt2)) )
    mcp = np.argmax(CR, axis=1)

    print('Analysing the number of traces to success...')

    Inc = int(Tn/1000)
    NT = np.arange(T0,Tn, Inc)

    R = np.zeros([GKR,len(NT)],dtype=np.float32)

    ii = 0
    for nt in tqdm(NT):
        Ht  = H[0:nt,:]
        #Tt  = T[0:nt,Sp]
        for kk in range(0,GKR):
            mh   = np.mean(Ht[:,kk])
            Hkk  = Ht[:   ,     kk]  -  mh
            Hkk2 = Hkk*Hkk

            Tt   = T [0:nt, mcp[kk]]
            Ttt  = Tt - np.mean(Tt)
            Ttt2 = Ttt*Ttt

            R[kk,ii]=abs(np.dot(Hkk,Ttt)/np.sqrt(np.sum(Hkk2)*np.sum(Ttt2)))
        ii=ii+1

    fwr = open('NTtS.pikl','wb')
    pickle.dump(refkey, fwr)
    pickle.dump(NT, fwr)
    pickle.dump(R, fwr)
    fwr.close()

    mpl.figure(keybyte)     
    for i in range(0,GKR):
        mpl.plot(NT,  R[i,:], linewidth=0.5, color='darkgrey')
    mpl.plot(NT, R[refkey,:], linewidth=0.25, color='black')
    mpl.savefig('NTtS_res.svg')


def parse_args():
    """
    Parse command line arguments.
    """
    parser = argparse.ArgumentParser()

    parser.add_argument("--operation", type=int, default='0',
        help="The operation to be performed; 0: CPA , 1: NTtSs")
# For CPA key-recovery attack
    parser.add_argument("--keybyte", type=int, default='0',
        help="The position of targeted key byte")
    parser.add_argument("--S0", type=int, default=0,
        help="The first sample position in a trace")
    parser.add_argument("--Sn", type=int, default=-1,
        help="The last sample position in a trace")
    parser.add_argument("--Nd", type=int, default=100,
        help="Number of traces are used")
# For Number of Trace to Success analysis
    parser.add_argument("--refkey", type=int, default='1',
        help="The correct key byte")
    parser.add_argument("--Sp", type=int, default=1,
        help="The guessing sample point of successful key")
    parser.add_argument("--T0", type=int, default=0,
        help="The least number of traces is used")
    parser.add_argument("--Tn", type=int, default=-1,
        help="The most number of traces is used")
# Trace data set file
    parser.add_argument("--sw", type=int, default=0,
        help="Perform Sliding Window pre-processing or not")
    parser.add_argument("--data_dir", type=str, default='.',
        help="Where the trace files are stored")

    return parser.parse_args()

def main():
    """
    Main program loop
    Example:
    python3 CPA.py --operation 0 --keybyte 0 --S0 0 --Sn -1 --Nd 30000 --data_dir ./traces.trs 
    python3 CPA.py --operation 1 --refkey  1 --Sp 1 --Tn 30000 --S0 0 --Sn -1 --Nd 30000 --data_dir ./traces.trs 
    """
    cmdargs  = parse_args()

    data_dir = os.path.abspath(cmdargs.data_dir)    
    IN=readtrace(cmdargs.Nd, cmdargs.S0, cmdargs.Sn, data_dir)
    if (cmdargs.sw != 0):
        #Number of samples per Clock is 5 (fs=250M/f=50M)
        ns = 5 
        #Len of siding window
        lsw = cmdargs.sw
        print('Ns before = %d' %(IN.Ns) )
        sw_prepro(IN, lsw, ns)
        print('Ns after = %d' %(IN.Ns) )

    if (cmdargs.operation == 0):
        if (cmdargs.keybyte != -1):
            print('Guessing key byte %dth' %(cmdargs.keybyte) )
            keyguess(cmdargs.keybyte,cmdargs.refkey, IN)           
        else:
            for kb in range(0,16):
                print('Guessing key byte %dth'%(kb))
                keyguess(kb,IN)
    elif (cmdargs.operation == 1):
        kb       = cmdargs.keybyte
        refkey   = cmdargs.refkey

        NTtS_Analyse(kb,refkey,  cmdargs.Sp, cmdargs.T0, cmdargs.Tn, IN)
        
    sys.exit(0)


if(__name__ == "__main__"):
    main()
