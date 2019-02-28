// See LICENSE for license details.

package freechips.rocketchip.tile

import Chisel._
import freechips.rocketchip.config._
import freechips.rocketchip.rocket._

class cop_ise extends BlackBox {
	val io = new Bundle() {
		val cop_clk 	= Clock(INPUT)
		val cop_rst 	= Bool(INPUT)
		val cop_valid = Bool(INPUT)
		val cop_ready = Bool(OUTPUT)
		val cop_wait  = Bool(OUTPUT)
		val cop_wr 	  = Bool(OUTPUT)		
		val cop_insn = Bits(INPUT ,width= 32)
		val cop_rs1  = Bits(INPUT ,width= 32)
		val cop_rs2  = Bits(INPUT ,width= 32)
		val cop_rd   = Bits(OUTPUT,width= 32)	}
}

//RoCC Black box which will then be defined in Verilog after generating RC system.
class RoCC_ISE(opcodes: OpcodeSet)(implicit p: Parameters) extends LazyRoCC(opcodes) {
  override lazy val module = new RoCC_ISE_Imp(this)
}

class RoCC_ISE_Imp(outer: RoCC_ISE)(implicit p: Parameters) extends LazyRoCCModuleImp(outer)
    with HasCoreParameters {
	
  val cmd = Queue(io.cmd)
  val insn= Cat(cmd.bits.inst.funct,
				cmd.bits.inst.rs2,
				cmd.bits.inst.rs1,
				cmd.bits.inst.xd,
				cmd.bits.inst.xs1,
				cmd.bits.inst.xs2,
				cmd.bits.inst.rd,
				cmd.bits.inst.opcode)

  val CoP_BB = Module(new cop_ise())
	CoP_BB.io.cop_clk 	:= clock
	CoP_BB.io.cop_rst 	:= reset
  CoP_BB.io.cop_valid <> cmd.fire()
	CoP_BB.io.cop_insn  <> insn
	CoP_BB.io.cop_rs1		<> cmd.bits.rs1
	CoP_BB.io.cop_rs2		<> cmd.bits.rs2
	
	io.busy 					:= 	CoP_BB.io.cop_wait
	io.resp.valid			:= 	CoP_BB.io.cop_wr
	io.resp.bits.data	:=  CoP_BB.io.cop_rd

  val rdreg = Reg(UInt(width = 5))
  when (cmd.fire()) {
		rdreg := cmd.bits.inst.rd
  }
	
	val cop_isworking = Reg(init=Bool(false))
	when (cmd.fire()) {
		cop_isworking := Bool(true)
	}.elsewhen (CoP_BB.io.cop_ready){
		cop_isworking := Bool(false)
	}
	cmd.ready := (~cop_isworking)	|| CoP_BB.io.cop_ready
  // command resolved if no stalls AND not issuing a load that will need a request

  // PROC RESPONSE INTERFACE
  //io.resp.valid := cmd.valid && doResp && !stallReg && !stallLoad 
  //io.resp.valid := RandVal //&& cmd.valid && doResp && !stallReg && !stallLoad
//  io.resp.valid := resval 
    // valid response if valid command, need a response, and no stalls
  io.resp.bits.rd := rdreg
    // Must respond with the appropriate tag or undefined behavior
//  io.resp.bits.data := resreg
    // Semantics is to always send out prior accumulator register value

//  io.busy := cmd.valid
    // Be busy when have pending memory requests or committed possibility of pending requests
  io.interrupt := Bool(false)
    // Set this true to trigger an interrupt on the processor (please refer to supervisor documentation)

  // MEMORY REQUEST INTERFACE
  io.mem.req.valid := Bool(false) // we're not performing any memory request ...
  io.mem.req.bits.addr := cmd.bits.rs1
  io.mem.req.bits.tag := Bits(0)
  io.mem.req.bits.cmd := M_XRD // perform a load (M_XWR for stores)
  io.mem.req.bits.typ := MT_D // D = 8 bytes, W = 4, H = 2, B = 1
  io.mem.req.bits.data := Bits(0) // we're not performing any stores...
  io.mem.req.bits.phys := Bool(false)
}

