
## Generic definition for a new family of devices: toy TPUs.
## (C) 2025 DJ Greaves, University of Cambridge, UK.
## soc25 cbg

from m5.defines import buildEnv
from m5.objects.Device import BasicPioDevice
from m5.objects.Serial import SerialDevice
from m5.params import *
from m5.proxy import *
from m5.util.fdthelper import *


class Ttpu(BasicPioDevice):
    type = "Ttpu"
    abstract = True
    cxx_header = "dev/misc/ttpu.hh"
    cxx_class = "gem5::Ttpu"
    platform = Param.Platform(Parent.any, "Platform this device is part of.")
    device = Param.SerialDevice(Parent.any, "A trusted processing/platform unit / TPU")



# eof

