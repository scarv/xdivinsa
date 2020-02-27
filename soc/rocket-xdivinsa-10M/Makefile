
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
CORE      = rocket-xdivinsa-10M
TARGET   ?= sakura-x
IMPL     ?= d1

extr_dir ?= $(abspath ../../external)
work_dir ?= $(abspath ../../work)

make_dir  = $(abspath .)
scal_dir  = $(make_dir)/src

core_dir  = $(extr_dir)/rocket-chip

coregen_dir  = $(core_dir)/vsim/generated-src

project_name = $(TARGET)-$(CORE)-imp

VIVADO = cd $(work_dir)/$(project_name) && vivado
UPDMEM = cd $(work_dir)/$(project_name) && updatemem
include $(BSX_HOME)/board/$(TARGET)/script/bsp


#--------------------------------------------------------------------
# Sources
#--------------------------------------------------------------------
rtl_sources = \
	$(work_dir)/$(project_name)/rtl_sources/system_top_wrapper.v \
	$(work_dir)/$(project_name)/rtl_sources/rocketcore.v \
	$(work_dir)/$(project_name)/rtl_sources/plusarg_reader.v \
	$(work_dir)/$(project_name)/rtl_sources/AsyncResetReg.v \
	$(work_dir)/$(project_name)/rtl_sources/cop_ise.v \
	$(work_dir)/$(project_name)/rtl_sources/es_trng.v \
	$(work_dir)/$(project_name)/rtl_sources/trng_reg.v \

prog_mem = $(work_dir)/$(project_name)/prog-bin/prog.mem

#--------------------------------------------------------------------
# Build Verilog
#--------------------------------------------------------------------
ROCKET = freechips.rocketchip.system
MODEL  = SCARVRocketTest
CONFIG = SCARVRocketCoPConfig
verilog: 
	$(MAKE) -C $(make_dir)/bootrom all
	cp -f $(make_dir)/bootrom/bootrom.img             $(core_dir)/bootrom/bootrom.img
	ln -s -f $(scal_dir)/SCARVRocketCoPConfig.scala   $(core_dir)/src/main/scala/system/SCARVRocketCoPConfig.scala
	ln -s -f $(scal_dir)/SCARVRocketSystem.scala      $(core_dir)/src/main/scala/system/SCARVRocketSystem.scala
	ln -s -f $(scal_dir)/SCARVRocketTest.scala        $(core_dir)/src/main/scala/system/SCARVRocketTest.scala
	ln -s -f $(scal_dir)/scarv_rocc.scala   		  $(core_dir)/src/main/scala/tile/scarv_rocc.scala
	$(MAKE) -C $(core_dir)/vsim verilog CONFIG=$(CONFIG) MODEL=$(MODEL)
#
	mkdir -p $(work_dir)/$(project_name)/rtl_sources
	mkdir -p $(work_dir)/$(project_name)/prog-bin
	cp -f $(make_dir)/system_top_wrapper.v                        $(work_dir)/$(project_name)/rtl_sources/system_top_wrapper.v
	cp -f $(coregen_dir)/$(ROCKET).$(CONFIG).v                    $(work_dir)/$(project_name)/rtl_sources/rocketcore.v
	cp -f $(coregen_dir)/$(ROCKET).$(CONFIG).behav_srams.v        $(work_dir)/$(project_name)/rtl_sources/memcore.v
	cp -f $(core_dir)/src/main/resources/vsrc/plusarg_reader.v    $(work_dir)/$(project_name)/rtl_sources/plusarg_reader.v
	cp -f $(core_dir)/src/main/resources/vsrc/AsyncResetReg.v     $(work_dir)/$(project_name)/rtl_sources/AsyncResetReg.v
	cp -f $(XDI_HOME)/rtl/*.v                                     $(work_dir)/$(project_name)/rtl_sources/
	cp -f $(XDI_HOME)/rtl/xdivinsa.v.$(IMPL)	                  $(work_dir)/$(project_name)/rtl_sources/cop_ise.v	
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
	$(VIVADO) -mode batch -source $(make_dir)/script/make_project.tcl -tclargs $(project_name) $(make_dir) $(work_dir) $(part) $(TARGET) 

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
	$(UPDMEM) -force -meminfo $(work_dir)/$(project_name)/prog.mmi -data $(prog_mem) -bit $(bitstream) -proc dummy -out $@

program-updated: $(work_dir)/$(project_name)/$(project_name).runs/impl_1/system_top_wrapper.new.bit
	$(VIVADO) -mode batch -source $(BSX_HOME)/soc/script/program.tcl -tclargs $(device) $(work_dir)/$(project_name)/$(project_name).runs/impl_1/system_top_wrapper.new.bit

.PHONY: search-ramb bit-update program-updated dis_bit

#--------------------------------------------------------------------
# Clean up
#--------------------------------------------------------------------

clean:
	rm -rf $(work_dir)/$(project_name)/*.log $(work_dir)/$(project_name)/*.jou $(junk)
cleanall: clean
	rm -fr $(work_dir)/$(project_name)

.PHONY: clean cleanall
