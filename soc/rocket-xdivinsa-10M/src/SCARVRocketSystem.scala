// See LICENSE.SiFive for license details.

package freechips.rocketchip.system

import Chisel._
import freechips.rocketchip.config.Parameters
import freechips.rocketchip.subsystem._
import freechips.rocketchip.devices.debug._
import freechips.rocketchip.devices.tilelink._
import freechips.rocketchip.util.DontTouch

/** Example Top with periphery devices and ports, and a Rocket subsystem */
class SCARVRocketSystem(implicit p: Parameters) extends RocketSubsystem
	with HasAsyncExtInterrupts
	with HasPeripheryDebug
    with CanHaveMasterAXI4MemPort
    with CanHaveMasterAXI4MMIOPort
//    with CanHaveSlaveAXI4Port
    with HasPeripheryBootROM{
//    with HasSystemErrorSlave{
  override lazy val module = new SCARVRocketSystemModuleImp(this)
}

class SCARVRocketSystemModuleImp[+L <: SCARVRocketSystem](_outer: L) extends RocketSubsystemModuleImp(_outer)
//	with HasRTCModuleImp
    with HasExtInterruptsModuleImp
	with HasPeripheryDebugModuleImp
    with CanHaveMasterAXI4MemPortModuleImp
    with CanHaveMasterAXI4MMIOPortModuleImp
//    with CanHaveSlaveAXI4PortModuleImp
    with HasPeripheryBootROMModuleImp
    with DontTouch

