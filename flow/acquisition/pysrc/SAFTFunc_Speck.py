
import random

import numpy as np
import binascii
from speck import SpeckCipher

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
        self.set1._plaintext_len=29*4
        self.set2._plaintext_len=29*4
        self.set1_dat    = binascii.a2b_hex('454e028b8c6fa548')   #constant message

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

        datin = np.zeros(29,dtype=np.uint32)
        m     = np.fromstring(self.edec.GenerateMessage(length=2*4),dtype=np.uint32)
#        m     = [0x7475432d, 0x3b726574] #[y x]
#        m     = [0x454e028b, 0x8c6fa548] #[y x]
        k     = [0x03020100, 0x0b0a0908, 0x13121110, 0x1b1a1918]

        key = int.from_bytes(np.array(k,dtype=np.uint32).tobytes(),byteorder='little')
        tiny_cipher = SpeckCipher(key, key_size=128, block_size=64)

        datin[ 0:27]= np.array(tiny_cipher.key_schedule)
        datin[27:29]= np.array(m,dtype=np.uint32)

        dat=datin.tobytes()
        
        rsp = self.comms.doSetIdata(dat) 

        if(rsp):
            print("doSetIdata command Successful")   
        else:
            print("doSetIdata command Failed")

        rsp = self.comms.doTfunc()
        if(rsp):
            print("T_func command Successful")

            print("\nTest Input      : 0x%s"%datin.tobytes().hex())

            text =int.from_bytes(np.array(m,dtype=np.uint32).tobytes(),byteorder='little')
#            ciptext    = tiny_cipher.encrypt(text)
            ciptext    = tiny_cipher.decrypt(text)

            print("\nReference  Output: %s"%hex(ciptext))

            datout = self.comms.doGetOdata(8)
            print("\nCalculated Output: 0x%s"%datout[::-1].hex())       

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
        idata = np.zeros(29,dtype=np.uint32)

        k     = [0x03020100, 0x0b0a0908, 0x13121110, 0x1b1a1918]
        key = int.from_bytes(np.array(k,dtype=np.uint32).tobytes(),byteorder='little')
        tiny_cipher = SpeckCipher(key, key_size=128, block_size=64)
        idata[ 0:27]= np.array(tiny_cipher.key_schedule)
        
        Ntpc          = 10 #number of traces per captures
        print("garthering %d traces ..." %self._num_traces) 

        for i in tqdm(range(0,self._num_traces,Ntpc)):
            
            self.scope.StartCapture(Ntpc)
            rbit        = random.getrandbits(1)
            m           = np.random.randint(0,2**32,2)
            if (ttest==1)&(rbit==1):
                # Add to set 1 with a fixed input data
                m       = np.fromstring(self.set1_dat,dtype='>u4')          
            idata[27:29]= np.array(m,dtype=np.uint32)
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

