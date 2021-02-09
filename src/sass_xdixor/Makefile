work_dir ?= $(abspath ../../work/sass_xdixor)
hal_dir  ?= $(abspath ../../soc/$(CORE)/hal)
sass_dir ?= $(abspath ../../external/fw-acquisition)

CORE ?= rocket-xdivinsa
XD ?= -DXDIVINSA=4

CC=$(RISCV)/bin/riscv64-unknown-elf-gcc
AS=$(RISCV)/bin/riscv64-unknown-elf-as
LD=$(RISCV)/bin/riscv64-unknown-elf-ld
OBJCOPY=$(RISCV)/bin/riscv64-unknown-elf-objcopy
OBJDUMP=$(RISCV)/bin/riscv64-unknown-elf-objdump

INC_DIRS =  -I../common  \
			-I$(hal_dir) \
			-I$(sass_dir)/target \
			
SRCS = sass_xdixor.c \
       ../common/common.c \
       $(hal_dir)/boot.S  \
       $(sass_dir)/target/sasstarget.c \

OUTHEX  = $(work_dir)/sass_xdixor-$(CORE).hex
OUTBIN  = $(work_dir)/sass_xdixor-$(CORE).bin
OUTPUT  = $(work_dir)/sass_xdixor-$(CORE).elf
DISASM  = $(work_dir)/sass_xdixor-$(CORE).disasm


all: $(OUTHEX) $(DISASM)

CCFLAGS+=-Os -O2 -march=rv32im -mabi=ilp32 -nostartfiles -nostdlib -fpie -Wl,-T$(hal_dir)/lscript.ld

$(OUTPUT) : $(SRCS)
	mkdir -p $(work_dir)
	$(CC) $(CCFLAGS) $(INC_DIRS) -D$(firstword $(subst -, ,$(CORE))) -D$(word 2, $(subst -, ,$(CORE))) $(XD) -o $@ $^

$(OUTBIN): $(OUTPUT)
	$(OBJCOPY) -O binary $< $@

$(OUTHEX): $(OUTBIN) 
	od -t x4 -An -w4 -v $< > $@	

$(DISASM) : $(OUTPUT)
	$(OBJDUMP) -Dt $< > $@

clean:
	rm -rf $(work_dir)
.PHONY: clean
