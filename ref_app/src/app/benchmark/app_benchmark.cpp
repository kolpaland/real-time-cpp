
///////////////////////////////////////////////////////////////////////////////
//  Copyright Christopher Kormanyos 2007 - 2015.
//  Distributed under the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <array>
#include <cstdint>
#include <math/checksums/crc/crc32.h>
#include <mcal_benchmark.h>
#include <mcal_cpu.h>
#include <mcal_irq.h>

#define APP_BENCHMARK_TYPE_NONE     0
#define APP_BENCHMARK_TYPE_CRC32    1

#define APP_BENCHMARK_TYPE   APP_BENCHMARK_TYPE_NONE
//#define APP_BENCHMARK_TYPE   APP_BENCHMARK_TYPE_CRC32

namespace app
{
  namespace benchmark
  {
    void task_init();
    void task_func();
  }
}

void app::benchmark::task_init()
{
  #if(APP_BENCHMARK_TYPE != APP_BENCHMARK_TYPE_NONE)

    mcal::benchmark::benchmark_port_type::set_direction_output();

  #endif
}

#if(APP_BENCHMARK_TYPE == APP_BENCHMARK_TYPE_CRC32)
  std::uint32_t app_benchmark_crc;
#endif

void app::benchmark::task_func()
{
  #if(APP_BENCHMARK_TYPE == APP_BENCHMARK_TYPE_CRC32)

    const std::array<std::uint8_t, 9U> data =
    {{
      0x31U, 0x32U, 0x33U, 0x34U, 0x35U, 0x36U, 0x37U, 0x38U, 0x39U
    }};

    typedef mcal::benchmark::benchmark_port_type port_type;

    mcal::irq::disable_all();
    port_type::set_pin_high();

    app_benchmark_crc = math::checksums::crc32_mpeg2(data.cbegin(),
                                                     data.cend());

    port_type::set_pin_low();
    mcal::irq::enable_all();

    if(app_benchmark_crc == UINT32_C(0x0376E6E7))
    {
      // The benchmark is OK.
      // Perform one nop and leave.

      mcal::cpu::nop();
    }
    else
    {
      // The benchmark result is not OK!
      // Remain in a blocking loop and crash the system.

      for(;;) { mcal::cpu::nop(); }
    }

  #endif // APP_BENCHMARK_TYPE == APP_BENCHMARK_TYPE_CRC32
}
