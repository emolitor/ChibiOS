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
 * @file    RP2040/rp_xosc.c
 * @brief   RP2040 XOSC driver source.
 *
 * @addtogroup RP_XOSC
 * @{
 */

#include "hal.h"
#include "rp_xosc.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported variables.                                                */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local variables and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   Initializes the XOSC (crystal oscillator).
 * @note    Assumes 1-15 MHz crystal is attached.
 */
void rp_xosc_init(void) {

  /* Set frequency range for 1-15 MHz crystal. */
  XOSC->CTRL = XOSC_CTRL_FREQ_RANGE_1_15MHZ;

  /* Set startup delay. */
  XOSC->STARTUP = RP_XOSC_STARTUP_DELAY;

  /* Enable the XOSC. */
  XOSC->CTRL |= XOSC_CTRL_ENABLE_ENABLE;

  /* Wait for XOSC to be stable. */
  while ((XOSC->STATUS & XOSC_STATUS_STABLE) == 0U) {
    /* Spin. */
  }
}

/**
 * @brief   Disables the XOSC.
 */
void rp_xosc_disable(void) {
  uint32_t tmp;

  /* Disable XOSC. */
  tmp = XOSC->CTRL;
  tmp &= ~XOSC_CTRL_ENABLE_Msk;
  tmp |= XOSC_CTRL_ENABLE_DISABLE;
  XOSC->CTRL = tmp;

  /* Wait for stable to go away. */
  while ((XOSC->STATUS & XOSC_STATUS_STABLE) != 0U) {
    /* Spin. */
  }
}

/** @} */
