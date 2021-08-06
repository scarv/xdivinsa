work_dir ?= $(abspath ../../work/test_aes)
hal_dir  ?= $(abspath ../../soc/$(CORE)/hal)

CORE ?= rocket-xdivinsa
XD ?= 

CC=$(RISCV)/bin/riscv64-unknown-elf-gcc
AS=$(RISCV)/bin/riscv64-unknown-elf-as
LD=$(RISCV)/bin/riscv64-unknown-elf-ld
OBJCOPY=$(RISCV)/bin/riscv64-unknown-elf-objcopy
OBJDUMP=$(RISCV)/bin/riscv64-unknown-elf-objdump

INC_DIRS =  -I../common  \
            -I$(hal_dir) 

SRCS = test_aes.c aes_enc.c aes_enc_ext.S aes_macro.S\
       ../common/common.c \
       $(hal_dir)/boot.S  

OUTHEX  = $(work_dir)/test_aes-$(CORE).hex
OUTBIN  = $(work_dir)/test_aes-$(CORE).bin
OUTPUT  = $(work_dir)/test_aes-$(CORE).elf
DISASM  = $(work_dir)/test_aes-$(CORE).disasm

CCFLAGS+= -O2 -march=rv32im -mabi=ilp32 -ffast-math -nostartfiles -nostdlib -fpie -Wl,-T$(hal_dir)/lscript.ld

all: $(OUTHEX) $(DISASM)

$(OUTPUT): $(SRCS)
	@echo  $(OUTPUT)
	@mkdir -p $(work_dir)
	@$(CC) $(CCFLAGS) $(INC_DIRS) -D$(firstword $(subst -, ,$(CORE))) -D$(word 2, $(subst -, ,$(CORE))) $(XD) -o $@ $^
	
$(OUTBIN): $(OUTPUT)
	@$(OBJCOPY) -O binary $< $@

$(OUTHEX): $(OUTBIN) 
	@od -t x4 -An -w4 -v $< > $@	

$(DISASM) : $(OUTPUT)
	@$(OBJDUMP) -Dt $< > $@

clean:
	rm -rf $(work_dir)
.PHONY: clean
