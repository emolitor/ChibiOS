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
 * @file    RP2040/rp_clocks.c
 * @brief   RP2040 clocks driver source.
 *
 * @addtogroup RP_CLOCKS
 * @{
 */

#include "hal.h"
#include "rp_clocks.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/**
 * @brief   Configured clock frequencies cache.
 */
static uint32_t configured_freq[RP_CLK_COUNT];

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/**
 * @brief   Check if clock has glitchless mux.
 */
static inline bool has_glitchless_mux(uint32_t clk_index) {

  return (clk_index == RP_CLK_SYS) || (clk_index == RP_CLK_REF);
}

/**
 * @brief   Configure a clock.
 *
 * @param[in] clk_index     clock index
 * @param[in] src           source selector for glitchless mux
 * @param[in] auxsrc        auxiliary source selector
 * @param[in] src_freq      source frequency in Hz
 * @param[in] freq          desired output frequency in Hz
 */
static void clock_configure(uint32_t clk_index, uint32_t src, uint32_t auxsrc,
                            uint32_t src_freq, uint32_t freq) {

  CLOCKS_CLK_TypeDef *clk = &CLOCKS->CLK[clk_index];
  uint32_t div;

  osalDbgAssert(src_freq >= freq, "source frequency too low");

  /* Calculate divider (8.8 fixed point). */
  if (freq == src_freq) {
    div = 1U << 8;  /* Divider of 1. */
  }
  else {
    div = (uint32_t)(((uint64_t)src_freq << 8) / freq);
  }

  /* If increasing divisor, set divisor before source. */
  if (div > clk->DIV) {
    clk->DIV = div;
  }

  /* If switching a glitchless slice to an aux source, switch away from aux
     first to avoid glitches when changing aux mux. */
  if (has_glitchless_mux(clk_index) && (src == CLOCKS_CLK_SYS_CTRL_SRC_AUX)) {
    clk->CTRL &= ~CLOCKS_CLK_REF_CTRL_SRC_Msk;
    while ((clk->SELECTED & 1U) == 0U) {
      /* Wait for switch. */
    }
  }
  else if (!has_glitchless_mux(clk_index)) {
    /* For non-glitchless clocks, disable before switching. */
    clk->CTRL &= ~CLOCKS_CLK_PERI_CTRL_ENABLE;
    if (configured_freq[clk_index] > 0U) {
      /* Delay for 3 cycles of the target clock. */
      uint32_t delay_cyc = configured_freq[RP_CLK_SYS] / configured_freq[clk_index] + 1U;
      for (volatile uint32_t i = 0U; i < delay_cyc * 3U; i++) {
        __NOP();
      }
    }
  }

  /* Set aux mux first, then glitchless mux if present. */
  clk->CTRL = (clk->CTRL & ~CLOCKS_CLK_SYS_CTRL_AUXSRC_Msk) |
              (auxsrc << CLOCKS_CLK_SYS_CTRL_AUXSRC_Pos);

  if (has_glitchless_mux(clk_index)) {
    clk->CTRL = (clk->CTRL & ~CLOCKS_CLK_SYS_CTRL_SRC_Msk) |
                (src << CLOCKS_CLK_SYS_CTRL_SRC_Pos);
    while ((clk->SELECTED & (1U << src)) == 0U) {
      /* Wait for switch. */
    }
  }

  /* Enable clock (for non-glitchless clocks). */
  if (!has_glitchless_mux(clk_index)) {
    clk->CTRL |= CLOCKS_CLK_PERI_CTRL_ENABLE;
  }

  /* Now set the final divisor. */
  clk->DIV = div;

  /* Store configured frequency. */
  configured_freq[clk_index] = freq;
}

/**
 * @brief   Configure a clock with no divider (1:1).
 */
static void clock_configure_undivided(uint32_t clk_index, uint32_t src,
                                       uint32_t auxsrc, uint32_t freq) {

  clock_configure(clk_index, src, auxsrc, freq, freq);
}

/**
 * @brief   Configure a clock with integer divider.
 */
static void clock_configure_int_divider(uint32_t clk_index, uint32_t src,
                                         uint32_t auxsrc, uint32_t src_freq,
                                         uint32_t int_divider) {

  CLOCKS_CLK_TypeDef *clk = &CLOCKS->CLK[clk_index];
  (void)src;  /* Only auxsrc is used for non-glitchless clocks. */
  uint32_t freq = src_freq / int_divider;
  uint32_t div = int_divider << 8;

  /* For non-glitchless clocks, disable before configuring. */
  if (!has_glitchless_mux(clk_index)) {
    clk->CTRL &= ~CLOCKS_CLK_PERI_CTRL_ENABLE;
    if (configured_freq[clk_index] > 0U) {
      uint32_t delay_cyc = configured_freq[RP_CLK_SYS] / configured_freq[clk_index] + 1U;
      for (volatile uint32_t i = 0U; i < delay_cyc * 3U; i++) {
        __NOP();
      }
    }
  }

  /* Set divisor first. */
  clk->DIV = div;

  /* Set aux mux. */
  clk->CTRL = (clk->CTRL & ~CLOCKS_CLK_SYS_CTRL_AUXSRC_Msk) |
              (auxsrc << CLOCKS_CLK_SYS_CTRL_AUXSRC_Pos);

  /* Enable clock. */
  if (!has_glitchless_mux(clk_index)) {
    clk->CTRL |= CLOCKS_CLK_PERI_CTRL_ENABLE;
  }

  configured_freq[clk_index] = freq;
}

/**
 * @brief   Start the watchdog tick generator.
 * @note    On RP2040, this provides the tick reference for watchdog, timer,
 *          and SysTick.
 */
static void start_tick(void) {

  /* Calculate cycles for 1us tick based on clk_ref frequency. */
  uint32_t cycles = configured_freq[RP_CLK_REF] / 1000000U;

  /* Enable the tick generator. */
  WATCHDOG->TICK = WATCHDOG_TICK_ENABLE | WATCHDOG_TICK_CYCLES(cycles);
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Initializes all clocks.
 * @note    This replaces the Pico SDK's runtime_init_clocks().
 */
void rp_clocks_init(void) {

  /* Disable clock resus that may be enabled from previous software. */
  CLOCKS->RESUS.CTRL = 0U;

  /* Initialize XOSC (12 MHz crystal). */
  rp_xosc_init();

  /* Before touching PLLs, switch clk_sys and clk_ref to safe sources. */
  CLOCKS->CLK[RP_CLK_SYS].CTRL &= ~CLOCKS_CLK_SYS_CTRL_SRC_Msk;
  while ((CLOCKS->CLK[RP_CLK_SYS].SELECTED & 1U) == 0U) {
    /* Wait for clk_sys to switch to clk_ref. */
  }
  CLOCKS->CLK[RP_CLK_REF].CTRL &= ~CLOCKS_CLK_REF_CTRL_SRC_Msk;
  while ((CLOCKS->CLK[RP_CLK_REF].SELECTED & 1U) == 0U) {
    /* Wait for clk_ref to switch to ROSC. */
  }

  /* Initialize PLL_SYS: 12 MHz * 125 / 6 / 2 = 125 MHz. */
  rp_pll_init(PLL_SYS, RP_PLL_SYS_REFDIV, RP_PLL_SYS_VCO_FREQ,
              RP_PLL_SYS_POSTDIV1, RP_PLL_SYS_POSTDIV2);

  /* Initialize PLL_USB: 12 MHz * 100 / 5 / 5 = 48 MHz. */
  rp_pll_init(PLL_USB, RP_PLL_USB_REFDIV, RP_PLL_USB_VCO_FREQ,
              RP_PLL_USB_POSTDIV1, RP_PLL_USB_POSTDIV2);

  /* CLK_REF = XOSC (12 MHz). */
  clock_configure_undivided(RP_CLK_REF,
                            CLOCKS_CLK_REF_CTRL_SRC_XOSC >> CLOCKS_CLK_REF_CTRL_SRC_Pos,
                            0U,  /* No aux mux used. */
                            RP_XOSCCLK);

  /* CLK_SYS = PLL_SYS (125 MHz). */
  clock_configure_undivided(RP_CLK_SYS,
                            CLOCKS_CLK_SYS_CTRL_SRC_AUX >> CLOCKS_CLK_SYS_CTRL_SRC_Pos,
                            CLOCKS_CLK_SYS_CTRL_AUXSRC_PLL_SYS >> CLOCKS_CLK_SYS_CTRL_AUXSRC_Pos,
                            RP_PLL_SYS_CLK);

  /* CLK_USB = PLL_USB (48 MHz). */
  clock_configure_undivided(RP_CLK_USB,
                            0U,  /* No glitchless mux. */
                            CLOCKS_CLK_USB_CTRL_AUXSRC_PLL_USB >> CLOCKS_CLK_USB_CTRL_AUXSRC_Pos,
                            RP_PLL_USB_CLK);

  /* CLK_ADC = PLL_USB (48 MHz). */
  clock_configure_undivided(RP_CLK_ADC,
                            0U,  /* No glitchless mux. */
                            CLOCKS_CLK_ADC_CTRL_AUXSRC_PLL_USB >> CLOCKS_CLK_ADC_CTRL_AUXSRC_Pos,
                            RP_PLL_USB_CLK);

  /* CLK_RTC = PLL_USB / 1024 (46875 Hz). */
  clock_configure_int_divider(RP_CLK_RTC,
                               0U,  /* No glitchless mux. */
                               CLOCKS_CLK_RTC_CTRL_AUXSRC_PLL_USB >> CLOCKS_CLK_RTC_CTRL_AUXSRC_Pos,
                               RP_PLL_USB_CLK,
                               RP_RTC_CLK_DIV);

  /* CLK_PERI = CLK_SYS (125 MHz). */
  clock_configure_undivided(RP_CLK_PERI,
                            0U,  /* No glitchless mux. */
                            CLOCKS_CLK_PERI_CTRL_AUXSRC_SYS >> CLOCKS_CLK_PERI_CTRL_AUXSRC_Pos,
                            RP_PLL_SYS_CLK);

  /* Start the tick generator. */
  start_tick();
}

/**
 * @brief   Returns the frequency of a clock in Hz.
 *
 * @param[in] clk_index     clock index (RP_CLK_xxx)
 * @return                  clock frequency in Hz
 */
uint32_t rp_clock_get_hz(uint32_t clk_index) {

  osalDbgAssert(clk_index < RP_CLK_COUNT, "invalid clock index");

  return configured_freq[clk_index];
}

/** @} */
