//
//  soc25_ttpu_verilator_main.cpp
//
//
#define VL_TIME_STAMP64 1
#include "verilated.h"
#include "obj_dir/Vsoc25__02dttpu.h"

uint64_t global_time = 0;
uint64_t vl_time_stamp64()
{
  return global_time;
}

int main(int argc, char** argv)
{
  int clock_count = 0;
  VerilatedContext* contextp = new VerilatedContext;
  contextp->commandArgs(argc, argv);
  auto top = new Vsoc25__02dttpu{contextp};
  top->reset = 1;
  while (!contextp->gotFinish())
    {
      top->clk = 1;
      top->eval(); global_time += 500; contextp->timeInc(500);
      top->clk = 0;
      top->eval(); global_time += 500; contextp->timeInc(500);
      if (clock_count == 4) top->reset = 0;
      if (clock_count++ > 100) break;
    }
  printf("Exit on clock count reached\n");
  delete top;
  delete contextp;
  return 0;

}

//eof
