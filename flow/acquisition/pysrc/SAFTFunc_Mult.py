
import random

import numpy as np

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

        self.set1        = SAFTraceWriter()
        self.set2        = SAFTraceWriter()

        # Constants input value
        #self.set1_dat   = bytes.fromhex("0000000000000000")
        #self.set1._plaintext_len=8
        #self.set2._plaintext_len=8
        self.set1_dat    = bytes.fromhex("00010203040506070000000000000000")

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
            print("a*b   == 0x%s"%datout[0:8].hex())
            print("%s    == 0x%s"%(hex(int.from_bytes(datin[0:4],'big')*int.from_bytes(datin[4:8],'big')),datout[0:8].hex()))
        else:
            print("T-func command Failed")

    def TotalTraces(self):
        """
        Return the total number of traces captured in both sets.
        """
        return len(self.set1) + len(self.set2)

    def RunAcq(self):
        """
        Runs the capture process on the target device.
        """

        dropped_traces = 0

        for i in tqdm(range(self._num_traces)):

            current_idata = None 
            rbit        = random.getrandbits(1)

            if(rbit):
                # Add to set 1 with a fixed message
                current_idata = self.set1_dat
            else:
                # Add to set 2 with a random message
                current_idata = self.edec.GenerateMessage(length=16)

            self.comms.doSetIdata(current_idata)

            self.scope.StartCapture()

            self.comms.doTfunc()

            self.scope.WaitForReady()

            tracedata = self.scope.GetData(self._trace_channel)

            if(tracedata[0] == None):
                dropped_traces += 1

            elif(rbit):
                # Add to set 1
                self.set1.AddTrace(current_idata,tracedata)

            else:
                # Add to set 2
                self.set2.AddTrace(current_idata,tracedata)

        return dropped_traces

