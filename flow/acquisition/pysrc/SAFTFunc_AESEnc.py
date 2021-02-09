
import random

import numpy as np
import binascii
import Crypto.Cipher.AES as AES

from tqdm import tqdm

from .SAFTraceWriter import SAFTraceWriter
from .SassEncryption import SassEncryption

class SAFTFunc(object):
    """
    Class is for evaluating Multiplication function.
    """


    def __init__(self,comms, scope, num_traces=10000, trace_channel="A"):
        """
        Create a new TTest Capture object.

        :param SassComms comms: The object used to communicate with the
            target device.
        :param SassScope scope: The oscilliscope object to get traces from.
        :param int num_traces: The number of traces overall to capture.
        :param str trace_channel: Which scope channel gets put into the trace files.
        """

        self.comms       = comms
        self.edec        = SassEncryption()
        self.scope       = scope
        
        self._num_traces  = num_traces
        self._trace_channel = trace_channel

        self.traceset    = SAFTraceWriter() 
        self.set1        = SAFTraceWriter()
        self.set2        = SAFTraceWriter()

        # Constants input value
        self.set1_dat    = binascii.a2b_hex('da39a3ee5e6b4b0d3255bfef95601890')   #constant message

    @property
    def num_traces(self):
        """ Get the number of traces in this set """
        return self._num_traces
    @num_traces.setter
    def num_traces(self, Nt):
        """ Set the number of traces for accquisition """
        self._num_traces = Nt

    @property
    def trace_channel(self):
        """ Get the trace_channel in this set """
        return self._trace_channel
    @trace_channel.setter
    def trace_channel(self, Ch):
        """ Set the number of traces for accquisition """
        self._trace_channel = Ch

    def FuncTest(self):
        """
        Run testing on target function.
        """

        k = binascii.a2b_hex('0123456789abcdef123456789abcdef0')
        m = binascii.a2b_hex('da39a3ee5e6b4b0d3255bfef95601890')
        
        rnddat = self.edec.GenerateMessage(length=16)

        m=rnddat
        #rnddat = bytes.fromhex("0102030405060708")

        rsp = self.comms.doSetIdata(m)  
        if(rsp):
            print("doSetIdata command Successful")   
        else:
            print("doSetIdata command Failed")

        rsp = self.comms.doTfunc()
        if(rsp):
            print("T_func command Successful")
            c = AES.new( k, AES.MODE_ECB).encrypt( m ) 

            datin = self.comms.doGetIdata(16)
            print("Data Input: %s"%datin[::-1].hex())

            datout = self.comms.doGetOdata(16)
            print("Data Ouput: %s"%datout[::-1].hex())

            print("Verify: \nk=%s \nm=%s \nc=%s "%(binascii.b2a_hex( k ), binascii.b2a_hex( m ), binascii.b2a_hex( datout )))
            print("AES.Enc(m,k) == %s" %(binascii.b2a_hex( c )))
            print("%s == %s"%(binascii.b2a_hex( c ), binascii.b2a_hex( datout )))
        else:
            print("T_func command Failed")

    def TotalTraces(self):
        """
        Return the total number of traces captured in both sets.
        """
        return len(self.set1) + len(self.set2)

    def RunAcq(self, ttest = 1):
        """
        Runs the capture process on the target device.
        """

        Ntpc          = 100 #number of traces per captures
        print("garthering %d traces ..." %self._num_traces)

        for i in tqdm(range(0,self._num_traces,Ntpc)):

            self.scope.StartCapture(Ntpc)
            rbit        = random.getrandbits(1)
            if  ttest==1: 
                if(rbit):
                    # Add to set 1 with a fixed input data
                    current_idata = self.set1_dat
                else:
                # Add to set 2 with a random odd input data                
                    current_idata = self.edec.GenerateMessage(length=16)
            else:
                current_idata = self.edec.GenerateMessage(length=16)

            self.comms.doSetIdata(current_idata)

            for i in range(Ntpc):
                self.comms.doTfunc()

            self.scope.WaitForReady()

            tracedata = self.scope.GetData(self._trace_channel,Ntpc)

            for i in range(Ntpc):
                if (ttest==1):
                    if(rbit):
                        # Add to set 1
                        self.set1.AddTrace(current_idata,tracedata[i])
                    else:
                        # Add to set 2
                        self.set2.AddTrace(current_idata,tracedata[i])
                else:
                    self.traceset.AddTrace(current_idata,tracedata[i])

        return Ntpc

