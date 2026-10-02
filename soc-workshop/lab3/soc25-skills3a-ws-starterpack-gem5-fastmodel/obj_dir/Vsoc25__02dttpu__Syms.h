// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VSOC25__02DTTPU__SYMS_H_
#define VERILATED_VSOC25__02DTTPU__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vsoc25__02dttpu.h"

// INCLUDE MODULE CLASSES
#include "Vsoc25__02dttpu___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)Vsoc25__02dttpu__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vsoc25__02dttpu* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vsoc25__02dttpu___024root      TOP;

    // SCOPE NAMES
    VerilatedScope __Vscope_SOC25_TTPU101;

    // CONSTRUCTORS
    Vsoc25__02dttpu__Syms(VerilatedContext* contextp, const char* namep, Vsoc25__02dttpu* modelp);
    ~Vsoc25__02dttpu__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
