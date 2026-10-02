// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vsoc25__02dttpu.h for the primary calling header

#include "Vsoc25__02dttpu__pch.h"
#include "Vsoc25__02dttpu___024root.h"

VL_INLINE_OPT void Vsoc25__02dttpu___024root___ico_sequent__TOP__0(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___ico_sequent__TOP__0\n"); );
    // Body
    vlSelf->dbus_out = ((1U == (IData)(vlSelf->addr))
                         ? (((IData)(vlSelf->SOC25_TTPU101__DOT__badf) 
                             << 0x1fU) | (IData)(vlSelf->SOC25_TTPU101__DOT__outdata_reg))
                         : ((2U == (IData)(vlSelf->addr))
                             ? (IData)(vlSelf->SOC25_TTPU101__DOT__credit_counter)
                             : 0xa5a4U));
}

void Vsoc25__02dttpu___024root___eval_ico(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_ico\n"); );
    // Body
    if ((1ULL & vlSelf->__VicoTriggered.word(0U))) {
        Vsoc25__02dttpu___024root___ico_sequent__TOP__0(vlSelf);
    }
}

void Vsoc25__02dttpu___024root___eval_triggers__ico(Vsoc25__02dttpu___024root* vlSelf);

bool Vsoc25__02dttpu___024root___eval_phase__ico(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_phase__ico\n"); );
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vsoc25__02dttpu___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelf->__VicoTriggered.any();
    if (__VicoExecute) {
        Vsoc25__02dttpu___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vsoc25__02dttpu___024root___eval_act(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_act\n"); );
}

void Vsoc25__02dttpu___024root___nba_sequent__TOP__0(Vsoc25__02dttpu___024root* vlSelf);

void Vsoc25__02dttpu___024root___eval_nba(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_nba\n"); );
    // Body
    if ((1ULL & vlSelf->__VnbaTriggered.word(0U))) {
        Vsoc25__02dttpu___024root___nba_sequent__TOP__0(vlSelf);
    }
}

void Vsoc25__02dttpu___024root___eval_triggers__act(Vsoc25__02dttpu___024root* vlSelf);

bool Vsoc25__02dttpu___024root___eval_phase__act(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_phase__act\n"); );
    // Init
    VlTriggerVec<1> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vsoc25__02dttpu___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelf->__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelf->__VactTriggered, vlSelf->__VnbaTriggered);
        vlSelf->__VnbaTriggered.thisOr(vlSelf->__VactTriggered);
        Vsoc25__02dttpu___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vsoc25__02dttpu___024root___eval_phase__nba(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_phase__nba\n"); );
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelf->__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vsoc25__02dttpu___024root___eval_nba(vlSelf);
        vlSelf->__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vsoc25__02dttpu___024root___dump_triggers__ico(Vsoc25__02dttpu___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vsoc25__02dttpu___024root___dump_triggers__nba(Vsoc25__02dttpu___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vsoc25__02dttpu___024root___dump_triggers__act(Vsoc25__02dttpu___024root* vlSelf);
#endif  // VL_DEBUG

void Vsoc25__02dttpu___024root___eval(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval\n"); );
    // Init
    IData/*31:0*/ __VicoIterCount;
    CData/*0:0*/ __VicoContinue;
    IData/*31:0*/ __VnbaIterCount;
    CData/*0:0*/ __VnbaContinue;
    // Body
    __VicoIterCount = 0U;
    vlSelf->__VicoFirstIteration = 1U;
    __VicoContinue = 1U;
    while (__VicoContinue) {
        if (VL_UNLIKELY((0x64U < __VicoIterCount))) {
#ifdef VL_DEBUG
            Vsoc25__02dttpu___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("soc25-ttpu.v", 11, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vsoc25__02dttpu___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelf->__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY((0x64U < __VnbaIterCount))) {
#ifdef VL_DEBUG
            Vsoc25__02dttpu___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("soc25-ttpu.v", 11, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelf->__VactIterCount = 0U;
        vlSelf->__VactContinue = 1U;
        while (vlSelf->__VactContinue) {
            if (VL_UNLIKELY((0x64U < vlSelf->__VactIterCount))) {
#ifdef VL_DEBUG
                Vsoc25__02dttpu___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("soc25-ttpu.v", 11, "", "Active region did not converge.");
            }
            vlSelf->__VactIterCount = ((IData)(1U) 
                                       + vlSelf->__VactIterCount);
            vlSelf->__VactContinue = 0U;
            if (Vsoc25__02dttpu___024root___eval_phase__act(vlSelf)) {
                vlSelf->__VactContinue = 1U;
            }
        }
        if (Vsoc25__02dttpu___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vsoc25__02dttpu___024root___eval_debug_assertions(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_debug_assertions\n"); );
    // Body
    if (VL_UNLIKELY((vlSelf->clk & 0xfeU))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY((vlSelf->reset & 0xfeU))) {
        Verilated::overWidthError("reset");}
    if (VL_UNLIKELY((vlSelf->rwbar & 0xfeU))) {
        Verilated::overWidthError("rwbar");}
    if (VL_UNLIKELY((vlSelf->sel & 0xfeU))) {
        Verilated::overWidthError("sel");}
    if (VL_UNLIKELY((vlSelf->addr & 0xfcU))) {
        Verilated::overWidthError("addr");}
}
#endif  // VL_DEBUG
