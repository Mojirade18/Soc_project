// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vsoc25__02dttpu.h for the primary calling header

#include "Vsoc25__02dttpu__pch.h"
#include "Vsoc25__02dttpu__Syms.h"
#include "Vsoc25__02dttpu___024root.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vsoc25__02dttpu___024root___dump_triggers__stl(Vsoc25__02dttpu___024root* vlSelf);
#endif  // VL_DEBUG

VL_ATTR_COLD void Vsoc25__02dttpu___024root___eval_triggers__stl(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_triggers__stl\n"); );
    // Body
    vlSelf->__VstlTriggered.set(0U, (IData)(vlSelf->__VstlFirstIteration));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vsoc25__02dttpu___024root___dump_triggers__stl(vlSelf);
    }
#endif
}
