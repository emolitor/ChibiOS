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
 * @file    rp2350_imagedef.S
 * @brief   RP2350 PICOBIN boot block definitions.
 *
 * @details This file contains the imagedef and end blocks required by
 *          the RP2350 bootrom to identify and boot the application.
 *
 * @addtogroup ARMCMx_RP2350_IMAGEDEF
 * @{
 */

/*===========================================================================*/
/* PICOBIN block constants from pico-sdk boot/picobin.h                      */
/*===========================================================================*/

/* Block markers */
#define PICOBIN_BLOCK_MARKER_START              0xffffded3
#define PICOBIN_BLOCK_MARKER_END                0xab123579

/* Block item type IDs (1 byte size field) */
#define PICOBIN_BLOCK_ITEM_1BS_IMAGE_TYPE       0x42
#define PICOBIN_BLOCK_ITEM_1BS_VECTOR_TABLE     0x03
#define PICOBIN_BLOCK_ITEM_1BS_ENTRY_POINT      0x44
#define PICOBIN_BLOCK_ITEM_2BS_LAST             0xff

/* Image type field values (matching pico-sdk picobin.h bit positions) */
#define PICOBIN_IMAGE_TYPE_IMAGE_TYPE_EXE       0x0001  /* bits 0-3: type = 1 (exe) */
#define PICOBIN_IMAGE_TYPE_EXE_CPU_ARM          0x0000  /* bits 8-10: cpu = 0 (ARM) */
#define PICOBIN_IMAGE_TYPE_EXE_CPU_RISCV        0x0100  /* bits 8-10: cpu = 1 (RISC-V) */
#define PICOBIN_IMAGE_TYPE_EXE_CHIP_RP2350      0x1000  /* bits 12-14: chip = 1 (RP2350) */
#define PICOBIN_IMAGE_TYPE_EXE_SECURITY_NS      0x0010  /* bits 4-5: security = 1 (NS) */
#define PICOBIN_IMAGE_TYPE_EXE_SECURITY_S       0x0020  /* bits 4-5: security = 2 (S) */

/*
 * RP2350 image type: ARM executable, RP2350 chip, secure mode.
 *
 * Security Mode Selection:
 * ------------------------
 * The RP2350 supports ARM TrustZone with two security states:
 *
 *   PICOBIN_IMAGE_TYPE_EXE_SECURITY_S  (0x0020) - Secure mode (default)
 *   PICOBIN_IMAGE_TYPE_EXE_SECURITY_NS (0x0010) - Non-Secure mode
 *
 * Secure mode (default):
 *   - Full access to all peripherals and memory regions
 *   - Required for bare-metal applications without TrustZone partitioning
 *   - The bootrom runs in Secure mode and jumps directly to the application
 *   - This is the correct choice for most ChibiOS applications
 *
 * Non-Secure mode:
 *   - Restricted access based on SAU/IDAU configuration
 *   - Used when a Secure bootloader or monitor configures TrustZone and
 *     then launches the application in Non-Secure state via BXNS
 *   - Requires additional TrustZone setup (SAU regions, secure gateways)
 *
 * To use Non-Secure mode, define RP2350_IMAGE_TYPE before including this
 * file (e.g., in your Makefile or a header included before startup):
 *
 *   #define RP2350_IMAGE_TYPE  (PICOBIN_IMAGE_TYPE_IMAGE_TYPE_EXE |    \
 *                               PICOBIN_IMAGE_TYPE_EXE_CPU_ARM |       \
 *                               PICOBIN_IMAGE_TYPE_EXE_CHIP_RP2350 |   \
 *                               PICOBIN_IMAGE_TYPE_EXE_SECURITY_NS)
 */
#if !defined(RP2350_IMAGE_TYPE) || defined(__DOXYGEN__)
#define RP2350_IMAGE_TYPE       (PICOBIN_IMAGE_TYPE_IMAGE_TYPE_EXE |    \
                                 PICOBIN_IMAGE_TYPE_EXE_CPU_ARM |       \
                                 PICOBIN_IMAGE_TYPE_EXE_CHIP_RP2350 |   \
                                 PICOBIN_IMAGE_TYPE_EXE_SECURITY_S)
#endif

/*===========================================================================*/
/* Imagedef block - placed at start of flash                                 */
/*===========================================================================*/

#if !defined(__DOXYGEN__)

                .syntax unified
                .thumb

/*
 * PICOBIN image definition block.
 * This minimal block (20 bytes) is placed at the beginning of the flash image.
 * The bootrom searches for this block within the first 4KB of flash.
 *
 * Block structure:
 *   - MARKER_START (4 bytes)
 *   - IMAGE_TYPE item (4 bytes): type=0x42, size=1, flags=RP2350_IMAGE_TYPE
 *   - LAST item (4 bytes): type=0xff, size=1
 *   - Link to next block (4 bytes): 0 = no next block
 *   - MARKER_END (4 bytes)
 *
 * Note: VECTOR_TABLE item is optional - the bootrom determines the entry
 * point from the ELF. This simplified format matches bare-metal examples.
 */
                .section .embedded_block, "a", %progbits
                .align  2
                .global __embedded_block
__embedded_block:
                /* Block marker start */
                .word   PICOBIN_BLOCK_MARKER_START

                /* IMAGE_TYPE item: type 0x42, 1 word of data */
                .byte   PICOBIN_BLOCK_ITEM_1BS_IMAGE_TYPE
                .byte   0x01                    /* 1 word of data */
                .hword  RP2350_IMAGE_TYPE       /* Image type flags */

                /* LAST item: marks end of block items */
                .byte   PICOBIN_BLOCK_ITEM_2BS_LAST
                .byte   0x01                    /* Block size in words (1) */
                .hword  0x0000                  /* Padding */

                /* Link to next block: 0 = no next block (single block image) */
                .word   0x00000000

                /* Block marker end */
                .word   PICOBIN_BLOCK_MARKER_END
__embedded_block_end:

/*
 * Provide symbol for linker compatibility (end block no longer needed).
 */
                .global __embedded_end_block
                .set    __embedded_end_block, __embedded_block_end

#endif /* !defined(__DOXYGEN__) */

/** @} */
