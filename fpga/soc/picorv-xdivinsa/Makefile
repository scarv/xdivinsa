
#--------------------------------------------------------------------
# global define
#--------------------------------------------------------------------

ifndef XILINX_VIVADO
$(error Please set environment variable XILINX_VIVADO for Xilinx tools)
endif

ifndef XDI_HOME
    $(error "Please run 'source ./bin/source.me.sh' to setup the project workspace")
endif

CORE      = picorv-xdivinsa
BOARD    ?= sakura-x
work_dir ?= $(abspath ../../work)

make_dir = $(abspath .)
core_dir = $(make_dir)/picorv_rtl

project_name = $(BOARD)-$(CORE)-imp

VIVADO = cd $(work_dir)/$(project_name) && vivado
UPDMEM = cd $(work_dir)/$(project_name) && updatemem
include $(XDI_HOME)/fpga/board/$(BOARD)/script/bsp

#--------------------------------------------------------------------
# Sources
#--------------------------------------------------------------------
rtl_sources = \
	$(work_dir)/$(project_name)/rtl_sources/system_top_wrapper.v \
	$(work_dir)/$(project_name)/rtl_sources/picorv32.v \
	$(work_dir)/$(project_name)/rtl_sources/xdivinsa.v \
	$(work_dir)/$(project_name)/rtl_sources/es_trng.v \
	$(work_dir)/$(project_name)/rtl_sources/trng_reg.v \

prog_mem = $(work_dir)/$(project_name)/prog-bin/prog.mem

#--------------------------------------------------------------------
# Build Verilog
#--------------------------------------------------------------------
verilog: $(make_dir)/system_top_wrapper.v $(core_dir)/*.v $(XDI_HOME)/rtl/*.v
	mkdir -p $(work_dir)/$(project_name)/rtl_sources
	mkdir -p $(work_dir)/$(project_name)/prog-bin
	cp -f $(make_dir)/system_top_wrapper.v	$(work_dir)/$(project_name)/rtl_sources/system_top_wrapper.v
	cp -f $(core_dir)/*.v 				    $(work_dir)/$(project_name)/rtl_sources/
	cp -f $(XDI_HOME)/rtl/*.v               $(work_dir)/$(project_name)/rtl_sources/
#	cp -f $(XDI_HOME)/rtl/xdivinsa.v	    $(work_dir)/$(project_name)/rtl_sources/cop_ise.v	

.PHONY: verilog

#--------------------------------------------------------------------
# Project generation
#--------------------------------------------------------------------
default: project
.PHONY: default

project = $(work_dir)/$(project_name)/$(project_name).xpr
project: $(project)
$(project): | $(rtl_sources)
	$(shell sed -i 's/CONFIG.PRIM_SOURCE.*/''CONFIG.PRIM_SOURCE {$(clktype)} \\''/g' script/make_project.tcl)
	$(shell sed -i 's/CONFIG.PRIM_IN_FREQ.*/''CONFIG.PRIM_IN_FREQ {$(inclock)} \\''/g' script/make_project.tcl)
	$(shell sed -i 's/set_property verilog_define.*/''set_property verilog_define \[list FPGA $(clktype)\] \[get_filesets sources_1\] ''/g' script/make_project.tcl)
	mkdir -p $(work_dir)/$(project_name)
	$(VIVADO) -mode batch -source $(make_dir)/script/make_project.tcl -tclargs $(project_name) $(XDI_HOME)/fpga $(work_dir) $(part) $(BOARD)

vivado: $(project)
	$(VIVADO) $(project) &

bitstream = $(work_dir)/$(project_name)/$(project_name).runs/impl_1/system_top_wrapper.bit
bitstream: $(work_dir)/$(project_name)/prog.mmi 
$(bitstream): $(rtl_sources) | $(project)
	$(VIVADO) -mode batch -source $(XDI_HOME)/fpga/script/make_bitstream.tcl -tclargs $(project_name) $(work_dir)

program: $(bitstream)
	$(VIVADO) -mode batch -source $(XDI_HOME)/fpga/script/program.tcl -tclargs $(device) $(bitstream)

.PHONY: project vivado bitstream program

#--------------------------------------------------------------------
# Debug helper
#--------------------------------------------------------------------
search-ramb: $(work_dir)/$(project_name)/prog.mmi
$(work_dir)/$(project_name)/prog.mmi: $(bitstream)
	$(VIVADO) -mode batch -source $(XDI_HOME)/fpga/script/search_ramb.tcl -tclargs $(project_name) $(work_dir) > $(work_dir)/$(project_name)/search-ramb.info
	python $(XDI_HOME)/fpga/script/mmi_gen.py $(work_dir)/$(project_name)/search-ramb.info $(work_dir)/$(project_name)/prog.mmi 32 32768 $(part)

bit-update: $(work_dir)/$(project_name)/$(project_name).runs/impl_1/system_top_wrapper.new.bit
$(work_dir)/$(project_name)/$(project_name).runs/impl_1/system_top_wrapper.new.bit: $(prog_mem) $(work_dir)/$(project_name)/prog.mmi
	$(UPDMEM) -force -meminfo $(work_dir)/$(project_name)/prog.mmi -data $(prog_mem) -bit $(bitstream) -proc dummy -out $@
program-updated: $(work_dir)/$(project_name)/$(project_name).runs/impl_1/system_top_wrapper.new.bit
	$(VIVADO) -mode batch -source $(XDI_HOME)/fpga/script/program.tcl -tclargs $(device) $(work_dir)/$(project_name)/$(project_name).runs/impl_1/system_top_wrapper.new.bit

.PHONY: search-ramb bit-update program-updated

#--------------------------------------------------------------------
# Clean up
#--------------------------------------------------------------------

clean:
	rm -rf $(work_dir)/$(project_name)/*.log $(work_dir)/$(project_name)/*.jou $(junk)
cleanall: clean
	rm -fr $(work_dir)/$(project_name)

.PHONY: clean cleanall
