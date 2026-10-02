// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vsoc25__02dttpu__pch.h"

//============================================================
// Constructors

Vsoc25__02dttpu::Vsoc25__02dttpu(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vsoc25__02dttpu__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , reset{vlSymsp->TOP.reset}
    , rwbar{vlSymsp->TOP.rwbar}
    , sel{vlSymsp->TOP.sel}
    , addr{vlSymsp->TOP.addr}
    , dbus_in{vlSymsp->TOP.dbus_in}
    , dbus_out{vlSymsp->TOP.dbus_out}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vsoc25__02dttpu::Vsoc25__02dttpu(const char* _vcname__)
    : Vsoc25__02dttpu(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vsoc25__02dttpu::~Vsoc25__02dttpu() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vsoc25__02dttpu___024root___eval_debug_assertions(Vsoc25__02dttpu___024root* vlSelf);
#endif  // VL_DEBUG
void Vsoc25__02dttpu___024root___eval_static(Vsoc25__02dttpu___024root* vlSelf);
void Vsoc25__02dttpu___024root___eval_initial(Vsoc25__02dttpu___024root* vlSelf);
void Vsoc25__02dttpu___024root___eval_settle(Vsoc25__02dttpu___024root* vlSelf);
void Vsoc25__02dttpu___024root___eval(Vsoc25__02dttpu___024root* vlSelf);

void Vsoc25__02dttpu::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vsoc25__02dttpu::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vsoc25__02dttpu___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vsoc25__02dttpu___024root___eval_static(&(vlSymsp->TOP));
        Vsoc25__02dttpu___024root___eval_initial(&(vlSymsp->TOP));
        Vsoc25__02dttpu___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vsoc25__02dttpu___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vsoc25__02dttpu::eventsPending() { return false; }

uint64_t Vsoc25__02dttpu::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "%Error: No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vsoc25__02dttpu::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vsoc25__02dttpu___024root___eval_final(Vsoc25__02dttpu___024root* vlSelf);

VL_ATTR_COLD void Vsoc25__02dttpu::final() {
    Vsoc25__02dttpu___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vsoc25__02dttpu::hierName() const { return vlSymsp->name(); }
const char* Vsoc25__02dttpu::modelName() const { return "Vsoc25__02dttpu"; }
unsigned Vsoc25__02dttpu::threads() const { return 1; }
void Vsoc25__02dttpu::prepareClone() const { contextp()->prepareClone(); }
void Vsoc25__02dttpu::atClone() const {
    contextp()->threadPoolpOnClone();
}

//============================================================
// Trace configuration

VL_ATTR_COLD void Vsoc25__02dttpu::trace(VerilatedVcdC* tfp, int levels, int options) {
    vl_fatal(__FILE__, __LINE__, __FILE__,"'Vsoc25__02dttpu::trace()' called on model that was Verilated without --trace option");
}
