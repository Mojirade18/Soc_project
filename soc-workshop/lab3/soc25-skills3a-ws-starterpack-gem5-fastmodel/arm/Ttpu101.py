#// Ttpu101.py - (C) 2025 DJ Greaves, University of Cambridge.
#// GEM5 C++ python metainfo for toy trusted processing unit 101 model.


# Also, add this line to the local SConscript
# SimObject('Ttpu101.py', sim_objects=['Ttpu101'], tags='arm isa') 

from m5.objects.Ttpu import Ttpu 
from m5.params import *


class Ttpu101(Ttpu): ## cbg
    type = "Ttpu101"
    cxx_header = "dev/arm/ttpu101.hh"
    cxx_class = "gem5::Ttpu101"
    interrupt = Param.ArmInterruptPin("Interrupt that connects to GIC")    

    def generateDeviceTree(self, state):
        node = self.generateBasicPioDeviceNode(
            state, "ttpu", self.pio_addr, 0x1000, []
        )
        node.appendCompatible(["arm,ttpu101", "arm,not_primecell"])

        # Hardcoded reference to the realview platform clocks, because the
        # clk_domain can only store one clock (i.e. it is not a VectorParam)
        realview = self._parent.unproxy(self)
        node.append(
            FdtPropertyWords(
                "clocks",
                [
                    state.phandle(realview.mcc.osc_peripheral),
                    state.phandle(realview.dcc.osc_smb),
                ],
            )
        )
        node.append(FdtPropertyStrings("clock-names", ["apb_pclk"]))
        yield node

# eof
