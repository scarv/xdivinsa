
# XDivinsa: A Diversifying Instruction Agent

This repository contains a frame-work to evaluate **XDivinsa** based on the instruction set extension (ISE) of RISCV.
**XDivinsa** is a combination of a time-based hiding technique and operand masking technique at the instruction level.
The time-based hiding technique is realised by injecting random delay in between duration of performing instruction. This makes the instruction performance diversifying in time dimention.
The operand masking technique reduces the dependency of instruction performance on input operands. By using random number in the masking calculation, this makes the instruction performance diversifying in power dimention.

**XDivinsa** can be effectively, automatically integrated into cryptosystem without needing cryptographic algorithm experts to thwart side-channel analysis.

## References and Useful links:
- [1] Yang, B., Rožic, V., Grujic, M., Mentens, N., & Verbauwhede, I. (2018). ES-TRNG: A High-throughput, Low-area True Random Number Generator based on Edge Sampling. IACR Transactions on Cryptographic Hardware and Embedded Systems, 2018(3), 267-292. https://doi.org/10.13154/tches.v2018.i3.267-292
- [NIST Statistical Test Suite] https://csrc.nist.gov/projects/random-bit-generation/documentation-and-software
- [AIS31 testsuit] https://www.bsi.bund.de/SharedDocs/Downloads/DE/BSI/Zertifizierung/Interpretationen/AIS_31_testsuit_zip.html
- [RocketChip Core](https://github.com/freechipsproject/rocket-chip) - The RocketChip core implementation being used in the platform.
- [Picorv32 Core](https://github.com/cliffordwolf/picorv32) - The picorv32 core implementation being used in the platform.
- [Sakura-x platform](http://satoh.cs.uec.ac.jp/SAKURA/hardware/SAKURA-X.html) - Sakura-X board homepage
