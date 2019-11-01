
#--------------------------------------------------------------------
# global define
#--------------------------------------------------------------------

ifndef XILINX_VIVADO
$(error Please set environment variable XILINX_VIVADO for Xilinx tools)
endif

ifndef BSX_HOME
$(error Please set environment for Sakura-X board support package)
endif

XDI_HOME ?= $(abspath ../../)
CORE      = picorv-xdivinsa
TARGET   ?= sakura-x
IMPL     ?= d0


extr_dir ?= $(abspath ../../external)
work_dir ?= $(abspath ../../work)

make_dir = $(abspath .)
core_dir = $(make_dir)/picorv_src


project_name = $(TARGET)-$(CORE)-imp

VIVADO = cd $(work_dir)/$(project_name) && vivado
UPDMEM = cd $(work_dir)/$(project_name) && updatemem
include $(BSX_HOME)/board/$(TARGET)/script/bsp


#--------------------------------------------------------------------
# Sources
#--------------------------------------------------------------------
verilog_picorv32 =$(core_dir)/picorv32.v 

rtl_sources = \
	$(work_dir)/$(project_name)/rtl_sources/system_top_wrapper.v \
	$(work_dir)/$(project_name)/rtl_sources/picorv32.v \
	$(work_dir)/$(project_name)/rtl_sources/cop_ise.v \
	$(work_dir)/$(project_name)/rtl_sources/es_trng.v \
	$(work_dir)/$(project_name)/rtl_sources/trng_reg.v \

prog_mem = $(work_dir)/$(project_name)/prog-bin/prog.mem

#--------------------------------------------------------------------
# Build Verilog
#--------------------------------------------------------------------

verilog: $(verilog_picorv32) $(rtl_cop) $(make_dir)/system_top_wrapper.v
	mkdir -p $(work_dir)/$(project_name)/rtl_sources
	mkdir -p $(work_dir)/$(project_name)/prog-bin
	cp -f $(make_dir)/system_top_wrapper.v	$(work_dir)/$(project_name)/rtl_sources/system_top_wrapper.v
	cp -f $(verilog_picorv32) 				$(work_dir)/$(project_name)/rtl_sources/picorv32.v
	cp -f $(XDI_HOME)/rtl/*.v                                     $(work_dir)/$(project_name)/rtl_sources/
	cp -f $(XDI_HOME)/rtl/xdivinsa.v.$(IMPL)	                  $(work_dir)/$(project_name)/rtl_sources/cop_ise.v	

.PHONY: verilog

#--------------------------------------------------------------------
# Build Verilog testbench
#--------------------------------------------------------------------

testbench: $(work_dir)/$(project_name)/rtl_sources/cop_ise.v $(XDI_HOME)/rtl/xdivinsa.vt
	mkdir -p $(work_dir)/rtl_testbench
	cp -f  $(work_dir)/$(project_name)/rtl_sources/cop_ise.v $(work_dir)/rtl_testbench
	cp -f  $(XDI_HOME)/rtl/xdivinsa.vt                           $(work_dir)/rtl_testbench/cop_ise_tb.v
	iverilog -g2012 -o $(work_dir)/rtl_testbench/isim -s cop_ise_tb $(work_dir)/rtl_testbench/cop_ise.v $(work_dir)/rtl_testbench/cop_ise_tb.v
	vvp -l $(work_dir)/rtl_testbench/simlog $(work_dir)/rtl_testbench/isim +WAVES=$(work_dir)/rtl_testbench/tbwaveform.vcd 
.PHONY: testbench

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
	$(VIVADO) -mode batch -source $(make_dir)/script/make_project.tcl -tclargs $(project_name) $(BSX_HOME) $(work_dir) $(part) $(TARGET)
#	ln -s $(prog_mem) $(work_dir)/$(project_name)/$(project_name).runs/synth_1/prog.mem
#	ln -s $(prog_mem) $(soc_dir)/$(project_name)/$(project_name).sim/sim_1/behav/prog.mem

vivado: $(project)
	$(VIVADO) $(project) &

bitstream = $(work_dir)/$(project_name)/$(project_name).runs/impl_1/system_top_wrapper.bit
bitstream: $(work_dir)/$(project_name)/prog.mmi 
$(bitstream): $(rtl_sources) | $(project)
	$(VIVADO) -mode batch -source $(BSX_HOME)/soc/script/make_bitstream.tcl -tclargs $(project_name) $(work_dir)

program: $(bitstream)
	$(VIVADO) -mode batch -source $(BSX_HOME)/soc/script/program.tcl -tclargs $(device) $(bitstream)

.PHONY: project vivado bitstream program

#--------------------------------------------------------------------
# Debug helper
#--------------------------------------------------------------------

search-ramb: $(work_dir)/$(project_name)/prog.mmi
$(work_dir)/$(project_name)/prog.mmi: $(bitstream)
	$(VIVADO) -mode batch -source $(BSX_HOME)/soc/script/search_ramb.tcl -tclargs $(project_name) $(work_dir) > $(work_dir)/$(project_name)/search-ramb.info
	python $(BSX_HOME)/soc/script/mmi_gen.py $(work_dir)/$(project_name)/search-ramb.info $(work_dir)/$(project_name)/prog.mmi 32 32768 $(part)

bit-update: $(work_dir)/$(project_name)/$(project_name).runs/impl_1/system_top_wrapper.new.bit
$(work_dir)/$(project_name)/$(project_name).runs/impl_1/system_top_wrapper.new.bit: $(prog_mem) $(work_dir)/$(project_name)/prog.mmi
#	data2mem -bm $(soc_dir)/prog-bin/prog.bmm -bd $< -bt $(bitstream) -o b $@
	$(UPDMEM) -force -meminfo $(work_dir)/$(project_name)/prog.mmi -data $(prog_mem) -bit $(bitstream) -proc dummy -out $@
program-updated: $(work_dir)/$(project_name)/$(project_name).runs/impl_1/system_top_wrapper.new.bit
	$(VIVADO) -mode batch -source $(BSX_HOME)/soc/script/program.tcl -tclargs $(device) $(work_dir)/$(project_name)/$(project_name).runs/impl_1/system_top_wrapper.new.bit

.PHONY: search-ramb bit-update program-updated

#--------------------------------------------------------------------
# Clean up
#--------------------------------------------------------------------

clean:
	rm -rf $(work_dir)/$(project_name)/*.log $(work_dir)/$(project_name)/*.jou $(junk)
cleanall: clean
	rm -fr $(work_dir)/$(project_name)

.PHONY: clean cleanall
