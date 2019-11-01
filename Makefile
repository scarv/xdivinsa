ifndef XDI_HOME
    $(error "Please run 'source ./bin/source.me.sh' to setup the project workspace")
endif

ifndef BSX_HOME
$(error Please set environment for Sakura-X board support package)
endif

export work_dir = $(XDI_HOME)/work

export PORT        ?= /dev/ttyUSB0
export BAUD        ?= 115200   
#export BAUD        ?= 57600
export NUM_TRACES  ?= 1000

export SASS_RIG    ?=$(abspath ./external/fw-acquisition)

CORE	?= rocket-xdivinsa
TARGET	= sakura-x

soc_dir ?= $(XDI_HOME)/soc

prog_mem = $(work_dir)/$(TARGET)-$(CORE)-imp/prog-bin/prog.mem

OPT     ?= BEQ
EXE     ?= CASE1

.PHONY: project-soc vivado bitstream verilog acquisition-firmware bit-update program-fpga
verilog:
	$(MAKE) -C $(BSX_HOME) verilog          work_dir=$(work_dir) 
project-soc:
	$(MAKE) -C $(BSX_HOME) project-soc      work_dir=$(work_dir) TARGET=$(TARGET)
vivado:
	$(MAKE) -C $(BSX_HOME) vivado           work_dir=$(work_dir) 
bitstream:
	$(MAKE) -C $(BSX_HOME) bitstream        work_dir=$(work_dir)
bit-update:
	$(MAKE) -C $(BSX_HOME) bit-update       work_dir=$(work_dir)
program-fpga:
	$(MAKE) -C $(BSX_HOME) program-fpga     work_dir=$(work_dir)

tb:
	$(MAKE) -C $(soc_dir)/$(CORE) testbench work_dir=$(work_dir)

helloworld:
	$(MAKE) -C $(BSX_HOME)/examples/helloworld all CORE=$(CORE) work_dir=$(work_dir)/helloworld hal_dir=$(soc_dir)/$(CORE)/hal
	{ echo '@00000000'; cat $(work_dir)/helloworld/helloworld-$(CORE).hex;} >$(prog_mem)
led-flash:
	$(MAKE) -C $(BSX_HOME)/examples/led_flash all CORE=$(CORE) work_dir=$(work_dir)/led_flash hal_dir=$(soc_dir)/$(CORE)/hal
	{ echo '@00000000'; cat $(work_dir)/led_flash/led_flash-$(CORE).hex;} >$(prog_mem)

test-xdivinsa:
	$(MAKE) -C $(XDI_HOME)/test/test_xdivinsa all CORE=$(CORE) work_dir=$(work_dir)/test_xdivinsa
	{ echo '@00000000'; cat $(work_dir)/test_xdivinsa/test_xdivinsa-$(CORE).hex;} >$(prog_mem)
test-modexp:
	@rm -r -f $(work_dir)/test_modexp
	$(MAKE) -C $(XDI_HOME)/test/test_modexp all CORE=$(CORE) work_dir=$(work_dir)/test_modexp  sass_dir=$(SASS_RIG) 
	{ echo '@00000000'; cat $(work_dir)/test_modexp/*.hex;} >$(prog_mem)
sass-xdivinsa:
	$(MAKE) -C $(XDI_HOME)/src/sass_xdivinsa all CORE=$(CORE) work_dir=$(work_dir)/sass_xdivinsa  sass_dir=$(SASS_RIG)
	{ echo '@00000000'; cat $(work_dir)/sass_xdivinsa/sass_xdivinsa-$(CORE).hex;} >$(prog_mem)
sass-modexp:
	$(MAKE) -C $(XDI_HOME)/src/sass_modexp all CORE=$(CORE) work_dir=$(work_dir)/sass_modexp  sass_dir=$(SASS_RIG)
	{ echo '@00000000'; cat $(work_dir)/sass_modexp/sass_modexp-$(CORE).hex;} >$(prog_mem)



t-func-verify:
	$(MAKE) -C $(XDI_HOME)/flow/acquisition t-func-verify
t-func-ttest:
	$(MAKE) -C $(XDI_HOME)/flow/acquisition capture-t-func-ttest
t-func-ttest-eval:
	$(MAKE) -C $(XDI_HOME)/flow/acquisition t-func-ttest-eval

.PHONY: helloworld led-flash test-cop
#--------------------------------------------------------------------
# Clean up
#--------------------------------------------------------------------
clean: clean-soft clean-proj
clean-soft:
	$(MAKE) -C $(BSX_HOME)/examples/helloworld clean
	$(MAKE) -C $(BSX_HOME)/examples/led_flash clean
clean-proj:
	$(MAKE) -C $(BSX_HOME) clean
clean-hard:
	$(MAKE) -C $(BSX_HOME) cleanall
cleanall: clean clean-hard
.PHONY: clean clean-soft clean-proj clean-hard cleanall
