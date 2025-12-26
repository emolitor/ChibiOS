/*
 * Minimal config for ChibiOS Pico SDK bindings.
 */

#if defined(PICO_RP2350) && PICO_RP2350
  #define LIB_PICO_STDIO_UART 0
  #define LIB_PICO_STDIO_USB 0
  #define LIB_PICO_STDIO_SEMIHOSTING 0
  #define HAS_RP2040_RTC 0
  #define LIB_PICO_PRINTF_PICO 0
  #define PICO_MUTEX_ENABLE_SDK120_COMPATIBILITY 0
  #define LIB_PICO_STDIO_RTT 0
  #define PICO_STDIO_IGNORE_NESTED_STDOUT 0
  #define LIB_PICO_PRINTF_NONE 0
  #define LIB_PICO_BINARY_INFO 0
  #define __ARM_ARCH_6M__ 0
#else
  #define HAS_GPIO_COPROCESSOR 0
  #define HAS_HSTX 0
  #define NUM_BOOT_LOCKS 0
#endif

#define LIB_FREERTOS_KERNEL 0

#ifndef LIB_PICO_MULTICORE
#define LIB_PICO_MULTICORE 0
#endif

#define PICO_BOOTROM_LOCKING_ENABLED 0
#define PICO_COMBINED_DOCS 0

#ifndef PICO_SECURE
#define PICO_SECURE 0
#endif

#ifndef PICO_USE_GPIO_COPROCESSOR
#define PICO_USE_GPIO_COPROCESSOR 0
#endif

#define PICO_IE_26_29_UNCHANGED_ON_RESET 0
#define PICO_TIME_DEFAULT_ALARM_POOL_DISABLED 0
#define PICO_USE_STACK_GUARDS 0

#ifndef PICO_NO_FLASH
#define PICO_NO_FLASH 0
#endif

/* Include the appropriate board header */
#if PICO_RP2350
#include "boards/pico2.h"
#else
#include "boards/pico.h"
#endif
