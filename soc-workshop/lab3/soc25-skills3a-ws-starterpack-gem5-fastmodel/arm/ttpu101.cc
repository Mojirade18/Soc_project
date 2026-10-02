
// ttpu101.cc - (C) 2025 DJ Greaves, University of Cambridge.
// GEM5 C++ wrapper for the Toy trusted processing unit C model.



/*
 * Copyright (c) 2010, 2015 ARM Limited
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

#include "dev/arm/ttpu101.hh"

#include "base/trace.hh"
#include "debug/Checkpoint.hh"
#include "debug/Ttpu.hh"
#include "dev/arm/amba_device.hh"
#include "dev/arm/base_gic.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"
#include "sim/sim_exit.hh"


// This demonstration is somewhat longwinded, but of interest.  We started with a model that was
//   1/ coded in C (eg also usable for QEMU)
//   2/ coded with net-level connections - it was quite low-level as 'high-level' models go!

//  Higher-level modelling styles would be to either:
//   a/ implement the C model behaviour directly here in the C++ ttpu101 class, or
//   b/ implement the model in SystemC and make net level calls to it (slow) 
//   c/ implement the model in SystemC TLM 2.0 and redirect these method calls to it (fast)
//  NB - recent versions of GEM5 support TLM calling.



// Include the C-codeed model directly in this C++
#include "soc25-ttpu.c"

namespace gem5
{

  
  // This code is a  'transactor' using TLM modelling terminology.
  // Here it converts from TLM calling to a net-level model.

  void SOC25_TTPU_CBG_write(SOC25_TTPU_CBG_state *state, Addr offset, uint32_t value, unsigned int size)
  {
    unsigned char /*bool*/ reset = (offset==16);  // Implement a reset if writing to offset 16.
    unsigned char /*bool*/ rwbar = 0; // Read/write bar = false for write.
    unsigned char /*bool*/ sel = 1;   // Select asserted
    unsigned char /*[1:0]*/addr = offset/4; // Convert to word address offset.
    unsigned int  /*[31:0]*/dbus_in = value; // Low 32 bits are written in.
    SOC25_TTPU101_eval(state, reset, rwbar, sel, addr, dbus_in, 0); // Call the device model, making a write.
    //printf("SOC25_TTPU_CBG_write: addr=0x%lX := %X  size=%i  reset=%i\n", offset, value, size, reset);    
  }
  
  // Another transactor.
  uint64_t SOC25_TTPU_CBG_read(SOC25_TTPU_CBG_state *state, Addr offset, unsigned int size)
  {
    //    printf("SOC25_TTPU_CBG_read: addr=0x%lX size=%i\n", offset, size);    
    unsigned char /*bool*/ reset = 0;     // Active high reset deassert
    unsigned char /*bool*/ rwbar = 1; // Read/write bar = true for read.
    unsigned char /*bool*/ sel = 1;   // Select asserted
    unsigned char /*[1:0]*/oaddr = offset/4; // Convert to word address offset.
    unsigned int  /*[31:0]*/dbus_in = 0; // Data bus in is unused on a read
    unsigned int  /*[31:0]*/ dbus_out = 0x2122224; // Readily identifiable pattern.
    SOC25_TTPU101_eval(state, reset, rwbar, sel, oaddr, dbus_in, &dbus_out); // Call the device model
    return dbus_out;
  }


  
  // Toy TPU constructor
  Ttpu101::Ttpu101(const TtpuParams &p)
    : Ttpu(p, 0x1000),
      intEvent([this]{ generateInterrupt(); }, name()),
      interrupt(0 /*for now p.interrupt->get() */)
  {
    SOC25_TTPU_CBG_write(&ttpu_state, 16, 0, 0);         // Perform a reset.
  }

  // GEM5 PIO read method
  Tick Ttpu101::read(PacketPtr pkt)
  {
    assert(pkt->getAddr() >= pioAddr && pkt->getAddr() < pioAddr + pioSize);
    Addr daddr = pkt->getAddr() - pioAddr;
    unsigned int size = pkt->getSize();
    DPRINTF(Ttpu, "ttpu101: read register %#x size=%d\n", daddr, pkt->getSize());
    printf("ttpu101: read register %lx size=%d\n", daddr, size);    
    
    uint32_t data = 0;
    switch(daddr) {
    case 32: // A secondary signature register
      data = 0x2000DEAD;
      break;
	
    default:
      data = SOC25_TTPU_CBG_read(&ttpu_state, daddr, size);      
    break;
    }

    pkt->setUintX(data, ByteOrder::little);
    pkt->makeAtomicResponse();
    return pioDelay;
  }
  

  // GEM5 PIO write method
  Tick Ttpu101::write(PacketPtr pkt)
  {
    assert(pkt->getAddr() >= pioAddr && pkt->getAddr() < pioAddr + pioSize);
    unsigned int size = pkt->getSize();
    Addr daddr = pkt->getAddr() - pioAddr;
    uint32_t wdata = pkt->getLE<uint32_t>();
    printf("ttpu101: write register %lx value %x size=%d\n", daddr, wdata, size);
    DPRINTF(Ttpu, " ttpu101: write register %#x value %#x size=%d\n", daddr, pkt->getLE<uint8_t>(), pkt->getSize());
    SOC25_TTPU_CBG_write(&ttpu_state, daddr, wdata, size);        
    pkt->makeAtomicResponse();
    return pioDelay;
  }

#if 0
  void Ttpu101::dataAvailable()
  {
    DPRINTF(Ttpu, "Data available, scheduling interrupt\n");
    //raiseInterrupts(UART_RXINTR | UART_RTINTR);
  }
#endif
  void Ttpu101::generateInterrupt()
  {
#if 0
    DPRINTF(Ttpu, "Generate Interrupt: imsc=0x%x rawInt=0x%x maskInt=0x%x\n",
	    imsc, rawInt, maskInt());
    
  if (maskInt()) {
    interrupt->raise();
    DPRINTF(Ttpu, " -- Generated\n");
  }
#endif  
  }

void Ttpu101::setInterrupts(uint16_t ints, uint16_t mask)
{
  #if 0
    const bool old_ints(!!maskInt());

    imsc = mask;
    rawInt = ints;

    if (!old_ints && maskInt()) {
        if (!intEvent.scheduled())
            schedule(intEvent, curTick() + intDelay);
    } else if (old_ints && !maskInt()) {
        interrupt->clear();
    }
#endif
}


// For saving and reloading a long simulation: the internal state must be serialised.
void Ttpu101::serialize(CheckpointOut &cp) const
{
    DPRINTF(Checkpoint, "Serializing Ttpu101\n");
}

void Ttpu101::unserialize(CheckpointIn &cp)
{
    DPRINTF(Checkpoint, "Unserializing Ttpu101\n");
}

} // namespace gem5

// eof
