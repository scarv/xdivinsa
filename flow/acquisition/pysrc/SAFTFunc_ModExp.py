
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

        self.traceset    = SAFTraceWriter() 
        self.set1        = SAFTraceWriter()
        self.set2        = SAFTraceWriter()

        # Constants input value
        #self.set1_dat   = bytes.fromhex("00000000000000000000000000000000")
        self.set1_dat   = self.edec.GenerateMessage(length=16)	            #constant random ciphertext
        #self.set1_dat    = int("bf_f5_4c_d7_1a_10_ef_c0_e8_ea_b8_62_ef_b2_7c_ef",16).to_bytes(16,byteorder='big')    #constant key

#        N=int("b9_b4_8b_3e_f6_10_fd_30_db_40_4e_d7_59_17_8d_03_ae_fa_8b_5d_9f_4a_ec_b0_7f_92_bf_07_39_f8_c2_5b_\
#b5_62_ef_49_de_2d_21_4d_37_95_9a_1e_d4_e4_88_46_39_a2_b0_c4_c6_98_69_de_dd_63_be_74_71_a5_28_72_\
#02_ec_76_52_fb_7f_30_f3_ed_66_be_88_41_da_6d_87_55_a8_24_91_26_78_e5_c7_ab_c3_f5_48_d1_92_6b_b8_\
#7c_05_3e_b5_e1_05_fc_a3_53_63_a1_02_be_ce_fb_d6_2b_b2_22_ac_f0_94_84_72_8e_08_2f_df_df_d6_e3_c5",16)
#        d=int("b1_f5_4c_d7_1a_10_ef_c0_e8_ea_b8_62_ef_b2_7c_e4_c1_c2_c0_d9_07_f4_51_db_55_75_77_f0_61_e2_7b_c6_\
#4c_8d_31_88_15_0c_24_74_8d_c6_a4_39_d5_83_e6_ff_7c_ed_e5_fd_bb_de_84_de_18_44_24_58_0c_2b_dc_13_\
#bb_ca_e4_99_33_f2_53_a8_9e_d5_61_ee_9d_f2_51_61_65_86_08_df_85_d4_af_96_bf_7d_aa_ba_dc_85_c9_7c_\
#1c_3e_b2_ce_63_e3_d6_1f_62_f9_92_8b_f8_d2_d6_86_9f_3e_89_34_a5_68_88_fe_80_b4_15_26_78_4e_1f_e1",16)

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
        N=int("f9_b4_8b_3e_f6_10_fd_30_db_40_4e_d7_59_17_8d_03",16)
        d=int("ef_f5_4c_d7_1a_10_ef_c0_e8_ea_b8_62_ef_b2_7c_bf",16)

        rnddat = self.edec.GenerateMessage(length=16)
        #rnddat = bytes.fromhex("0102030405060708")

        rsp = self.comms.doSetIdata(rnddat)  
        if(rsp):
            print("doSetIdata command Successful")   
        else:
            print("doSetIdata command Failed")

        rsp = self.comms.doTfunc()
        if(rsp):
            print("T_func command Successful")

            datin = self.comms.doGetIdata(16)
            print("Data Input: %s"%datin[::-1].hex())

            datout = self.comms.doGetOdata(16)
            print("Data Ouput: %s"%datout[::-1].hex())

            print("Verify: \nN=%d \nk=%d\nc=0x%s "%(N,d,datin[::-1].hex()))
            print("pow(c,k,N) == 0x%s"%datout[::-1].hex())
            print("%s == 0x%s"%(hex(pow(int.from_bytes(datin,'little'),d,N)),datout[::-1].hex()))
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

            rbit                        = random.getrandbits(1)
            if  ttest==1: 
                if(rbit):
                    # Add to set 1 with a fixed input data
                    current_idata       = self.set1_dat
                else:
                    # Add to set 2 with a random odd input data                
                    current_idata       = self.edec.GenerateMessage(length=16)
                    while current_idata[15]%2 == 0:
                        current_idata   = self.edec.GenerateMessage(length=16)

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

