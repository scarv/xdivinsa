CC=$(RISCV)/bin/riscv64-unknown-elf-gcc
AS=$(RISCV)/bin/riscv64-unknown-elf-as
LD=$(RISCV)/bin/riscv64-unknown-elf-ld
OBJCOPY=$(RISCV)/bin/riscv64-unknown-elf-objcopy
OBJDUMP=$(RISCV)/bin/riscv64-unknown-elf-objdump

INC_DIRS = -I../common -I$(hal_dir)
SRCS = test_xdivinsa.c \
       ../common/common.c \
       $(hal_dir)/boot.S \

OUTHEX  = $(work_dir)/test_xdivinsa-$(CORE).hex
OUTBIN  = $(work_dir)/test_xdivinsa-$(CORE).bin
OUTPUT  = $(work_dir)/test_xdivinsa-$(CORE).elf
DISASM  = $(work_dir)/test_xdivinsa-$(CORE).disasm

all: $(OUTHEX) $(DISASM)

CCFLAGS+=-Os -O2 -march=rv32im -mabi=ilp32 -ffast-math -nostartfiles -nostdlib -fpie -Wl,-T$(hal_dir)/lscript.ld

$(OUTPUT) : $(SRCS)
	mkdir -p $(work_dir)
	$(CC) $(CCFLAGS) $(INC_DIRS) -D$(firstword $(subst -, ,$(CORE))) -D$(word 2, $(subst -, ,$(CORE))) -DXDIVINSA=3 -o $@ $^
	
$(OUTBIN): $(OUTPUT)
	$(OBJCOPY) -O binary $< $@

$(OUTHEX): $(OUTBIN) 
	od -t x4 -An -w4 -v $< > $@	

$(DISASM) : $(OUTPUT)
	$(OBJDUMP) -Dt $< > $@

clean:
	rm -rf $(work_dir)
.PHONY: clean
