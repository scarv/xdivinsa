
import random

import numpy as np
import binascii
from chacha20poly1305 import ChaCha20Poly1305

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
        self.set1._plaintext_len=64
        self.set2._plaintext_len=64
        self.set1_dat    = binascii.a2b_hex('090000004a00000000000000')   #constant message

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

        rnddat = self.edec.GenerateMessage(length=64)

        datin = np.zeros(16,dtype=np.uint32)
        c     = [0x61707865, 0x3320646e, 0x79622d32, 0x6b206574]
        k     = [0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,
                 0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c]
        cn    = [0x00000001, 0x09000000, 0x4a000000, 0x00000000]
        datin[ 0: 4]= np.array(c)
        datin[ 4:12]= np.array(k)
        datin[12:16]= np.array(cn)
        datin[13:16]= np.random.randint(0,2**32,3)
        m=datin.tobytes()
           
        rsp = self.comms.doSetIdata(m) 

        if(rsp):
            print("doSetIdata command Successful")   
        else:
            print("doSetIdata command Failed")

        rsp = self.comms.doTfunc()
        if(rsp):
            print("T_func command Successful")

            print("\nTest Input      : 0x%s"%m.hex())

#            rddatin = self.comms.doGetIdata(128)
#            print("\nMasked Input    : %s"%rddatin[0:128].hex())

            key   = datin[ 4:12].tobytes()
            nonce = datin[13:16].tobytes()
            text  = np.zeros(64,dtype=np.uint8)
            cip   = ChaCha20Poly1305(key)
            ciphertext = cip.encrypt(nonce, text)
            print("\nReference  Output: 0x%s"%ciphertext[0:64].hex())

            datout = self.comms.doGetOdata(64)
            print("\nCalculated Output: 0x%s"%datout[0:64].hex())            
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
        idata = np.zeros(16,dtype=np.uint32)
        c     = [0x61707865, 0x3320646e, 0x79622d32, 0x6b206574]
        k     = [0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,
                 0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c]         
        cn    = [0x00000001, 0x09000000, 0x4a000000, 0x00000000]
        idata[ 0: 4]= np.array(c)
        idata[ 4:12]= np.array(k)
        idata[12:16]= np.array(cn)
        
        Ntpc          = 10 #number of traces per captures
        print("garthering %d traces ..." %self._num_traces) 

        for j in tqdm(range(0,self._num_traces,Ntpc)):
            
            self.scope.StartCapture(Ntpc)
            rbit        = random.getrandbits(1)
            m           = np.random.randint(0,2**32,3) 
            if (ttest==1)&(rbit==1):
	            # Add to set 1 with a fixed input data                         
                m       = np.fromstring(self.set1_dat,dtype='>u4')
            idata[13:16]= m
            current_idata=idata.tobytes()
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
