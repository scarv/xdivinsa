// See LICENSE.SiFive for license details.
// See LICENSE.Berkeley for license details.

package freechips.rocketchip.system

import Chisel._
import freechips.rocketchip.config._
import freechips.rocketchip.subsystem._
import freechips.rocketchip.tile._
import freechips.rocketchip.rocket._
import freechips.rocketchip.devices.debug.{IncludeJtagDTM, JtagDTMKey}
import freechips.rocketchip.diplomacy._

class WithSCARVTinyCore extends Config((site, here, up) => {
  case XLen => 32
  case RocketTilesKey => List(RocketTileParams(
      core = RocketCoreParams(
        useVM = false,
//        fpu = None,
        fpu = Some(FPUParams(fLen = 32)),
        mulDiv = Some(MulDivParams(mulUnroll = 8))),
      btb = None,
      dcache = Some(DCacheParams(
        rowBits = site(SystemBusKey).beatBits,
        nSets = 256, // 16Kb scratchpad
        nWays = 1,
        nTLBEntries = 4,
        nMSHRs = 0,
        blockBytes = site(CacheBlockBytes),
        scratch = Some(0x80000000L))),
      icache = Some(ICacheParams(
        rowBits = site(SystemBusKey).beatBits,
        nSets = 64,
        nWays = 1,
        nTLBEntries = 4,
        blockBytes = site(CacheBlockBytes)))))
  case RocketCrossingKey => List(RocketCrossingParams(
    crossingType = SynchronousCrossing(),
    master = TileMasterPortParams()
  ))
})

class WithISERoCC extends Config((site, here, up) => {
  case BuildRoCC => List(
    (p: Parameters) => {
        val CoP_ISE = LazyModule(new RoCC_ISE(OpcodeSet.custom0)(p))
        CoP_ISE
    })
})

class SCARVSysConfig extends Config(
  new WithNoMemPort ++
  new WithNMemoryChannels(0) ++
  new WithSCARVTinyCore ++
  new BaseConfig)

class SCARVRocketCoPConfig extends Config(
	new WithNBreakpoints(2)	++
	new WithJtagDTM  ++
	//new WithCiRoCC ++
	new WithISERoCC ++
	//new TinyConfig ++ 
    new SCARVSysConfig ++
	new BaseConfig)




