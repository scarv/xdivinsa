ifndef XDI_HOME
    $(error "Please run 'source ./bin/source.me.sh' to setup the project workspace")
endif

export work_dir     = $(XDI_HOME)/work
export BOARD	   ?= sakura-x
export CORE	       ?= picorv-xdivinsa
export prog_mem     = $(work_dir)/$(BOARD)-$(CORE)-imp/prog-bin/prog.mem

export PORT        ?= /dev/ttyUSB0
export BAUD        ?= 115200   
export NUM_TRACES  ?= 1000
export T_FUNC      ?= AES
export SASS_RIG    ?= $(abspath ./external/fw-acquisition)
export TraceFile   ?= traces.trs

.PHONY: toolchain
toolchain:
	$(XDI_HOME)/toolchain/clone.sh
	$(XDI_HOME)/toolchain/build-xdi-rv32.sh

.PHONY: fpga-project vivado bitstream verilog acquisition-firmware bit-update program-fpga
verilog:
	$(MAKE) -C $(XDI_HOME)/fpga/soc/$(CORE) verilog          work_dir=$(work_dir) 
fpga-project:
	$(MAKE) -C $(XDI_HOME)/fpga/soc/$(CORE) project          work_dir=$(work_dir) 
vivado:
	$(MAKE) -C $(XDI_HOME)/fpga/soc/$(CORE) vivado           work_dir=$(work_dir) 
bitstream:
	$(MAKE) -C $(XDI_HOME)/fpga/soc/$(CORE) bitstream        work_dir=$(work_dir)
bit-update:
	$(MAKE) -C $(XDI_HOME)/fpga/soc/$(CORE) bit-update       work_dir=$(work_dir)
program-fpga:
	$(MAKE) -C $(XDI_HOME)/fpga/soc/$(CORE) program-updated  work_dir=$(work_dir)


.PHONY: helloworld test-xdivinsa
helloworld:
	$(MAKE) -C $(XDI_HOME)/src/helloworld all CORE=$(CORE) work_dir=$(work_dir)/helloworld hal_dir=$(XDI_HOME)/fpga/soc/$(CORE)/hal
	{ echo '@00000000'; cat $(work_dir)/helloworld/helloworld-$(CORE).hex;} >$(prog_mem)

test-xdivinsa:
	$(MAKE) -C $(XDI_HOME)/src/test_xdivinsa all CORE=$(CORE) work_dir=$(work_dir)/test_xdivinsa hal_dir=$(XDI_HOME)/fpga/soc/$(CORE)/hal
	{ echo '@00000000'; cat $(work_dir)/test_xdivinsa/test_xdivinsa-$(CORE).hex;} >$(prog_mem)
build-aes:
	$(MAKE) -C $(XDI_HOME)/src/test_aes clean all CORE=$(CORE) work_dir=$(work_dir)/test_aes hal_dir=$(XDI_HOME)/fpga/soc/$(CORE)/hal
	{ echo '@00000000'; cat $(work_dir)/test_aes/test_aes-$(CORE).hex;} >$(prog_mem)
build-protected-aes:
	$(MAKE) -C $(XDI_HOME)/src/test_aes clean all CORE=$(CORE) work_dir=$(work_dir)/test_aes hal_dir=$(XDI_HOME)/fpga/soc/$(CORE)/hal XD="-DXDIVINSA=3"
	{ echo '@00000000'; cat $(work_dir)/test_aes/test_aes-$(CORE).hex;} >$(prog_mem)

#--------------------------------------------------------------------
# Clean up
#--------------------------------------------------------------------
clean: clean-soft clean-proj
clean-soft:
	$(MAKE) -C $(XDI_HOME)/src/helloworld    clean
	$(MAKE) -C $(XDI_HOME)/src/test-xdivinsa clean
clean-proj:
	$(MAKE) -C $(XDI_HOME)/fpga/soc/$(CORE)  cleanall
cleanall: clean clean-proj 
.PHONY: clean clean-soft clean-proj cleanall
