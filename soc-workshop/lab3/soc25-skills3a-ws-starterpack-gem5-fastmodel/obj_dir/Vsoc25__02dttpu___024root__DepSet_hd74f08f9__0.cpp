// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vsoc25__02dttpu.h for the primary calling header

#include "Vsoc25__02dttpu__pch.h"
#include "Vsoc25__02dttpu__Syms.h"
#include "Vsoc25__02dttpu___024root.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vsoc25__02dttpu___024root___dump_triggers__ico(Vsoc25__02dttpu___024root* vlSelf);
#endif  // VL_DEBUG

void Vsoc25__02dttpu___024root___eval_triggers__ico(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_triggers__ico\n"); );
    // Body
    vlSelf->__VicoTriggered.set(0U, (IData)(vlSelf->__VicoFirstIteration));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vsoc25__02dttpu___024root___dump_triggers__ico(vlSelf);
    }
#endif
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vsoc25__02dttpu___024root___dump_triggers__act(Vsoc25__02dttpu___024root* vlSelf);
#endif  // VL_DEBUG

void Vsoc25__02dttpu___024root___eval_triggers__act(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___eval_triggers__act\n"); );
    // Body
    vlSelf->__VactTriggered.set(0U, ((IData)(vlSelf->clk) 
                                     & (~ (IData)(vlSelf->__Vtrigprevexpr___TOP__clk__0))));
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = vlSelf->clk;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vsoc25__02dttpu___024root___dump_triggers__act(vlSelf);
    }
#endif
}

extern const VlUnpacked<IData/*16:0*/, 128> Vsoc25__02dttpu__ConstPool__TABLE_hfc0749c2_0;

VL_INLINE_OPT void Vsoc25__02dttpu___024root___nba_sequent__TOP__0(Vsoc25__02dttpu___024root* vlSelf) {
    if (false && vlSelf) {}  // Prevent unused
    Vsoc25__02dttpu__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vsoc25__02dttpu___024root___nba_sequent__TOP__0\n"); );
    // Init
    CData/*6:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    // Body
    VL_WRITEF("%NSOC25_TTPU101: %1t clocked\n",vlSymsp->name(),
              64,VL_TIME_UNITED_Q(1),-12);
    if (vlSelf->reset) {
        vlSelf->SOC25_TTPU101__DOT__credit_counter = 0xfU;
        vlSelf->SOC25_TTPU101__DOT__income_counter = 0U;
        vlSelf->SOC25_TTPU101__DOT__badf = 0U;
    } else {
        vlSelf->SOC25_TTPU101__DOT__income_counter 
            = (0xfffffU & ((IData)(1U) + vlSelf->SOC25_TTPU101__DOT__income_counter));
        if (((0U == vlSelf->SOC25_TTPU101__DOT__income_counter) 
             & (0xfU > (IData)(vlSelf->SOC25_TTPU101__DOT__credit_counter)))) {
            vlSelf->SOC25_TTPU101__DOT__credit_counter 
                = (0xfU & ((IData)(1U) + (IData)(vlSelf->SOC25_TTPU101__DOT__credit_counter)));
        }
        if (((((IData)(vlSelf->sel) & (~ (IData)(vlSelf->rwbar))) 
              & (1U == (IData)(vlSelf->addr))) & (0U 
                                                  == (IData)(vlSelf->SOC25_TTPU101__DOT__credit_counter)))) {
            vlSelf->SOC25_TTPU101__DOT__badf = 1U;
        } else if ((((IData)(vlSelf->sel) & (~ (IData)(vlSelf->rwbar))) 
                    & (1U == (IData)(vlSelf->addr)))) {
            vlSelf->SOC25_TTPU101__DOT__credit_counter 
                = (0xfU & ((IData)(vlSelf->SOC25_TTPU101__DOT__credit_counter) 
                           - (IData)(1U)));
            vlSelf->SOC25_TTPU101__DOT__outdata_reg 
                = (0xffffU & (vlSelf->SOC25_TTPU101__DOT__OneTimeROM__DOT__data 
                              ^ vlSelf->dbus_in));
            vlSelf->SOC25_TTPU101__DOT__badf = 0U;
            if ((IData)(((vlSelf->SOC25_TTPU101__DOT__OneTimeROM__DOT__data 
                          >> 0x10U) | (0x1111U == (0x1111U 
                                                   & vlSelf->dbus_in))))) {
                vlSelf->SOC25_TTPU101__DOT__hidden_state 
                    = (0x7fU & ((IData)(1U) + (IData)(vlSelf->SOC25_TTPU101__DOT__hidden_state)));
            }
        }
    }
    vlSelf->dbus_out = ((1U == (IData)(vlSelf->addr))
                         ? (((IData)(vlSelf->SOC25_TTPU101__DOT__badf) 
                             << 0x1fU) | (IData)(vlSelf->SOC25_TTPU101__DOT__outdata_reg))
                         : ((2U == (IData)(vlSelf->addr))
                             ? (IData)(vlSelf->SOC25_TTPU101__DOT__credit_counter)
                             : 0xa5a4U));
    __Vtableidx1 = vlSelf->SOC25_TTPU101__DOT__hidden_state;
    vlSelf->SOC25_TTPU101__DOT__OneTimeROM__DOT__data 
        = Vsoc25__02dttpu__ConstPool__TABLE_hfc0749c2_0
        [__Vtableidx1];
}
