// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vsoc25__02dttpu.h for the primary calling header

#include "Vsoc25__02dttpu__pch.h"
#include "Vsoc25__02dttpu___024root.h"

VL_ATTR_COLD void Vsoc25__02dttpu___024root___eval_static(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_static\n"); );
}

VL_ATTR_COLD void Vsoc25__02dttpu___024root___eval_initial(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_initial\n"); );
    // Body
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = vlSelf->clk;
}

VL_ATTR_COLD void Vsoc25__02dttpu___024root___eval_final(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_final\n"); );
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vsoc25__02dttpu___024root___dump_triggers__stl(Vsoc25__02dttpu___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vsoc25__02dttpu___024root___eval_phase__stl(Vsoc25__02dttpu___024root* vlSelf);

VL_ATTR_COLD void Vsoc25__02dttpu___024root___eval_settle(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_settle\n"); );
    // Init
    IData/*31:0*/ __VstlIterCount;
    CData/*0:0*/ __VstlContinue;
    // Body
    __VstlIterCount = 0U;
    vlSelf->__VstlFirstIteration = 1U;
    __VstlContinue = 1U;
    while (__VstlContinue) {
        if (VL_UNLIKELY((0x64U < __VstlIterCount))) {
#ifdef VL_DEBUG
            Vsoc25__02dttpu___024root___dump_triggers__stl(vlSelf);
#endif
            VL_FATAL_MT("soc25-ttpu.v", 11, "", "Settle region did not converge.");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        __VstlContinue = 0U;
        if (Vsoc25__02dttpu___024root___eval_phase__stl(vlSelf)) {
            __VstlContinue = 1U;
        }
        vlSelf->__VstlFirstIteration = 0U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vsoc25__02dttpu___024root___dump_triggers__stl(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___dump_triggers__stl\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VstlTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VstlTriggered.word(0U))) {
        VL_DBG_MSGF("         'stl' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

extern const VlUnpacked<IData/*16:0*/, 128> Vsoc25__02dttpu__ConstPool__TABLE_hfc0749c2_0;

VL_ATTR_COLD void Vsoc25__02dttpu___024root___stl_sequent__TOP__0(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___stl_sequent__TOP__0\n"); );
    // Init
    CData/*6:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    // Body
    __Vtableidx1 = vlSelf->SOC25_TTPU101__DOT__hidden_state;
    vlSelf->SOC25_TTPU101__DOT__OneTimeROM__DOT__data 
        = Vsoc25__02dttpu__ConstPool__TABLE_hfc0749c2_0
        [__Vtableidx1];
    vlSelf->dbus_out = ((1U == (IData)(vlSelf->addr))
                         ? (((IData)(vlSelf->SOC25_TTPU101__DOT__badf) 
                             << 0x1fU) | (IData)(vlSelf->SOC25_TTPU101__DOT__outdata_reg))
                         : ((2U == (IData)(vlSelf->addr))
                             ? (IData)(vlSelf->SOC25_TTPU101__DOT__credit_counter)
                             : 0xa5a4U));
}

VL_ATTR_COLD void Vsoc25__02dttpu___024root___eval_stl(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_stl\n"); );
    // Body
    if ((1ULL & vlSelf->__VstlTriggered.word(0U))) {
        Vsoc25__02dttpu___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD void Vsoc25__02dttpu___024root___eval_triggers__stl(Vsoc25__02dttpu___024root* vlSelf);

VL_ATTR_COLD bool Vsoc25__02dttpu___024root___eval_phase__stl(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_phase__stl\n"); );
    // Init
    CData/*0:0*/ __VstlExecute;
    // Body
    Vsoc25__02dttpu___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = vlSelf->__VstlTriggered.any();
    if (__VstlExecute) {
        Vsoc25__02dttpu___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vsoc25__02dttpu___024root___dump_triggers__ico(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___dump_triggers__ico\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VicoTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VicoTriggered.word(0U))) {
        VL_DBG_MSGF("         'ico' region trigger index 0 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vsoc25__02dttpu___024root___dump_triggers__act(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___dump_triggers__act\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VactTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 0 is active: @(posedge clk)\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vsoc25__02dttpu___024root___dump_triggers__nba(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___dump_triggers__nba\n"); );
    // Body
    if ((1U & (~ (IData)(vlSelf->__VnbaTriggered.any())))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelf->__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 0 is active: @(posedge clk)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vsoc25__02dttpu___024root___ctor_var_reset(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___ctor_var_reset\n"); );
    // Body
    vlSelf->clk = VL_RAND_RESET_I(1);
    vlSelf->reset = VL_RAND_RESET_I(1);
    vlSelf->rwbar = VL_RAND_RESET_I(1);
    vlSelf->sel = VL_RAND_RESET_I(1);
    vlSelf->addr = VL_RAND_RESET_I(2);
    vlSelf->dbus_in = VL_RAND_RESET_I(32);
    vlSelf->dbus_out = VL_RAND_RESET_I(32);
    vlSelf->SOC25_TTPU101__DOT__income_counter = VL_RAND_RESET_I(20);
    vlSelf->SOC25_TTPU101__DOT__credit_counter = VL_RAND_RESET_I(4);
    vlSelf->SOC25_TTPU101__DOT__hidden_state = VL_RAND_RESET_I(7);
    vlSelf->SOC25_TTPU101__DOT__outdata_reg = VL_RAND_RESET_I(16);
    vlSelf->SOC25_TTPU101__DOT__badf = VL_RAND_RESET_I(1);
    vlSelf->SOC25_TTPU101__DOT__OneTimeROM__DOT__data = VL_RAND_RESET_I(17);
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = VL_RAND_RESET_I(1);
}
