#include "sam.h"
#include <assert.h>
#include "../dcc_stdio.h"
#include "../logger.h"

int main (void)
{
  uint32_t myCount = 200;
  unsigned long test_u32 = 0x01234567;

#ifndef NDEBUG
  for (int i=0; i<100000; i++)
  ;
  // include the following line if you want to simulate the 'standard' stop on entry behaviour
  // WARNING: this will always breakpoint, even when not in a debugger. Meaning that your code will never execute if outside a debugger.
  //__BKPT(0);
#endif

  logInit();

  // some example debug message calls
#ifndef NDEBUG
  dbg_write_str("hello world");

  dbg_write_char('t');
  dbg_write_char('e');
  dbg_write_char('s');
  dbg_write_char('t');
  dbg_write_char('\n');

  dbg_write_u32(&test_u32, 1);

  static const unsigned short test_u16[] = {0x0123, 0x4567, 0x89AB, 0xCDEF, 0x0123, 0x4567, 0x89AB, 0xCDEF};
  dbg_write_u16(test_u16, 8);

  static const unsigned char test_u8[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0XDD, 0xEE, 0xFF};
  dbg_write_u8(test_u8, 16);
#endif

  // an example logging call. NOTE: check logger.h for usage, don't think it's just like printf()!
  logMsg("Here's that 32-bit value %8f", test_u32);

  // sleep until we have an interrupt
  while (1) 
  {
/* uncomment the line below to trace every time this code is reached
 * WARNING: making this call induces a SIGNIFICANT delay, use sparingly and wisely (if at all)
 *          calling it every time you wake is **not** wise
 * to show how often the trace point was hit use the following debugger command:
 * monitor trace point
*/
//    dbg_trace_point(0);

    // demo an assertion firing into the debugger
    assert(myCount != 250);
  }
}
