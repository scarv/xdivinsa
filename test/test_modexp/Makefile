work_dir ?= $(abspath ../../work/test_modexp)
hal_dir  ?= $(abspath ../../soc/$(CORE)/hal)
LIBSCARV ?= /home/thinhpham/Documents/SCARV/sw-libscarv

CORE ?= rocket-xdivinsa
XD ?= -DXDIVINSA -DXDIVINSA_T3

CONF += $(XD) -DTEST_DUMP

include $(LIBSCARV)/Makefile.conf
include $(LIBSCARV)/Makefile.arch-riscv
#CONF += -DCONF_MP_MPZ_MAX_LIMBS=34
#CONF += -DCONF_MP_MRZ_MAX_LIMBS=34

CC=$(RISCV)/bin/riscv64-unknown-elf-gcc
AS=$(RISCV)/bin/riscv64-unknown-elf-as
LD=$(RISCV)/bin/riscv64-unknown-elf-ld
OBJCOPY=$(RISCV)/bin/riscv64-unknown-elf-objcopy
OBJDUMP=$(RISCV)/bin/riscv64-unknown-elf-objdump

INC_DIRS =  -I../common  \
            -I$(hal_dir) \
            -I$(LIBSCARV)/build/riscv/include 

LLIB = $(LIBSCARV)/build/riscv/lib/libscarv.a
EXE  ?=exp_lscarv_lr


SRCS = ../common/common.c \
       $(hal_dir)/boot.S \


OUTPUT  = $(work_dir)/$(EXE:%=%.elf)
OUTBIN  = $(OUTPUT:%.elf=%.bin)
OUTHEX  = $(OUTBIN:%.bin=%.hex)


DISASM  = $(OUTPUT:%.elf=%.disasm)

all: $(OUTHEX) $(DISASM)

CCFLAGS+= -O2 -march=rv32im -mabi=ilp32 -ffast-math -nostartfiles -nostdlib -fpie -Wl,-T$(hal_dir)/lscript.ld

$(LLIB):
	$(MAKE) -C $(LIBSCARV) headers objects disasm libs ARCH=riscv CONF="$(CONF)"

$(work_dir)/$(EXE).elf: $(EXE).c $(SRCS) $(LLIB)
	@echo  $(OUTPUT)
	mkdir -p $(work_dir)
	$(CC) $(CCFLAGS) $(INC_DIRS) -D$(firstword $(subst -, ,$(CORE))) -D$(word 2, $(subst -, ,$(CORE))) $(CONF)  -o $@ $^
	
$(OUTBIN): $(OUTPUT)
	$(OBJCOPY) -O binary $< $@

$(OUTHEX): $(OUTBIN) 
	od -t x4 -An -w4 -v $< > $@	

$(DISASM) : $(OUTPUT)
	$(OBJDUMP) -Dt $< > $@

clean:
	rm -rf $(work_dir)
.PHONY: clean
