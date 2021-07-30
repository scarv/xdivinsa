
# XDivinsa: A Diversifying Instruction Agent

This repository contains a frame-work to evaluate **XDivinsa** based on the instruction set extension (ISE) of RISCV.
**XDivinsa** is a combination of a hardware-based hiding technique and diversified ISE.
The technique is realised by injecting random delay and dummy operations in between duration of instruction execution based on hardware diversification. This makes the instruction performance diversifying in time and power dimention.
**XDivinsa** can be effectively, automatically integrated into cryptosystem without needing cryptographic algorithm experts to mitigate side-channel analysis attacks.

<!--- -------------------------------------------------------------------- --->

## Organisation

```
├── bin                - scripts (e.g., environment configuration)
├── fpga               - source code for the FPGA implementation using Vivado
│   ├── board          - source for supporting a specific board (e.g. sakura-x)
│   ├── script         - scripts for handling the FPGA bitstream on Vivado
│   └── soc            - the Vivado projects based on Picorv and Rocketchip SoC.
├── rtl                - the implementations of the XDivinsa co-processor.
├── csrc               - source code for embedded software
│   ├── common           - shared code resources
│   ├── helloworld       - source code for the hello-world program.
│   ├── test-xdivinsa    - source code for verifying the XDivinsa ISE.
│   └── ...              - ...
├── toolchain          - scripts to install the XDivinsa-supporting RISC-V toolchain.
└── work               - working directory for build

```

<!--- -------------------------------------------------------------------- --->

## Quickstart

- For seting up the repository

  - Fix path for the RISCV toolchain, e.g., 
  
    ```sh
    export RISCV="/opt/xdi-riscv"
    ```

  - Fix path for the installed VIVADO Design suite, e.g., 
  
    ```sh
    export VIVADO_TOOL_DIR="/opt/Xilinx/Vivado/2018.2"
    ```

  - Clone the repository and setup environment.
  
    ```sh
    git clone https://github.com/scarv/xdivinsa ./xdivinsa
    cd ./xdivinsa
    git submodule update --init --recursive
    source bin/source.me.sh
    ```
  
  - Build the XDivinsa-supporting RISC-V toolchain. 

    ```sh
    make toolchain 
    ```
  
- Build the FPGA implementation for XDivinsa-RISCV systems.

  - Select a RISC-V base core, i.e., Picorv or Rocket-Chip.

    ```sh
      export CORE=[picorv-xdivinsa/rocket-xdivinsa]
    ```
A
  - Build the Vivado project for the FPGA implementation of the XDivinsa-supporting RISC-V processor system. 
 
    ```sh
      make verilog
      make fpga-project
    ```
  - Compile the FPGA implementation to generate bitstream. 
 
    ```sh
      make bitstream
    ```

- Run the testing software on the FPGA board.

  - Do the [board setup](#Board-setup)

  - Open serial port via /dev/ttyUSB in another terminal to monitor the program running, e.g.,

    ```sh
    microcom -p /dev/ttyUSB0 -s 115200
    ```

  - Build the testing sofware for the XDivinsa ISE.
    ```sh
    make -B test-xdivinsa
    ```
  - Update and download bitstream.
    ```sh
    make bit-update
    make program-fpga
    ```

<!--- -------------------------------------------------------------------- --->

## Board setup

The image below illustrates the setup of the Sakura-X board for the system.

![Board Setup](https://github.com/scarv/xdivinsa/blob/master/Sakura-X-Setup.png)

The system has a serial port to communicate to the host computer. 
A USB-UART cable (like Adafruit USB to TTL Serial Cable) is needed to connect directly from a USB port of the host computer to the pins of the serial port on FPGA board. 

## References and Useful links:
- [1] Yang, B., Rožic, V., Grujic, M., Mentens, N., & Verbauwhede, I. (2018). ES-TRNG: A High-throughput, Low-area True Random Number Generator based on Edge Sampling. IACR Transactions on Cryptographic Hardware and Embedded Systems, 2018(3), 267-292. https://doi.org/10.13154/tches.v2018.i3.267-292
- [NIST Statistical Test Suite] https://csrc.nist.gov/projects/random-bit-generation/documentation-and-software
- [AIS31 testsuit] https://www.bsi.bund.de/SharedDocs/Downloads/DE/BSI/Zertifizierung/Interpretationen/AIS_31_testsuit_zip.html
- [RocketChip Core](https://github.com/freechipsproject/rocket-chip) - The RocketChip core implementation being used in the platform.
- [Picorv32 Core](https://github.com/cliffordwolf/picorv32) - The picorv32 core implementation being used in the platform.
- [Sakura-x platform](http://satoh.cs.uec.ac.jp/SAKURA/hardware/SAKURA-X.html) - Sakura-X board homepage
