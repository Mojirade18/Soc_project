#!/bin/bash
# add-ttpu-to-gem5.sh
# Adds the SoC-25 Lab 3 toy TPU (TTPU101) to a CURRENT gem5 source tree.
# It copies the lecturer's device files and makes small edits to gem5's own
# files, instead of overwriting them (the lecturer's copies of RealView.py and
# arm/SConscript come from an older gem5 and would break the build).
#
# Run it from inside the unzipped Lab 3 starter pack folder:
#     bash add-ttpu-to-gem5.sh
set -e
GEM5=${GEM5:-$HOME/gem5}
SRC=$GEM5/src/dev

[ -f misc/ttpu.cc ] && [ -f arm/ttpu101.cc ] || { echo "ERROR: run this inside the Lab 3 starter pack folder"; exit 1; }
[ -f $SRC/arm/RealView.py ] || { echo "ERROR: gem5 not found at $GEM5"; exit 1; }

# 1. New folder for the generic (base) TTPU class.
mkdir -p $SRC/misc
cp misc/SConscript misc/Ttpu.py misc/ttpu.cc misc/ttpu.hh $SRC/misc/
echo "Copied base class into  $SRC/misc"

# 2. The specific TTPU101 device and its C model.
cp arm/Ttpu101.py arm/ttpu101.cc arm/ttpu101.hh arm/soc25-ttpu.c arm/soc25-ttpu.h $SRC/arm/
echo "Copied TTPU101 files into $SRC/arm"

# 3 + 4. Small edits to gem5's own files (backups kept as *.orig).
python3 - "$SRC" << 'PYEOF'
import sys, shutil, os
src = sys.argv[1]

# 3. arm/SConscript: register the new SimObject and its C++ source.
p = os.path.join(src, "arm", "SConscript")
s = open(p).read()
if "Ttpu101" not in s:
    shutil.copy(p, p + ".orig")
    s = s.rstrip("\n") + (
        "\n\n# SoC-25 Lab 3: toy trusted processing unit\n"
        "SimObject('Ttpu101.py', sim_objects=['Ttpu101'], tags=['arm isa'])\n"
        "Source('ttpu101.cc', tags=['arm isa'])\n")
    open(p, "w").write(s)
    print("Edited  ", p)
else:
    print("Already edited", p)

# 4. arm/RealView.py: import the classes, add an instance at 0x1C0D0000,
#    and put it in the list of off-chip devices.
p = os.path.join(src, "arm", "RealView.py")
s = open(p).read()
if "Ttpu101" not in s:
    shutil.copy(p, p + ".orig")
    anchor = "from m5.objects.Terminal import Terminal\n"
    assert anchor in s, "import anchor not found"
    s = s.replace(anchor, anchor +
        "from m5.objects.Ttpu import Ttpu  ## soc25\n"
        "from m5.objects.Ttpu101 import Ttpu101  ## soc25\n", 1)

    anchor = "    watchdog = Sp805(pio_addr=0x1C0F0000, interrupt=ArmSPI(num=32))\n"
    assert s.count(anchor) == 1, "instance anchor not unique"
    s = s.replace(anchor,
        "    ttpu = Ttpu101(pio_addr=0x1C0D0000, interrupt=ArmSPI(num=41))  ## soc25\n\n"
        + anchor, 1)

    anchor = ("            self.vio[0],\n"
              "            self.vio[1],\n"
              "        ] + self.uart\n")
    assert s.count(anchor) == 1, "device list anchor not unique"
    s = s.replace(anchor,
        "            self.vio[0],\n"
        "            self.vio[1],\n"
        "            self.ttpu,  ## soc25\n"
        "        ] + self.uart\n", 1)
    open(p, "w").write(s)
    print("Edited  ", p)
else:
    print("Already edited", p)
PYEOF

echo
echo "Done. Now rebuild gem5:"
echo "  cd ~/gem5 && scons build/ARM/gem5.opt -j2 PYTHON_CONFIG=/usr/bin/python3-config"
