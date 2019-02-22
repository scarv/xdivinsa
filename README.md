
# Implementing a FPGA-based true random number generators on RISC-V based SoC which instatiates a picorv32 core 

This repository contains a frame-work to evaluate the implementations of true random number generator (TRNG) (i.e. edge-sampling [1], tero [2], meta_ro5 [3]) on an SoC using a RISC-V picorv32 core on different Boards (i.e. Arty, Sakura-X, Zedboard, Zybo).
The TRNG is implemented and wrapped in a co-processor coupled with picorv32. Some custom instructions are defined to control the TRNG.

Note that this code is designed to work specifically with the subsystems built as part of the SCARV project. These subsystems are not yet published.


## Getting Started

```sh
$> git clone git@github.com:scarv/bsp.git 
$> cd bsp
```
In order to build all targets, you will need:

- Build riscv-toolset from [github.com/riscv/riscv-tools](https://github.com/riscv/riscv-tools)

Install required packages for RISC-V toolset before building the toolset:

```sh
$> cd <path to riscv-tool directory>
$> git clone https://github.com/riscv/riscv-tools.git 
$> cd riscv-tools 
$> git submodule update --init --recursive
```

```sh
sudo apt-get install autoconf automake autotools-dev curl libmpc-dev libmpfr-dev libgmp-dev gawk \
                     build-essential bison flex texinfo gperf libncurses5-dev libusb-1.0-0 libboost-dev 
```
	
Build the riscv-tool:

```sh
$> export RISCV=</opt/riscv>
$> cd <path to riscv-tool directory>/riscv-tools
$> ./build.sh
```

- Install Vivado 2018.2 toolset and export install folder

```sh
$> export VIVADO_TOOL_DIR=</opt/Xilinx/Vivado/2018.2>
```

Running the following commands to compile and run test of trng.

```sh
$> source ./bin/source.me.sh
-------------------------[Setting Up Project]--------------------------
BSP_HOME        = <path to directory>/hw-trng
RISCV	        = </opt/riscv>
VIVADO_TOOL_DIR	= <path to Xilinx directory/Vivado/2018.2>
------------------------------[Finished]-------------------------------
$> make seteval-hw-trng
$> export CORE=picorv-trng
```
To setup the enviroment variables of the project.

```sh
$> export trng_ins=<es_trng/tero_trng/meta_ro5_trng>
```
To select a trng that is implemented.

```sh
$> export TARGET=<sakura-x/arty/zedboard/zybo>
```
To select a target board.

```sh
$> make verilog 
```
To generate verilog source file of the project.

```sh
$> make project-soc 
```
To generate Vivado project of SoC system on the selected board.


```sh
$> make bitstream 
```
To implement the bitstream of the SoC system for FPGA device. The 'system_top_wrapper.bit' file is generated in ```<path to directory>/hw-trng/work/<$TARGET>-picorv-trng-<$trng_ins>-imp/<$TARGET>-picorv-trng-<$trng_ins>-imp.runs/impl_1```

```sh
$> make test_trng 
```
To compile test program for trng.


```sh
$> make bit-update 
```
To embed the firmware into the bitstream which is programmed to the FPGA device. A new bitstream file, 'system_top_wrapper.new.bit', file is generated in the same place of the 'system_top_wrapper.bit' file


## Run an example
```sh
$> make program-newbitstream 
``` 
To program FPGA with the updated bitstream, 'system_top_wrapper.new.bit'.

```sh
$> make trng-read-ran-binary Nblock=1
```
To generate and read one block of random bit (32 Kbits)  
 
```sh
$> make trng-read-ran-bytes RAN_FILE=$(PRF_HOME)/work/random.pi NBlock=N
```

To generate, read N block of random bit and write to ./work/random.pi, a raw binary file which is used for randomness test by NIST Statistical test suite or AIS31 test suit.

## References and Useful links:
- [1] Yang, B., Rožic, V., Grujic, M., Mentens, N., & Verbauwhede, I. (2018). ES-TRNG: A High-throughput, Low-area True Random Number Generator based on Edge Sampling. IACR Transactions on Cryptographic Hardware and Embedded Systems, 2018(3), 267-292. https://doi.org/10.13154/tches.v2018.i3.267-292
- [2] Haddad P., Fischer V., Bernard F., Nicolai J. (2015) A Physical Approach for Stochastic Modeling of TERO-Based TRNG. In: Güneysu T., Handschuh H. (eds) Cryptographic Hardware and Embedded Systems -- CHES 2015. CHES 2015. Lecture Notes in Computer Science, vol 9293. Springer, Berlin, Heidelberg
- [3] Vasyltsov I., Hambardzumyan E., Kim YS., Karpinskyy B. (2008) Fast Digital TRNG Based on Metastable Ring Oscillator. In: Oswald E., Rohatgi P. (eds) Cryptographic Hardware and Embedded Systems – CHES 2008. CHES 2008. Lecture Notes in Computer Science, vol 5154. Springer, Berlin, Heidelberg
- [NIST Statistical Test Suite] https://csrc.nist.gov/projects/random-bit-generation/documentation-and-software
- [AIS31 testsuit] https://www.bsi.bund.de/SharedDocs/Downloads/DE/BSI/Zertifizierung/Interpretationen/AIS_31_testsuit_zip.html
- [Picorv32 Core](https://github.com/cliffordwolf/picorv32) - The picorv32 core implementation being used in the platform.
- [Arty platform](https://www.xilinx.com/products/boards-and-kits/arty.html) Arty7-35T board homepage
- [Sakura-x platform](http://satoh.cs.uec.ac.jp/SAKURA/hardware/SAKURA-X.html) - Sakura-X board homepage
- [Zedboard platform](http://zedboard.org/product/zedboard) - Zedboard board homepage
- [Zybo platform](https://store.digilentinc.com/zybo-zynq-7000-arm-fpga-soc-trainer-board/) - Zybo board homepage
