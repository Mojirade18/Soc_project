

// ttpu101.hh - (C) 2025 DJ Greaves, University of Cambridge.
// GEM5 C++ wrapper for the Toy trusted processing C model.

/*
 * Copyright (c) 2010-2015 ARM Limited
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


/** @file
 * Implementiation of a Toy TPU --- the ttpu101
 */

#ifndef __DEV_ARM_TTPU101_H__
#define __DEV_ARM_TTPU101_H__

#include "dev/arm/amba_device.hh"
#include "dev/misc/ttpu.hh"

#include "soc25-ttpu.h"

namespace gem5
{

class BaseGic;

struct TtpuParams;   // Automatically generated from the Python

class Ttpu101 : public Ttpu, public AmbaDevice
{

  SOC25_TTPU_CBG_state ttpu_state; // Device's internal state.
  
public:
  Ttpu101(const TtpuParams &p); // Constructor
   
  void serialize(CheckpointOut &cp) const override;
  void unserialize(CheckpointIn &cp) override;
  
public: // Main programmable I/O functions - TLM style
  Tick read(PacketPtr pkt) override;
  Tick write(PacketPtr pkt) override;







  
public: // Ttpu
  //  void dataAvailable() override;
  
  
protected: // Interrupt handling
  /** Function to generate interrupt */
  void generateInterrupt();
  
  /**
     * Assign new interrupt values and update interrupt signals
     *
     * A new interrupt is scheduled signalled if the set of unmasked
     * interrupts goes empty to non-empty. Conversely, if the set of
     * unmasked interrupts goes from non-empty to empty, the interrupt
     * signal is cleared.
     *
     * @param ints New <i>raw</i> interrupt status
     * @param mask New interrupt mask
     */
    void setInterrupts(uint16_t ints, uint16_t mask);

#if 0
  /**
     * Convenience function to update the interrupt mask
     *
     * @see setInterrupts
     * @param mask New interrupt mask
     */
    void setInterruptMask(uint16_t mask) { setInterrupts(rawInt, mask); }
    /**
     * Convenience function to raise a new interrupt
     *
     * @see setInterrupts
     * @param ints Set of interrupts to raise
     */
    void raiseInterrupts(uint16_t ints) { setInterrupts(rawInt | ints, imsc); }
    /**
     * Convenience function to clear interrupts
     *
     * @see setInterrupts
     * @param ints Set of interrupts to clear
     */
    void clearInterrupts(uint16_t ints) { setInterrupts(rawInt & ~ints, imsc); }

    /** Masked interrupt status register */
    inline uint16_t maskInt() const { return rawInt & imsc; }

#endif
  
    /** Wrapper to create an event from this device. */
    EventFunctionWrapper intEvent;

  protected: // Configuration
    /** Should the simulation end on an EOT */


    ArmInterruptPin* const interrupt;

    /** Delay before interrupting */
    const Tick intDelay = 10;
};

} // namespace gem5

#endif //__DEV_ARM_TTPU101_H__
