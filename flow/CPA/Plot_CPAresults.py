#!/usr/bin/python3

import sys
import pickle
import numpy as np
import argparse
import matplotlib.pyplot as mpl


def parse_args():
    """
    Parse command line arguments.
    """
    parser = argparse.ArgumentParser()

    parser.add_argument("--operation", type=int, default='0',
        help="The operation to be performed: 0: CPA Key-Recovery Attack; 1: TTtS")
    parser.add_argument("--ylim", type=float, default='0.2',
        help="set a limitation for y-axis of the graph")
    parser.add_argument("--datfile", type=str, default='attack_R.pikl',
        help="Result data")
    parser.add_argument("--svgfile", type=str, default='plot_attack.svg',
        help="svg file")
    return parser.parse_args()

def main():
    """
    Main program loop
    Example:
    python3 Plot_CPAresults.py --operation 0 --datfile ./attack_R.pikl --svgfile plot_attack.svg
    python3 Plot_CPAresults.py --operation 1 --datfile ./TTtS.pikl --svgfile plot_TTtS.svg
    """
    cmdargs  = parse_args()
    frd = open(cmdargs.datfile,'rb')
    if (cmdargs.operation == 0):
        corkey=pickle.load(frd)
        R     =pickle.load(frd)
        g=np.unravel_index(np.argmax(R, axis=None), R.shape)

        mpl.figure(1) 
        mpl.ylim([0, cmdargs.ylim])
        mpl.xlim([0, np.size(R,1)])
        for i in range(0,np.size(R,0)):
            mpl.plot(R[ i,:], linewidth=0.5, color='darkgrey')

        mpl.plot(R[  0   ,:], linewidth=0.5, color='darkgrey', label='wrong keys')
        mpl.plot(R[g[0]  ,:], linewidth=0.7, color='blue'    , label='guessed key')
        mpl.plot(R[corkey,:], linewidth=0.7, color='black'   , label='correct key')
        mpl.legend()

    elif (cmdargs.operation == 1):
        refkey=pickle.load(frd)
        NT    =pickle.load(frd)
        R     =pickle.load(frd)

        mpl.figure(figsize=(7,4))     
        mpl.xlim([0, np.max(NT)])
        mpl.xlabel("Number of Traces", fontsize= 11)
        mpl.xticks(fontsize= 11)
        mpl.ylim([0, cmdargs.ylim])
        mpl.yticks(fontsize= 11)
        mpl.ylabel("Correlation Coefficient", fontsize= 11)
        for i in range(0,np.size(R,0)):
            mpl.plot(NT,  R[i,:], linewidth=0.5, color='darkgrey')

        mpl.plot(NT, R[i,:]     , linewidth=0.5, color='darkgrey', label='other keys')
        mpl.plot(NT, R[refkey,:], linewidth=0.7, color='black'   , label='correct key')
        mpl.legend(fontsize= 11)

    frd.close()
    mpl.savefig(cmdargs.svgfile)        
#    mpl.show()
    sys.exit(0)

if(__name__ == "__main__"):
    main()
