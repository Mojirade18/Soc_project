// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vsoc25__02dttpu.h for the primary calling header

#ifndef VERILATED_VSOC25__02DTTPU___024ROOT_H_
#define VERILATED_VSOC25__02DTTPU___024ROOT_H_  // guard

#include "verilated.h"


class Vsoc25__02dttpu__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vsoc25__02dttpu___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clk,0,0);
    VL_IN8(reset,0,0);
    VL_IN8(rwbar,0,0);
    VL_IN8(sel,0,0);
    VL_IN8(addr,1,0);
    CData/*3:0*/ SOC25_TTPU101__DOT__credit_counter;
    CData/*6:0*/ SOC25_TTPU101__DOT__hidden_state;
    CData/*0:0*/ SOC25_TTPU101__DOT__badf;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VicoFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
    CData/*0:0*/ __VactContinue;
    SData/*15:0*/ SOC25_TTPU101__DOT__outdata_reg;
    VL_IN(dbus_in,31,0);
    VL_OUT(dbus_out,31,0);
    IData/*19:0*/ SOC25_TTPU101__DOT__income_counter;
    IData/*16:0*/ SOC25_TTPU101__DOT__OneTimeROM__DOT__data;
    IData/*31:0*/ __VactIterCount;
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<1> __VactTriggered;
    VlTriggerVec<1> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vsoc25__02dttpu__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vsoc25__02dttpu___024root(Vsoc25__02dttpu__Syms* symsp, const char* v__name);
    ~Vsoc25__02dttpu___024root();
    VL_UNCOPYABLE(Vsoc25__02dttpu___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
