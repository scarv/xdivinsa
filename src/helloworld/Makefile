work_dir ?= $(abspath ../../work/helloworld)
hal_dir  ?= $(abspath ../../soc/$(CORE)/hal)

CORE ?=picorv

CC=$(RISCV)/bin/riscv64-unknown-elf-gcc
AS=$(RISCV)/bin/riscv64-unknown-elf-as
LD=$(RISCV)/bin/riscv64-unknown-elf-ld
OBJCOPY=$(RISCV)/bin/riscv64-unknown-elf-objcopy
OBJDUMP=$(RISCV)/bin/riscv64-unknown-elf-objdump

INC_DIRS =-I$(hal_dir)
SRCS = helloworld.c \
       $(hal_dir)/boot.S \

OUTHEX  = $(work_dir)/helloworld-$(CORE).hex
OUTBIN  = $(work_dir)/helloworld-$(CORE).bin
OUTPUT  = $(work_dir)/helloworld-$(CORE).elf
DISASM  = $(work_dir)/helloworld-$(CORE).disasm

all: $(OUTHEX) $(DISASM)

CCFLAGS+=-Os -O2 -march=rv32im -mabi=ilp32 -ffreestanding -nostdlib -fpie -Wl,-T$(hal_dir)/lscript.ld

$(OUTPUT) : $(SRCS)
	mkdir -p $(work_dir)
	$(CC) $(CCFLAGS) $(INC_DIRS) -o $@ $^
	
$(OUTBIN): $(OUTPUT)
	$(OBJCOPY) -O binary $< $@

$(OUTHEX): $(OUTBIN) 
	od -t x4 -An -w4 -v $< > $@	

$(DISASM) : $(OUTPUT)
	$(OBJDUMP) -Dt $< > $@

clean:
	rm -rf $(work_dir)
.PHONY: clean
