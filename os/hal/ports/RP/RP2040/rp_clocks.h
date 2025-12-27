/*
    ChibiOS - Copyright (C) 2006..2021 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    RP2040/rp_clocks.h
 * @brief   RP2040 clocks driver header.
 *
 * @addtogroup RP_CLOCKS
 * @{
 */

#ifndef RP_CLOCKS_H
#define RP_CLOCKS_H

#include "rp_xosc.h"
#include "rp_pll.h"

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @name    PLL_SYS configuration
 * @{
 */
#if !defined(RP_PLL_SYS_REFDIV) || defined(__DOXYGEN__)
#define RP_PLL_SYS_REFDIV               1U
#endif
#if !defined(RP_PLL_SYS_VCO_FREQ) || defined(__DOXYGEN__)
#define RP_PLL_SYS_VCO_FREQ             1500000000U     /* 1500 MHz VCO */
#endif
#if !defined(RP_PLL_SYS_POSTDIV1) || defined(__DOXYGEN__)
#define RP_PLL_SYS_POSTDIV1             6U
#endif
#if !defined(RP_PLL_SYS_POSTDIV2) || defined(__DOXYGEN__)
#define RP_PLL_SYS_POSTDIV2             2U
#endif
/** @} */

/**
 * @name    PLL_USB configuration
 * @{
 */
#if !defined(RP_PLL_USB_REFDIV) || defined(__DOXYGEN__)
#define RP_PLL_USB_REFDIV               1U
#endif
#if !defined(RP_PLL_USB_VCO_FREQ) || defined(__DOXYGEN__)
#define RP_PLL_USB_VCO_FREQ             1200000000U     /* 1200 MHz VCO */
#endif
#if !defined(RP_PLL_USB_POSTDIV1) || defined(__DOXYGEN__)
#define RP_PLL_USB_POSTDIV1             5U
#endif
#if !defined(RP_PLL_USB_POSTDIV2) || defined(__DOXYGEN__)
#define RP_PLL_USB_POSTDIV2             5U
#endif
/** @} */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/**
 * @brief   PLL_SYS output frequency.
 */
#define RP_PLL_SYS_CLK  (RP_PLL_SYS_VCO_FREQ / (RP_PLL_SYS_POSTDIV1 * RP_PLL_SYS_POSTDIV2))

/**
 * @brief   PLL_USB output frequency (must be 48 MHz for USB).
 */
#define RP_PLL_USB_CLK  (RP_PLL_USB_VCO_FREQ / (RP_PLL_USB_POSTDIV1 * RP_PLL_USB_POSTDIV2))

/**
 * @brief   RTC clock divisor (from PLL_USB).
 */
#define RP_RTC_CLK_DIV  1024U

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void rp_clocks_init(void);
  uint32_t rp_clock_get_hz(uint32_t clk_index);
#ifdef __cplusplus
}
#endif

#endif /* RP_CLOCKS_H */

/** @} */
