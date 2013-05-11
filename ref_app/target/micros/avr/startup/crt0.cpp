///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2007 - 2013.
//  Distributed under the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <mcal/mcal.h>

namespace crt
{
  void init_ram()   __attribute__ ((section(".init4")));
  void init_ctors() __attribute__ ((section(".init6")));
}

extern "C" void startup() __attribute__ ((section(".init0")));

extern "C" void startup()
{
  // Load the SREG register.
  asm volatile ("eor  r1, r1");
  asm volatile ("out  0x3F, r1");

  // Load the stack pointer.
  // Note that __stack is defined in the linker definition file.
  asm volatile ("ldi r28, lo8(__stack)");
  asm volatile ("ldi r29, hi8(__stack)");

  // Load the SPH register (stack pointer high).
  asm volatile ("out 0x3E, r29");

  // Load the SPL register (stack pointer low).
  asm volatile ("out 0x3D, r28");

  // Chip init: Port, oscillator and watchdog.
  mcal::cpu::init();

  // Initialize statics from ROM to RAM.
  // Zero-clear non-initialized static RAM.
  crt::init_ram();
  mcal::wdg::trigger();

  // Call all ctor initializations.
  crt::init_ctors();
  mcal::wdg::trigger();

  // Call main (and never return).
  asm volatile("call main");

  // Catch an unexpected return from main.
  for(;;)
  {
    // Replace with a loud error if desired.
    mcal::wdg::trigger();
  }
}
