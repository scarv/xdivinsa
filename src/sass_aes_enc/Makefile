work_dir ?= $(abspath ../../work/sass_aes_enc)
hal_dir  ?= $(abspath ../../soc/$(CORE)/hal)
sass_dir ?= $(abspath ../../external/fw-acquisition)
#LIBSCARV ?= /home/thinhpham/Documents/SCARV/sw-libscarv

CORE ?= rocket-xdivinsa
XD ?= -DXDIVINSA=6


#CONF += $(XD)
#include $(LIBSCARV)/Makefile.conf
#include $(LIBSCARV)/Makefile.arch-riscv

CC=$(RISCV)/bin/riscv64-unknown-elf-gcc
AS=$(RISCV)/bin/riscv64-unknown-elf-as
LD=$(RISCV)/bin/riscv64-unknown-elf-ld
OBJCOPY=$(RISCV)/bin/riscv64-unknown-elf-objcopy
OBJDUMP=$(RISCV)/bin/riscv64-unknown-elf-objdump

INC_DIRS =  -I../common  \
            -I$(hal_dir) \
            -I$(sass_dir)/target 
#            -I$(LIBSCARV)/build/riscv/include \
#			-I$(LIBSCARV)/build/riscv/include/scarv 

#LLIB = $(LIBSCARV)/build/riscv/lib/libscarv.a

SRCS = sass_aes_enc.c aes_enc.c aes_enc_ext.S aes_macro.S\
       ../common/common.c \
       $(hal_dir)/boot.S  \
       $(sass_dir)/target/sasstarget.c \

OUTHEX  = $(work_dir)/sass_aes_enc-$(CORE).hex
OUTBIN  = $(work_dir)/sass_aes_enc-$(CORE).bin
OUTPUT  = $(work_dir)/sass_aes_enc-$(CORE).elf
DISASM  = $(work_dir)/sass_aes_enc-$(CORE).disasm


all: $(OUTHEX) $(DISASM)

CCFLAGS+= -O2 -march=rv32im -mabi=ilp32 -ffast-math -nostartfiles -nostdlib -fpie -Wl,-T$(hal_dir)/lscript.ld

#$(LLIB):
#	$(MAKE) -C $(LIBSCARV) headers objects disasm libs ARCH=riscv CONF="$(CONF)"

$(OUTPUT): $(SRCS)
	@echo  $(OUTPUT)
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
