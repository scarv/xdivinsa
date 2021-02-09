
import random

import numpy as np

from tqdm import tqdm

from .SAFTraceWriter import SAFTraceWriter
from .SassEncryption import SassEncryption

class SAFTFunc(object):
    """
    Class is for evaluating XdiXor function.
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
        self.set1._plaintext_len=8
        self.set2._plaintext_len=8
        self.set1_dat    = bytes.fromhex("00000304")

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
        #rnddat = bytes.fromhex("0102030405060708")
        rnddat = self.edec.GenerateMessage(8)

        rsp = self.comms.doSetIdata(rnddat)  
        if(rsp):
            print("doSetIdata command Successful")   
        else:
            print("doSetIdata command Failed")

        rsp = self.comms.doTfunc()
        if(rsp):
            print("T-func command Successful")

            datin = self.comms.doGetIdata(8)
            print("Data Input: %s"%datin.hex())

            datout = self.comms.doGetOdata(8)
            print("Data Ouput: %s"%datout.hex())

            print("Verify: \na=0x%s \nb=0x%s"%(datin[0:4].hex(),datin[4:8].hex()))
            print("a^b   == 0x%s"%datout[4:8].hex())
            print("%s    == 0x%s"%(hex(int.from_bytes(datin[0:4],'big')^int.from_bytes(datin[4:8],'big')),datout[4:8].hex()))
        else:
            print("T-func command Failed")

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
            rbit       = random.getrandbits(1)
            op1        = np.random.randint(0,2**24)
            op2        = np.random.randint(0,2**24)
            if (ttest==1)&(rbit==1):
                # Add to set 1 with a fixed message
                fixres = int.from_bytes(self.set1_dat,'big')
                op2    = fixres ^ op1
            current_idata = op1.to_bytes(4,'big') + op2.to_bytes(4,'big')
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

