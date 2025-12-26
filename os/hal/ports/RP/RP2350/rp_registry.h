/*
    ChibiOS - Copyright (C) 2006..2024 Giovanni Di Sirio

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
 * @file    RP2350/rp_registry.h
 * @brief   RP2350 capabilities registry.
 *
 * @addtogroup HAL
 * @{
 */

#ifndef RP_REGISTRY_H
#define RP_REGISTRY_H

/*===========================================================================*/
/* Platform capabilities.                                                    */
/*===========================================================================*/

/**
 * @name    RP2350 capabilities
 * @{
 */

/*===========================================================================*/
/* Common.                                                                   */
/*===========================================================================*/

/*===========================================================================*/
/* RP2350.                                                                   */
/*===========================================================================*/

/* GPIO attributes.*/
/**
 * @brief   Number of GPIO lines on Bank 0.
 * @note    RP2350 has 48 GPIO lines total (0-47), but common packages
 *          like the Pico 2 only expose GPIO 0-29 (30 lines).
 *          This value can be overridden in mcuconf.h if more lines are needed.
 */
#if !defined(RP_GPIO_NUM_LINES) || defined(__DOXYGEN__)
#define RP_GPIO_NUM_LINES                   30
#endif

/* UART attributes.*/
#define RP_HAS_UART0                        TRUE
#define RP_HAS_UART1                        TRUE

/* PIO attributes.*/
#define RP_HAS_PIO0                         TRUE
#define RP_HAS_PIO1                         TRUE
#define RP_HAS_PIO2                         TRUE

/* TIMER attributes.*/
#define RP_HAS_TIMER0                       TRUE
#define RP_HAS_TIMER1                       TRUE
#define RP_ST_NUM_ALARMS                    8

/* RTC attributes.*/
#define RP_HAS_RTC                          FALSE

/* SPI attributes.*/
#define RP_HAS_SPI0                         TRUE
#define RP_HAS_SPI1                         TRUE

/* I2C attributes.*/
#define RP_HAS_I2C0                         TRUE
#define RP_HAS_I2C1                         TRUE

/* DMA attributes.*/
#define RP_HAS_DMA                          TRUE
#define RP_DMA_NUM_CHANNELS                 16

/* WDG attributes.*/
#define RP_HAS_WDG                          TRUE
#define RP_WDG_STORAGE_SIZE                 32U

/** @} */

#endif /* RP_REGISTRY_H */

/** @} */
