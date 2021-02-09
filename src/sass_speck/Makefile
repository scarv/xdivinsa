work_dir ?= $(abspath ../../work/sass_speck)
hal_dir  ?= $(abspath ../../soc/$(CORE)/hal)
sass_dir ?= $(abspath ../../external/fw-acquisition)

CORE ?= rocket-xdivinsa
XD ?= -DXDIVINSA=2

CC=$(RISCV)/bin/riscv64-unknown-elf-gcc
AS=$(RISCV)/bin/riscv64-unknown-elf-as
LD=$(RISCV)/bin/riscv64-unknown-elf-ld
OBJCOPY=$(RISCV)/bin/riscv64-unknown-elf-objcopy
OBJDUMP=$(RISCV)/bin/riscv64-unknown-elf-objdump

INC_DIRS =  -I./ \
            -I../common  \
            -I$(hal_dir) \
            -I$(sass_dir)/target \
			
SRCS = sass_speck.c speck.c\
       ../common/common.c \
       $(hal_dir)/boot.S  \
       $(sass_dir)/target/sasstarget.c \

OUTHEX  = $(work_dir)/sass_speck-$(CORE).hex
OUTBIN  = $(work_dir)/sass_speck-$(CORE).bin
OUTPUT  = $(work_dir)/sass_speck-$(CORE).elf
DISASM  = $(work_dir)/sass_speck-$(CORE).disasm


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
	$(OBJDUMP) -dC $< > $@

clean:
	rm -rf $(work_dir)
.PHONY: clean
