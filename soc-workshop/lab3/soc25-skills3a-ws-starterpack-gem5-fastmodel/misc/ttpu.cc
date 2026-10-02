//
// ttpu.cc - (C) 2025 DJ Greaves, University of Cambridge, UK.
//

/** @file
 * Demonstation GEM5 Generic/base class for Trusted Processing Units.
 * Implements a base class for the SoC-25 demo/toy TPU family (there is only one).
 */

#include "dev/misc/ttpu.hh"

namespace gem5
{


  // A basic programmed I/O device.
Ttpu::Ttpu(const Params &p, Addr pio_size) :
    BasicPioDevice(p, pio_size), platform(p.platform), device(p.device)
{
    status = 0;
}

} // namespace gem5
