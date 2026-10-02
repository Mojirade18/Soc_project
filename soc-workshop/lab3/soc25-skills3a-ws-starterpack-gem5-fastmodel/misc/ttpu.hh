
// ttpu.hh - (C) 2025 DJ Greaves, University of Cambridge, UK.
// Demonstation GEM5 Generic/base class for Trusted Processing Units.

/** @file
 * // Demonstation GEM5 Generic/base class for Trusted Processing Units.
 */

#ifndef __TTPU_HH__
#define __TTPU_HH__

#include "base/callback.hh"
#include "dev/io_device.hh"

#include "params/Ttpu.hh"

namespace gem5
{

class Platform;


class Ttpu : public BasicPioDevice
{
  protected:
    int status;
    Platform *platform;
    SerialDevice *device;

  public:
    using Params = TtpuParams;
    Ttpu(const Params &p, Addr pio_size);

    /**
     * Returns whether we have an interrupt pending.
     */
    bool intStatus() { return status ? true : false; }
};

} // namespace gem5

#endif // __TTPU_HH__
