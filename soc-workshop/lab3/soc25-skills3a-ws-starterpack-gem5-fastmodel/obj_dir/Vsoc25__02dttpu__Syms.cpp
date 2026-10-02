// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vsoc25__02dttpu__pch.h"
#include "Vsoc25__02dttpu.h"
#include "Vsoc25__02dttpu___024root.h"

// FUNCTIONS
Vsoc25__02dttpu__Syms::~Vsoc25__02dttpu__Syms()
{
}

Vsoc25__02dttpu__Syms::Vsoc25__02dttpu__Syms(VerilatedContext* contextp, const char* namep, Vsoc25__02dttpu* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP{this, namep}
{
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-12);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
    // Setup scopes
    __Vscope_SOC25_TTPU101.configure(this, name(), "SOC25_TTPU101", "SOC25_TTPU101", -12, VerilatedScope::SCOPE_OTHER);
}
